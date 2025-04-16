#ifndef OPENQASM_QUANTUM_ALGORITHMS_HPP
#define OPENQASM_QUANTUM_ALGORITHMS_HPP

#include "quantum_circuit.hpp"
#include "quantum_simulator.hpp"
#include <vector>
#include <functional>

namespace qasm {

/**
 * @brief Class for implementing quantum algorithms
 */
class QuantumAlgorithms {
public:
    /**
     * @brief Implements Grover's search algorithm
     * @param num_qubits Number of qubits for the search space
     * @param oracle Function that marks the solution state
     * @return QuantumCircuit implementing Grover's algorithm
     */
    static QuantumCircuit groversSearch(size_t num_qubits, 
                                      const std::function<bool(size_t)>& oracle);

    /**
     * @brief Implements Quantum Fourier Transform
     * @param num_qubits Number of qubits to transform
     * @return QuantumCircuit implementing QFT
     */
    static QuantumCircuit quantumFourierTransform(size_t num_qubits);

    /**
     * @brief Implements Shor's factoring algorithm
     * @param N Number to factor
     * @return QuantumCircuit implementing Shor's algorithm
     */
    static QuantumCircuit shorsAlgorithm(size_t N);

    /**
     * @brief Implements Quantum Phase Estimation
     * @param unitary Unitary operator to estimate phase of
     * @param precision Number of precision qubits
     * @return QuantumCircuit implementing QPE
     */
    static QuantumCircuit quantumPhaseEstimation(
        const std::function<void(QuantumCircuit&, size_t)>& unitary,
        size_t precision);

    /**
     * @brief Implements Quantum Amplitude Amplification
     * @param circuit Initial quantum circuit
     * @param oracle Function that marks good states
     * @param iterations Number of amplification iterations
     * @return QuantumCircuit implementing amplitude amplification
     */
    static QuantumCircuit amplitudeAmplification(
        const QuantumCircuit& circuit,
        const std::function<bool(size_t)>& oracle,
        size_t iterations);

    /**
     * @brief Implements Quantum Approximate Optimization Algorithm (QAOA)
     * @param cost_hamiltonian Cost Hamiltonian to optimize
     * @param mixer_hamiltonian Mixer Hamiltonian
     * @param p Number of QAOA layers
     * @param params Parameters for each layer
     * @return QuantumCircuit implementing QAOA
     */
    static QuantumCircuit qaoa(
        const std::function<double(const std::vector<bool>&)>& cost_hamiltonian,
        const std::function<void(QuantumCircuit&, size_t)>& mixer_hamiltonian,
        size_t p,
        const std::vector<double>& params);

    /**
     * @brief Implements Variational Quantum Eigensolver (VQE)
     * @param hamiltonian Hamiltonian to find ground state of
     * @param ansatz Ansatz circuit
     * @param params Initial parameters
     * @return QuantumCircuit implementing VQE
     */
    static QuantumCircuit vqe(
        const std::function<double(const std::vector<bool>&)>& hamiltonian,
        const QuantumCircuit& ansatz,
        const std::vector<double>& params);

    /**
     * @brief Implements Quantum Support Vector Machine (QSVM)
     * @param training_data Training data points
     * @param kernel Quantum kernel function
     * @return QuantumCircuit implementing QSVM
     */
    static QuantumCircuit qsvm(
        const std::vector<std::vector<double>>& training_data,
        const std::function<double(const std::vector<double>&, 
                                 const std::vector<double>&)>& kernel);

    /**
     * @brief Implements Quantum Neural Network (QNN)
     * @param num_qubits Number of qubits
     * @param num_layers Number of layers
     * @param params Network parameters
     * @return QuantumCircuit implementing QNN
     */
    static QuantumCircuit quantumNeuralNetwork(
        size_t num_qubits,
        size_t num_layers,
        const std::vector<double>& params);

    /**
     * @brief Implements Quantum Principal Component Analysis (QPCA)
     * @param data Data matrix
     * @param num_components Number of principal components
     * @return QuantumCircuit implementing QPCA
     */
    static QuantumCircuit quantumPCA(
        const std::vector<std::vector<double>>& data,
        size_t num_components);

    /**
     * @brief Implements Quantum k-means clustering
     * @param data Data points
     * @param k Number of clusters
     * @return QuantumCircuit implementing quantum k-means
     */
    static QuantumCircuit quantumKMeans(
        const std::vector<std::vector<double>>& data,
        size_t k);

private:
    /**
     * @brief Implements controlled phase rotation
     * @param circuit Circuit to add gate to
     * @param control Control qubit
     * @param target Target qubit
     * @param angle Rotation angle
     */
    static void controlledPhaseRotation(
        QuantumCircuit& circuit,
        size_t control,
        size_t target,
        double angle);

    /**
     * @brief Implements quantum arithmetic operations
     * @param circuit Circuit to add operations to
     * @param a First number
     * @param b Second number
     * @param result Result qubits
     */
    static void quantumArithmetic(
        QuantumCircuit& circuit,
        size_t a,
        size_t b,
        const std::vector<size_t>& result);
};

} // namespace qasm

#endif // OPENQASM_QUANTUM_ALGORITHMS_HPP 