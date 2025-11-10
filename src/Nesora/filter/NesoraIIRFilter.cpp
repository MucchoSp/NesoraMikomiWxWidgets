#include "NesoraIIRFilter.h"

void NesoraIIRFilter::SetCoefficients(const std::vector<double>& a_coeffs, const std::vector<double>& b_coeffs) {
    a_coefficients = a_coeffs;
    b_coefficients = b_coeffs;
    history.resize(a_coefficients.size() - 1, 0.0);
    output_history.resize(a_coefficients.size() - 1, 0.0);
    input_history.resize(b_coefficients.size() - 1, 0.0);
}

std::vector<NesoraIIRFilterPD> NesoraIIRFilter::GetPeaks() const {
    return peaks;
}

std::vector<NesoraIIRFilterPD>& NesoraIIRFilter::GetPeaks() {
    return peaks;
}

std::vector<NesoraIIRFilterPD> NesoraIIRFilter::GetDips() const {
    return dips;
}

std::vector<NesoraIIRFilterPD>& NesoraIIRFilter::GetDips() {
    return dips;
}

void NesoraIIRFilter::Reset() {
    std::fill(history.begin(), history.end(), 0.0);
    std::fill(output_history.begin(), output_history.end(), 0.0);
    std::fill(input_history.begin(), input_history.end(), 0.0);
}

std::vector<double> NesoraIIRFilter::GetResponse() const {
    return response;
}

void NesoraIIRFilter::CalculateCoefficientsFromPDs() {
    // 分母係数の計算
    a_coefficients = {1.0};
    for (const auto& peak : peaks) {
        double r = peak.r;
        double theta = peak.theta;
        std::vector<double> second_order = {1.0, -2.0 * r * std::cos(theta), r * r};
        std::vector<double> new_a(a_coefficients.size() + 2, 0.0);
        for (size_t i = 0; i < a_coefficients.size(); i++) {
            for (size_t j = 0; j < second_order.size(); j++) {
                new_a[i + j] += a_coefficients[i] * second_order[j];
            }
        }
        a_coefficients = new_a;
    }

    // 分子係数の計算
    b_coefficients = {1.0};
    for (const auto& dip : dips) {
        double r = dip.r;
        double theta = dip.theta;
        std::vector<double> second_order = {1.0, -2.0 * r * std::cos(theta), r * r};
        std::vector<double> new_b(b_coefficients.size() + 2, 0.0);
        for (size_t i = 0; i < b_coefficients.size(); i++) {
            for (size_t j = 0; j < second_order.size(); j++) {
                new_b[i + j] += b_coefficients[i] * second_order[j];
            }
        }
        b_coefficients = new_b;
    }
    double A1 = 0.0;
    for (const auto& ak : a_coefficients)
        A1 += ak;
    double B1 = 0.0;
    for (const auto& bk : b_coefficients)
        B1 += bk;
    Gain = A1 / B1;
    for (auto& bk : b_coefficients)
        bk = bk * Gain;
}

std::vector<double> NesoraIIRFilter::CalculateFrequencyResponse(int num_samples) {
    response.clear();
    if (num_samples <= 0) return response;
    response.reserve(num_samples);

    for (int n = 0; n < num_samples; n++) {
        double omega;
        if (num_samples == 1) omega = 0.0;
        else omega = nsPI * static_cast<double>(n) / static_cast<double>(num_samples - 1);

        // z^{-1} = e^{-j omega}
        std::complex<double> z_inv = std::exp(std::complex<double>(0.0, -omega));

        // Horner 法で多項式を評価（z^{-1} を変数として）
        auto eval_poly = [&](const std::vector<double>& coef) -> std::complex<double> {
            if (coef.empty()) return std::complex<double>(0.0, 0.0);
            std::complex<double> p = coef.back();
            for (int k = static_cast<int>(coef.size()) - 2; k >= 0; --k) {
                p = p * z_inv + coef[k];
            }
            return p;
        };

        std::complex<double> numerator = eval_poly(b_coefficients);
        std::complex<double> denominator = eval_poly(a_coefficients);

        double mag;
        if (std::abs(denominator) < 1e-300) {
            mag = std::numeric_limits<double>::infinity(); // or a very large number
        } else {
            mag = std::abs(numerator / denominator);
        }
        response.push_back(mag);
    }

    for (auto& val : response) {
        val = std::log10(val + 1e-10) * 10.0; // dBへの変換
    }

    return response;
}

double NesoraIIRFilter::Filter(double x) {
    // 入力履歴の更新
    input_history.insert(input_history.begin(), x);
    if (input_history.size() > b_coefficients.size() - 1) {
        input_history.pop_back();
    }

    // 出力計算
    double y = 0.0;

    // 分子部分
    y += b_coefficients[0] * x;
    for (size_t i = 1; i < std::min(b_coefficients.size(), input_history.size() + 1); i++) {
        y += b_coefficients[i] * input_history[i - 1];
    }

    // 分母部分
    for (size_t i = 1; i < std::min(a_coefficients.size(), output_history.size() + 1); i++) {
        y -= a_coefficients[i] * output_history[i - 1];
    }

    // 出力履歴の更新
    output_history.insert(output_history.begin(), y);
    if (output_history.size() > a_coefficients.size() - 1) {
        output_history.pop_back();
    }

    return y;
}

std::vector<unsigned char> NesoraIIRFilter::SaveData() {
    std::vector<unsigned char> data;
    // Save peaks (r, theta) and dips (r, theta) so UI control points can be restored
    size_t peaks_size = peaks.size();
    data.insert(data.end(), reinterpret_cast<unsigned char*>(&peaks_size), reinterpret_cast<unsigned char*>(&peaks_size) + sizeof(size_t));
    for (const auto& p : peaks) {
        data.insert(data.end(), reinterpret_cast<const unsigned char*>(&p.r), reinterpret_cast<const unsigned char*>(&p.r) + sizeof(double));
        data.insert(data.end(), reinterpret_cast<const unsigned char*>(&p.theta), reinterpret_cast<const unsigned char*>(&p.theta) + sizeof(double));
    }

    size_t dips_size = dips.size();
    data.insert(data.end(), reinterpret_cast<unsigned char*>(&dips_size), reinterpret_cast<unsigned char*>(&dips_size) + sizeof(size_t));
    for (const auto& d : dips) {
        data.insert(data.end(), reinterpret_cast<const unsigned char*>(&d.r), reinterpret_cast<const unsigned char*>(&d.r) + sizeof(double));
        data.insert(data.end(), reinterpret_cast<const unsigned char*>(&d.theta), reinterpret_cast<const unsigned char*>(&d.theta) + sizeof(double));
    }

    return data;
}

void NesoraIIRFilter::LoadData(const std::vector<unsigned char>& data) {
    size_t offset = 0;
    const size_t total = data.size();

    auto read_size = [&](size_t &out) -> bool {
        if (offset + sizeof(size_t) > total) return false;
        std::memcpy(&out, data.data() + offset, sizeof(size_t));
        offset += sizeof(size_t);
        return true;
    };

    // Load peaks
    size_t peaks_size = 0;
    if (!read_size(peaks_size)) {
        std::cout << "NesoraIIRFilter::LoadData: insufficient data for peaks_size" << std::endl;
        return;
    }
    if (offset + peaks_size * sizeof(double) * 2 > total) {
        std::cout << "NesoraIIRFilter::LoadData: peaks data exceeds buffer" << std::endl;
        return;
    }
    peaks.clear();
    for (size_t i = 0; i < peaks_size; i++) {
        NesoraIIRFilterPD p;
        std::memcpy(&p.r, data.data() + offset, sizeof(double));
        offset += sizeof(double);
        std::memcpy(&p.theta, data.data() + offset, sizeof(double));
        offset += sizeof(double);
        peaks.push_back(p);
    }

    // Load dips
    size_t dips_size = 0;
    if (!read_size(dips_size)) {
        std::cout << "NesoraIIRFilter::LoadData: insufficient data for dips_size" << std::endl;
        return;
    }
    if (offset + dips_size * sizeof(double) * 2 > total) {
        std::cout << "NesoraIIRFilter::LoadData: dips data exceeds buffer" << std::endl;
        return;
    }
    dips.clear();
    for (size_t i = 0; i < dips_size; i++) {
        NesoraIIRFilterPD d;
        std::memcpy(&d.r, data.data() + offset, sizeof(double));
        offset += sizeof(double);
        std::memcpy(&d.theta, data.data() + offset, sizeof(double));
        offset += sizeof(double);
        dips.push_back(d);
    }

    if (a_coefficients.size() >= 1) {
        history.resize(a_coefficients.size() - 1, 0.0);
        output_history.resize(a_coefficients.size() - 1, 0.0);
    } else {
        history.clear();
        output_history.clear();
    }
    if (b_coefficients.size() >= 1) {
        input_history.resize(b_coefficients.size() - 1, 0.0);
    } else {
        input_history.clear();
    }
    // Recalculate coefficients from PDs and prepare histories
    CalculateCoefficientsFromPDs();
    CalculateFrequencyResponse(512); // reasonable default until UI recalculates with actual width
}



