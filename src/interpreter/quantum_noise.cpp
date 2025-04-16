#include "quantum_noise.hpp"
#include <cmath>
#include <algorithm>

namespace qasm {

QuantumNoise::QuantumNoise(double error_rate) 
    : error_rate(error_rate), noise_type(NoiseType::DEPOLARIZING) {
    rng.seed(std::random_device()());
    dist = std::uniform_real_distribution<double>(0.0, 1.0);
    initializeNoiseMatrix();
}

QuantumNoise::~QuantumNoise() {}

void QuantumNoise::setErrorRate(double rate) {
    error_rate = rate;
    updateNoiseMatrix();
}

void QuantumNoise::setNoiseType(NoiseType type) {
    noise_type = type;
    updateNoiseMatrix();
}

void QuantumNoise::setCustomNoise(const std::vector<std::vector<std::complex<double>>>& noise_matrix) {
    this->noise_matrix = noise_matrix;
}

void QuantumNoise::initializeNoiseMatrix() {
    switch (noise_type) {
        case NoiseType::DEPOLARIZING: {
            // Depolarizing noise matrix
            noise_matrix = {
                {std::sqrt(1 - error_rate), 0},
                {0, std::sqrt(1 - error_rate)},
                {0, std::sqrt(error_rate/3)},
                {std::sqrt(error_rate/3), 0},
                {0, std::sqrt(error_rate/3)},
                {-std::sqrt(error_rate/3), 0},
                {std::sqrt(error_rate/3), 0},
                {0, -std::sqrt(error_rate/3)}
            };
            break;
        }
        case NoiseType::AMPLITUDE_DAMPING: {
            // Amplitude damping noise matrix
            double gamma = error_rate;
            noise_matrix = {
                {1, 0},
                {0, std::sqrt(1 - gamma)},
                {0, std::sqrt(gamma)},
                {0, 0}
            };
            break;
        }
        case NoiseType::PHASE_DAMPING: {
            // Phase damping noise matrix
            double lambda = error_rate;
            noise_matrix = {
                {1, 0},
                {0, std::sqrt(1 - lambda)},
                {0, 0},
                {0, std::sqrt(lambda)}
            };
            break;
        }
        case NoiseType::BIT_FLIP: {
            // Bit flip noise matrix
            noise_matrix = {
                {std::sqrt(1 - error_rate), 0},
                {0, std::sqrt(1 - error_rate)},
                {0, std::sqrt(error_rate)},
                {std::sqrt(error_rate), 0}
            };
            break;
        }
        case NoiseType::PHASE_FLIP: {
            // Phase flip noise matrix
            noise_matrix = {
                {std::sqrt(1 - error_rate), 0},
                {0, std::sqrt(1 - error_rate)},
                {std::sqrt(error_rate), 0},
                {0, -std::sqrt(error_rate)}
            };
            break;
        }
        case NoiseType::BIT_PHASE_FLIP: {
            // Combined bit and phase flip noise matrix
            noise_matrix = {
                {std::sqrt(1 - error_rate), 0},
                {0, std::sqrt(1 - error_rate)},
                {0, std::sqrt(error_rate)},
                {-std::sqrt(error_rate), 0}
            };
            break;
        }
    }
}

void QuantumNoise::updateNoiseMatrix() {
    initializeNoiseMatrix();
}

std::vector<std::complex<double>> QuantumNoise::tensorProduct(
    const std::vector<std::complex<double>>& a,
    const std::vector<std::complex<double>>& b
) {
    std::vector<std::complex<double>> result(a.size() * b.size());
    for (size_t i = 0; i < a.size(); ++i) {
        for (size_t j = 0; j < b.size(); ++j) {
            result[i * b.size() + j] = a[i] * b[j];
        }
    }
    return result;
}

void QuantumNoise::applyKrausOperators(
    std::vector<std::complex<double>>& state,
    const std::vector<std::vector<std::complex<double>>>& operators
) {
    std::vector<std::complex<double>> new_state(state.size(), 0);
    for (const auto& op : operators) {
        std::vector<std::complex<double>> temp_state(state.size(), 0);
        for (size_t i = 0; i < state.size(); ++i) {
            for (size_t j = 0; j < state.size(); ++j) {
                temp_state[i] += op[i * state.size() + j] * state[j];
            }
        }
        for (size_t i = 0; i < state.size(); ++i) {
            new_state[i] += temp_state[i];
        }
    }
    state = new_state;
}

void QuantumNoise::applyNoise(std::vector<std::complex<double>>& state, int qubit) {
    if (dist(rng) < error_rate) {
        applyKrausOperators(state, {noise_matrix});
    }
}

void QuantumNoise::applyNoise(std::vector<std::complex<double>>& state, int qubit1, int qubit2) {
    if (dist(rng) < error_rate) {
        auto noise_matrix_2q = tensorProduct(noise_matrix, noise_matrix);
        applyKrausOperators(state, {noise_matrix_2q});
    }
}

void QuantumNoise::applyNoise(std::vector<std::complex<double>>& state, int qubit1, int qubit2, int qubit3) {
    if (dist(rng) < error_rate) {
        auto noise_matrix_2q = tensorProduct(noise_matrix, noise_matrix);
        auto noise_matrix_3q = tensorProduct(noise_matrix_2q, noise_matrix);
        applyKrausOperators(state, {noise_matrix_3q});
    }
}

std::vector<std::vector<std::complex<double>>> QuantumNoise::getNoiseMatrix() const {
    return noise_matrix;
}

} // namespace qasm 