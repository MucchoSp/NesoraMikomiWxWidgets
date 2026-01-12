#include "NesoraParametricIIRFilter.h"


void NesoraParametricIIRFilter::SetCoefficients(const std::vector<double>& a_coeffs, const std::vector<double>& b_coeffs) {
    a_coefficients = a_coeffs;
    b_coefficients = b_coeffs;
    output_history.resize(a_coefficients.size() - 1, 0.0);
    input_history.resize(b_coefficients.size() - 1, 0.0);
}

std::vector<NesoraIIRFilterPD> NesoraParametricIIRFilter::GetPeaks() const {
    return peaks;
}

std::vector<NesoraIIRFilterPD>& NesoraParametricIIRFilter::GetPeaks() {
    return peaks;
}

std::vector<NesoraIIRFilterPD> NesoraParametricIIRFilter::GetDips() const {
    return dips;
}

std::vector<NesoraIIRFilterPD>& NesoraParametricIIRFilter::GetDips() {
    return dips;
}

void NesoraParametricIIRFilter::Reset() {
    std::fill(output_history.begin(), output_history.end(), 0.0);
    std::fill(input_history.begin(), input_history.end(), 0.0);
}

std::vector<double> NesoraParametricIIRFilter::GetResponse() const {
    return response;
}

void NesoraParametricIIRFilter::CalculateCoefficientsFromPDs(const std::map<int, double>& parameters) {
    // 分母係数の計算
    a_coefficients.assign(peaks.size() * 2 + 1, 0.0);
    a_coefficients[0] = 1.0; // 初期値
    std::vector<double> peaks_buffer(peaks.size() * 2 + 1, 0.0);
    for (size_t p = 0;p < peaks.size();p++) {
        double r = peaks[p].r;
        double theta = peaks[p].theta;
        for (const auto& param : parameters) {
            if (param.second != 0.0){ 
                r += peaks_paramater_status[param.first][p].delta_r * param.second;
                theta += peaks_paramater_status[param.first][p].delta_theta * param.second;
            }
        }
        std::fill(peaks_buffer.begin(), peaks_buffer.end(), 0.0);
        for (size_t i = 0; i < p * 2 + 1; i++) {
            peaks_buffer[i] += a_coefficients[i];
            peaks_buffer[i + 1] += a_coefficients[i] * -2.0 * r * std::cos(theta);
            peaks_buffer[i + 2] += a_coefficients[i] * r * r;
        }
        a_coefficients = peaks_buffer;
    }

    // 分子係数の計算
    b_coefficients.assign(dips.size() * 2 + 1, 0.0);
    b_coefficients[0] = 1.0; // 初期値
    std::vector<double> dips_buffer(dips.size() * 2 + 1, 0.0);
    for (size_t p = 0;p < dips.size();p++) {
        double r = dips[p].r;
        double theta = dips[p].theta;
        for (const auto& param : parameters) {
            if (param.second != 0.0){ 
                r += dips_paramater_status[param.first][p].delta_r * param.second;
                theta += dips_paramater_status[param.first][p].delta_theta * param.second;
            }
        }
        std::fill(dips_buffer.begin(), dips_buffer.end(), 0.0);
        for (size_t i = 0; i < p * 2 + 1; i++) {
            dips_buffer[i] += b_coefficients[i];
            dips_buffer[i + 1] += b_coefficients[i] * -2.0 * r * std::cos(theta);
            dips_buffer[i + 2] += b_coefficients[i] * r * r;
        }
        b_coefficients = dips_buffer;
    }
    double A1 = std::reduce(a_coefficients.begin(), a_coefficients.end());
    double B1 = std::reduce(b_coefficients.begin(), b_coefficients.end());
    Gain = (B1 == 0.0) ? 0.0 : A1 / B1;
    for (auto& bk : b_coefficients)
        bk = bk * Gain;
}

std::vector<double> NesoraParametricIIRFilter::CalculateFrequencyResponse(int num_samples) {
    if (num_samples <= 0) {
        response.clear();
        return response;
    }
    response.assign(num_samples, 0.0);

    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4; // デフォルト値

    auto worker = [&](int start, int end) {
        for (int n = 0; n < num_samples; n++) {
            double omega;
            if (num_samples == 1)
                omega = 0.0;
            else
                omega = nsPI * static_cast<double>(n) / static_cast<double>(num_samples - 1);

            // z^{-1} = e^{-j omega}
            std::complex<double> z_inv = std::exp(std::complex<double>(0.0, -omega));

            // Horner 法で多項式を評価（z^{-1} を変数として）
            auto eval_poly = [&](const std::vector<double>& coef) -> std::complex<double> {
                if (coef.empty()) return std::complex<double>(0.0, 0.0);
                std::complex<double> p = coef.back();
                for (size_t k = coef.size(); k >= 2; k--)
                    p = p * z_inv + coef[k];
                return p;
            };

            std::complex<double> numerator = eval_poly(b_coefficients);
            std::complex<double> denominator = eval_poly(a_coefficients);

            double mag;
            if (std::abs(denominator) < 1e-300)
                mag = std::numeric_limits<double>::infinity(); // or a very large number
            else
                mag = std::abs(numerator / denominator);
            
            response[n] = std::log10(mag + 1e-10) * 10.0; // dBへの変換
        }
    };

    // スレッドを起動
    std::vector<std::thread> threads;
    int chunk_size = num_samples / num_threads;
    
    for (unsigned int t = 0; t < num_threads; t++) {
        int start = t * chunk_size;
        int end = (t == num_threads - 1) ? num_samples : (t + 1) * chunk_size;
        threads.emplace_back(worker, start, end);
    }

    // 全スレッドの完了を待つ
    for (auto& thread : threads) {
        thread.join();
    }

    return response;
}

double NesoraParametricIIRFilter::Filter(const std::map<int, double>& parameters, double x) {
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
