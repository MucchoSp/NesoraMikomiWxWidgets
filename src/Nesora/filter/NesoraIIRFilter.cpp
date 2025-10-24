#include "NesoraIIRFilter.h"

void NesoraIIRFilter::SetCoefficients(const std::vector<std::complex<double>>& a_coeffs, const std::vector<std::complex<double>>& b_coeffs) {
    a_coefficients = a_coeffs;
    b_coefficients = b_coeffs;
    history.resize(a_coefficients.size() - 1, 0.0);
    output_history.resize(a_coefficients.size() - 1, 0.0);
    input_history.resize(b_coefficients.size() - 1, 0.0);
}

std::vector<std::complex<double>> NesoraIIRFilter::GetACoefficients() const {
    return a_coefficients;
}

std::vector<std::complex<double>>& NesoraIIRFilter::GetACoefficients() {
    return a_coefficients;
}

std::vector<std::complex<double>> NesoraIIRFilter::GetBCoefficients() const {
    return b_coefficients;
}

std::vector<std::complex<double>>& NesoraIIRFilter::GetBCoefficients() {
    return b_coefficients;
}

void NesoraIIRFilter::Reset() {
    std::fill(history.begin(), history.end(), 0.0);
    std::fill(output_history.begin(), output_history.end(), 0.0);
    std::fill(input_history.begin(), input_history.end(), 0.0);
}

std::vector<double> NesoraIIRFilter::CalculateFrequencyResponse(int num_samples) const {
    std::vector<double> response;
    if (num_samples <= 0) return response;
    response.reserve(num_samples);

    // 0..pi を num_samples 点でサンプリング（端点 pi を含む）
    for (int n = 0; n < num_samples; ++n) {
        double omega;
        if (num_samples == 1) omega = 0.0;
        else omega = M_PI * static_cast<double>(n) / static_cast<double>(num_samples - 1);

        // z^{-1} = e^{-j omega}
        std::complex<double> z_inv = std::exp(std::complex<double>(0.0, -omega));

        // Horner 法で多項式を評価（z^{-1} を変数として）
        auto eval_poly = [&](const std::vector<std::complex<double>>& coef) -> std::complex<double> {
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
    return response;
}

double NesoraIIRFilter::Filter(double x) {
    // 分子部分の計算
    double numerator = 0.0;
    numerator += b_coefficients[0].real() * x;
    for (size_t i = 0; i < input_history.size(); ++i) {
        numerator += b_coefficients[i + 1].real() * input_history[i];
    }

    // 分母部分の計算
    double denominator = 0.0;
    for (size_t i = 0; i < output_history.size(); ++i) {
        denominator += a_coefficients[i + 1].real() * output_history[i];
    }

    double y = (numerator - denominator) / a_coefficients[0].real();

    // 履歴の更新
    if (!input_history.empty()) {
        for (size_t i = input_history.size() - 1; i > 0; --i) {
            input_history[i] = input_history[i - 1];
        }
        input_history[0] = x;
    }

    if (!output_history.empty()) {
        for (size_t i = output_history.size() - 1; i > 0; --i) {
            output_history[i] = output_history[i - 1];
        }
        output_history[0] = y;
    }

    return y;
}
