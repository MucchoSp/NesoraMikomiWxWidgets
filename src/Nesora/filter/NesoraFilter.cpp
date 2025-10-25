#include "NesoraFilter.h"
#include <iostream>


void NesoraFormantFilter::AddParamater(double frequency, double bandwidth, double amplitude) {
    paramaters.push_back({frequency,bandwidth,amplitude});
}

const std::vector<NesoraFormantParam>& NesoraFormantFilter::GetParamater() const{
    return paramaters;
}

std::vector<NesoraFormantParam>& NesoraFormantFilter::GetParamater() {
    return paramaters;
}

void NesoraFormantFilter::SetParamater(const std::vector<NesoraFormantParam>& param) {
    paramaters = param;
}

std::vector<double> NesoraFormantFilter::GetFilter(double dx, size_t N) const {
    std::vector<double> out(N);
    for(size_t i = 0;i < N;i++) {
        out[i] = 0;
        for(size_t j = 0;j < paramaters.size();j++) {
            out[i] += paramaters[j].f1_amplitude * exp(-(0.5 * ((double)i * dx - paramaters[j].f1_frequency) * ((double)i * dx - paramaters[j].f1_frequency)) / (paramaters[j].f1_bandwidth * paramaters[j].f1_bandwidth));
        }
    }
    return out;
}

inline double integrand(double omega, double b, double sigma, double n) {
    return std::exp(-(omega - b) * (omega - b) / (2.0 * sigma * sigma)) * std::cos(omega * n);
}

inline double numerical_integration(double b, double sigma, double n, int N = 10000) {
    double delta_omega = nsPI / static_cast<double>(N);
    double sum = (integrand(0.0, b, sigma, n) + integrand(nsPI, b, sigma, n)) / 2.0;

    for (int k = 1; k < N; ++k) {
        double omega_k = k * delta_omega;
        sum += integrand(omega_k, b, sigma, n);
    }

    return sum * delta_omega;
}

double calculate_h_n(double a, double b, double sigma, double n) {
    if (sigma <= 0.0) {
        std::cerr << "Error: sigma must be greater than 0." << std::endl;
        return NAN; // Not a Number (計算不能)
    }

    double integral_result = numerical_integration(b, sigma, n);

    double coefficient = (a / nsPI) * std::exp((b * b - b) / (2.0 * sigma * sigma));
    
    return coefficient * integral_result;
}

std::vector<double> generate_filter_kernel(double a, double b, double sigma, int kernel_radius) {
    int kernel_size = 2 * kernel_radius + 1;
    std::vector<double> kernel(kernel_size);

    double sum = 0.0;

    for (int i = 0; i < kernel_size; ++i) {
        int n = i - kernel_radius;
        
        kernel[i] = calculate_h_n(a, b, sigma, static_cast<double>(n));
        sum += kernel[i];
    }
    
    if (sum != 0.0) {
        for (int i = 0; i < kernel_size; ++i) {
            kernel[i] /= sum;
        }
    }

    return kernel;
}

std::vector<double> convolve(const std::vector<double>& signal, const std::vector<double>& kernel) {
    
    int signal_len = signal.size();
    int kernel_len = kernel.size();
    
    // 出力信号の長さは (signal_len + kernel_len - 1)
    std::vector<double> output(signal_len + kernel_len - 1, 0.0);

    // 畳み込み計算
    // y[n] = sum_k (x[k] * h[n-k]) を計算する
    // (実装上は y[n] = sum_k (x[n-k] * h[k]) の方が書きやすい)
    
    for (int n = 0; n < output.size(); ++n) {
        for (int k = 0; k < kernel_len; ++k) {
            
            // x(n-k) に対応するインデックス
            int signal_index = n - k;
            
            // 元信号の範囲外 (0 未満 or signal_len 以上) じゃないかチェック
            if (signal_index >= 0 && signal_index < signal_len) {
                output[n] += signal[signal_index] * kernel[k];
            }
        }
    }
    
    return output;
}

void NesoraFormantFilter::GenerateKernel(){
    if(paramaters.size() == 0)
        return;
    
    // 各パラメータに基づいてカーネルを生成し、全てのカーネルを畳み込む
    std::vector<double> combined_kernel = {1.0}; // 単位インパルス応答で初期化

    for(const auto& param : paramaters) {
        double a = param.f1_amplitude;
        double b = param.f1_frequency;
        double sigma = param.f1_bandwidth;

        int kernel_radius = static_cast<int>(sigma * 4); // カーネルの半径を帯域幅の4倍に設定
        std::vector<double> kernel = generate_filter_kernel(a, b, sigma, kernel_radius);

        combined_kernel = convolve(combined_kernel, kernel);
    }

    kernel = combined_kernel;

    inputwave_buffer.resize(kernel.size(), 0.0);
}

double NesoraFormantFilter::Filter(double x) {
    if(kernel.size() == 0)
        GenerateKernel();
    
    inputwave_buffer.push_back(x);
    inputwave_buffer.erase(inputwave_buffer.begin());

    double output = 0.0;
    for(size_t i = 0;i < kernel.size();i++) {
        output += inputwave_buffer[inputwave_buffer.size() - 1 - i] * kernel[i];
    }

    return output;
}
