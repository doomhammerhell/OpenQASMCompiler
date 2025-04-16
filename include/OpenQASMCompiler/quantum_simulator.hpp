#ifndef OPENQASM_QUANTUM_SIMULATOR_HPP
#define OPENQASM_QUANTUM_SIMULATOR_HPP

#include <vector>
#include <complex>
#include <memory>
#include <random>
#include <unordered_map>
#include "quantum_circuit.hpp"
#include "quantum_gates.hpp"

namespace qasm {

/**
 * @brief Noise models for quantum simulation
 */
enum class NoiseModel {
    NONE,           ///< No noise
    DEPOLARIZING,   ///< Depolarizing noise
    AMPLITUDE_DAMPING, ///< Amplitude damping noise
    PHASE_DAMPING,  ///< Phase damping noise
    BIT_FLIP,       ///< Bit flip noise
    PHASE_FLIP,     ///< Phase flip noise
    BIT_PHASE_FLIP  ///< Combined bit and phase flip noise
};

/**
 * @class QuantumSimulator
 * @brief Simulates quantum circuits and performs measurements
 * 
 * This class provides functionality to simulate quantum circuits
 * and perform measurements on the quantum state. It supports:
 * - State vector simulation
 * - Measurement operations
 * - Probability calculations
 * - Expectation value calculations
 * - Noise models
 * - Density matrix simulation
 */
class QuantumSimulator {
public:
    /**
     * @brief Construct a new Quantum Simulator object
     * @param num_qubits Number of qubits in the circuit
     * @param noise_model Noise model to use (default: NONE)
     * @param noise_parameter Noise parameter value (default: 0.0)
     */
    explicit QuantumSimulator(size_t num_qubits, 
                            NoiseModel noise_model = NoiseModel::NONE,
                            double noise_parameter = 0.0);

    /**
     * @brief Simulate a quantum circuit
     * @param circuit The quantum circuit to simulate
     * @param shots Number of measurement shots (default: 1)
     */
    void simulate(const QuantumCircuit& circuit, size_t shots = 1);

    /**
     * @brief Measure a specific qubit
     * @param qubit_index Index of the qubit to measure
     * @return Measurement result (0 or 1)
     */
    bool measure(size_t qubit_index);

    /**
     * @brief Measure all qubits
     * @return Vector of measurement results
     */
    std::vector<bool> measureAll();

    /**
     * @brief Get the probability of measuring a specific state
     * @param state The state to measure
     * @return Probability of measuring the state
     */
    double getProbability(const std::vector<bool>& state) const;

    /**
     * @brief Get the expectation value of an observable
     * @param observable The observable to measure
     * @return Expectation value
     */
    double getExpectationValue(const std::vector<std::vector<std::complex<double>>>& observable) const;

    /**
     * @brief Get measurement statistics
     * @param shots Number of measurement shots
     * @return Map of measurement outcomes to their counts
     */
    std::unordered_map<std::string, size_t> getMeasurementStats(size_t shots);

    /**
     * @brief Get the density matrix
     * @return The density matrix
     */
    std::vector<std::vector<std::complex<double>>> getDensityMatrix() const;

    /**
     * @brief Reset the simulator to the initial state
     */
    void reset();

    /**
     * @brief Get the current state vector
     * @return The state vector
     */
    const std::vector<std::complex<double>>& getStateVector() const;

    /**
     * @brief Set the noise model
     * @param model The noise model to use
     * @param parameter The noise parameter value
     */
    void setNoiseModel(NoiseModel model, double parameter);

private:
    size_t num_qubits_;
    std::vector<std::complex<double>> state_vector_;
    std::mt19937 rng_;
    std::uniform_real_distribution<double> dist_;
    NoiseModel noise_model_;
    double noise_parameter_;

    // Gate application methods
    void applySingleQubitGate(const SingleQubitGate& gate);
    void applyTwoQubitGate(const TwoQubitGate& gate);
    void applyThreeQubitGate(const ThreeQubitGate& gate);
    void applyControlledGate(const ControlledGate& gate);
    void applyParameterizedGate(const ParameterizedGate& gate);

    // Noise application methods
    void applyNoise(size_t qubit);
    void applyDepolarizingNoise(size_t qubit);
    void applyAmplitudeDampingNoise(size_t qubit);
    void applyPhaseDampingNoise(size_t qubit);
    void applyBitFlipNoise(size_t qubit);
    void applyPhaseFlipNoise(size_t qubit);
    void applyBitPhaseFlipNoise(size_t qubit);

    // Helper methods
    void initializeState();
    void normalizeState();
    double getProbabilityAmplitude(const std::vector<bool>& state) const;
    std::vector<std::complex<double>> tensorProduct(
        const std::vector<std::complex<double>>& a,
        const std::vector<std::complex<double>>& b) const;
    std::vector<std::complex<double>> kroneckerProduct(
        const std::vector<std::vector<std::complex<double>>>& a,
        const std::vector<std::vector<std::complex<double>>>& b) const;
    std::string stateToString(const std::vector<bool>& state) const;
};

} // namespace qasm

#endif // OPENQASM_QUANTUM_SIMULATOR_HPP 