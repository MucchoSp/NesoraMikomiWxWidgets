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
    response.reserve(num_samples);
    for (int n = 0; n < num_samples; ++n) {
        double omega = (M_PI * n) / num_samples;
        std::complex<double> numerator(0.0, 0.0);
        std::complex<double> denominator(0.0, 0.0);

        // 分子の計算
        for (size_t k = 0; k < b_coefficients.size(); ++k) {
            numerator += b_coefficients[k] * std::exp(std::complex<double>(0.0, -omega * k));
        }

        // 分母の計算
        for (size_t k = 0; k < a_coefficients.size(); ++k) {
            denominator += a_coefficients[k] * std::exp(std::complex<double>(0.0, -omega * k));
        }

        std::complex<double> H = numerator / denominator;
        response.push_back(std::abs(H));
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
