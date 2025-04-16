#ifndef QUANTUM_STATE_HPP
#define QUANTUM_STATE_HPP

#include <vector>
#include <complex>
#include <immintrin.h> // For SIMD instructions
#include <memory>
#include <unordered_map>
#include <string>
#include "quantum_noise.hpp"

namespace qasm {

class QuantumState {
public:
    QuantumState(int num_qubits);
    ~QuantumState();

    // Basic operations
    void applyHadamard(int qubit);
    void applyCNOT(int control, int target);
    void applyX(int qubit);
    void applyY(int qubit);
    void applyZ(int qubit);
    void applyPhase(int qubit, double angle);
    void applyRx(int qubit, double angle);
    void applyRy(int qubit, double angle);
    void applyRz(int qubit, double angle);

    // Three+ qubit gates
    void applyToffoli(int control1, int control2, int target);
    void applyFredkin(int control, int target1, int target2);
    void applyCCNOT(int control1, int control2, int target);
    void applyCSWAP(int control, int target1, int target2);

    // Measurement
    bool measure(int qubit);
    
    // State access
    const std::vector<std::complex<double>>& getState() const { return state; }
    int getNumQubits() const { return num_qubits; }

    // State caching
    void saveState(const std::string& name);
    void loadState(const std::string& name);
    void clearCache();

    // Quantum noise
    void enableNoise(double error_rate = 0.01);
    void disableNoise();
    void setNoiseType(QuantumNoise::NoiseType type);
    void setCustomNoise(const std::vector<std::vector<std::complex<double>>>& noise_matrix);
    bool isNoiseEnabled() const { return noise_enabled; }
    double getErrorRate() const { return noise ? noise->getErrorRate() : 0.0; }

private:
    int num_qubits;
    std::vector<std::complex<double>> state;
    std::unordered_map<std::string, std::vector<std::complex<double>>> state_cache;
    std::unique_ptr<QuantumNoise> noise;
    bool noise_enabled;
    
    // Helper functions
    void applySingleQubitGate(int qubit, const std::vector<std::complex<double>>& matrix);
    void applyTwoQubitGate(int qubit1, int qubit2, const std::vector<std::complex<double>>& matrix);
    void applyThreeQubitGate(int qubit1, int qubit2, int qubit3, 
                           const std::vector<std::complex<double>>& matrix);
    void applyControlledGate(int control, int target, const std::vector<std::complex<double>>& matrix);
    
    // SIMD optimizations
    void simdMatrixMultiply(const std::vector<std::complex<double>>& matrix,
                           const std::vector<std::complex<double>>& input,
                           std::vector<std::complex<double>>& output);
    
    // Utility functions
    int getStateIndex(int qubit, bool value) const;
    void normalize();
    void sortQubits(int& q1, int& q2, int& q3);

    // Noise application
    void applyNoise(int qubit);
    void applyNoise(int qubit1, int qubit2);
    void applyNoise(int qubit1, int qubit2, int qubit3);
};

} // namespace qasm

#endif // QUANTUM_STATE_HPP 