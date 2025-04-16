#ifndef QUANTUM_NOISE_HPP
#define QUANTUM_NOISE_HPP

#include <random>
#include <vector>
#include <complex>
#include <memory>

namespace qasm {

class QuantumNoise {
public:
    enum class NoiseType {
        DEPOLARIZING,    // Depolarizing noise
        AMPLITUDE_DAMPING, // Amplitude damping noise
        PHASE_DAMPING,   // Phase damping noise
        BIT_FLIP,        // Bit flip noise
        PHASE_FLIP,      // Phase flip noise
        BIT_PHASE_FLIP   // Combined bit and phase flip noise
    };

    QuantumNoise(double error_rate = 0.01);
    ~QuantumNoise();

    // Noise configuration
    void setErrorRate(double rate);
    void setNoiseType(NoiseType type);
    void setCustomNoise(const std::vector<std::vector<std::complex<double>>>& noise_matrix);

    // Noise application
    void applyNoise(std::vector<std::complex<double>>& state, int qubit);
    void applyNoise(std::vector<std::complex<double>>& state, int qubit1, int qubit2);
    void applyNoise(std::vector<std::complex<double>>& state, int qubit1, int qubit2, int qubit3);

    // Diagnostic methods
    double getErrorRate() const { return error_rate; }
    NoiseType getNoiseType() const { return noise_type; }
    std::vector<std::vector<std::complex<double>>> getNoiseMatrix() const;

private:
    double error_rate;
    NoiseType noise_type;
    std::vector<std::vector<std::complex<double>>> noise_matrix;
    std::mt19937 rng;
    std::uniform_real_distribution<double> dist;

    // Standard noise matrices
    void initializeNoiseMatrix();
    void updateNoiseMatrix();

    // Helper operations
    std::vector<std::complex<double>> tensorProduct(
        const std::vector<std::complex<double>>& a,
        const std::vector<std::complex<double>>& b
    );
    void applyKrausOperators(
        std::vector<std::complex<double>>& state,
        const std::vector<std::vector<std::complex<double>>>& operators
    );
};

} // namespace qasm

#endif // QUANTUM_NOISE_HPP 