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
        for (size_t i = 0; i < a_coefficients.size(); ++i) {
            for (size_t j = 0; j < second_order.size(); ++j) {
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
        for (size_t i = 0; i < b_coefficients.size(); ++i) {
            for (size_t j = 0; j < second_order.size(); ++j) {
                new_b[i + j] += b_coefficients[i] * second_order[j];
            }
        }
        b_coefficients = new_b;
    }
}

std::vector<double> NesoraIIRFilter::CalculateFrequencyResponse(int num_samples) {
    response.clear();
    if (num_samples <= 0) return response;
    response.reserve(num_samples);

    for (int n = 0; n < num_samples; ++n) {
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
        val = std::log10(val + 1e-10) * 0.5; // Avoid log(0)
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
    for (size_t i = 1; i < b_coefficients.size(); ++i) {
        y += b_coefficients[i] * input_history[i - 1];
    }

    // 分母部分
    for (size_t i = 1; i < a_coefficients.size(); ++i) {
        y -= a_coefficients[i] * output_history[i - 1];
    }

    // 出力履歴の更新
    output_history.insert(output_history.begin(), y);
    if (output_history.size() > a_coefficients.size() - 1) {
        output_history.pop_back();
    }

    return y;
}



