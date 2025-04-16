#include "OpenQASMCompiler/quantum_algorithms.hpp"
#include "OpenQASMCompiler/quantum_simulator.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <bitset>

using namespace qasm;

/**
 * @brief Prints the quantum state probabilities
 * 
 * This function prints the probability amplitudes of non-zero states
 * in a human-readable format, showing the basis state and its probability.
 * 
 * @param sim The quantum simulator containing the state to print
 */
void printState(const QuantumSimulator& sim) {
    std::cout << "Quantum State:" << std::endl;
    for (size_t i = 0; i < (1ULL << sim.getNumQubits()); ++i) {
        auto state = sim.getState();
        double prob = std::norm(state[i]);
        if (prob > 1e-6) {
            std::cout << "|" << std::bitset<8>(i).to_string().substr(8 - sim.getNumQubits()) 
                      << "⟩: " << prob << std::endl;
        }
    }
}

int main() {
    // Example 1: Grover's Search
    {
        std::cout << "\n=== Grover's Search Example ===" << std::endl;
        std::cout << "This example demonstrates Grover's algorithm for searching an unsorted database.\n"
                  << "We search for the state |101⟩ (5 in decimal) in a 3-qubit system.\n"
                  << "The algorithm should amplify the probability of finding the target state.\n";
        
        size_t num_qubits = 3;
        auto oracle = [](size_t x) { return x == 5; }; // Search for |101⟩
        
        auto circuit = QuantumAlgorithms::groversSearch(num_qubits, oracle);
        QuantumSimulator sim(num_qubits);
        sim.simulate(circuit);
        
        std::cout << "\nMeasurement results after Grover's search:" << std::endl;
        for (int i = 0; i < 10; ++i) {
            auto result = sim.measure(0);
            std::cout << "Trial " << i << ": " << result << std::endl;
        }
    }
    
    // Example 2: Quantum Fourier Transform
    {
        std::cout << "\n=== Quantum Fourier Transform Example ===" << std::endl;
        std::cout << "This example demonstrates the Quantum Fourier Transform (QFT).\n"
                  << "QFT is a key component in many quantum algorithms, including Shor's algorithm.\n"
                  << "We apply QFT to a 3-qubit system and observe the resulting state.\n";
        
        size_t num_qubits = 3;
        auto circuit = QuantumAlgorithms::quantumFourierTransform(num_qubits);
        
        QuantumSimulator sim(num_qubits);
        sim.simulate(circuit);
        printState(sim);
    }
    
    // Example 3: Shor's Algorithm
    {
        std::cout << "\n=== Shor's Algorithm Example ===" << std::endl;
        std::cout << "This example demonstrates Shor's algorithm for factoring integers.\n"
                  << "We attempt to factor N = 15, which should find factors 3 and 5.\n"
                  << "The algorithm uses quantum period finding to find the factors.\n";
        
        size_t N = 15; // Number to factor
        auto circuit = QuantumAlgorithms::shorsAlgorithm(N);
        
        QuantumSimulator sim(circuit.getNumQubits());
        sim.simulate(circuit);
        
        std::cout << "\nMeasurement results for factoring " << N << ":" << std::endl;
        for (int i = 0; i < 10; ++i) {
            auto result = sim.measure(0);
            std::cout << "Trial " << i << ": " << result << std::endl;
        }
    }
    
    // Example 4: VQE for Molecular Hamiltonian
    {
        std::cout << "\n=== VQE Example ===" << std::endl;
        std::cout << "This example demonstrates the Variational Quantum Eigensolver (VQE).\n"
                  << "We simulate a simplified H2 molecule Hamiltonian to find its ground state energy.\n"
                  << "The ansatz circuit uses parameterized rotations and entangling gates.\n";
        
        // Define molecular Hamiltonian (simplified)
        auto hamiltonian = [](const std::vector<bool>& state) {
            // Example: H2 molecule Hamiltonian
            double energy = 0.0;
            if (state[0] != state[1]) energy += 0.5; // Coulomb term
            if (state[0] && state[1]) energy += 0.3; // Exchange term
            return energy;
        };
        
        // Create ansatz circuit
        QuantumCircuit ansatz(2);
        ansatz.addGate(std::make_unique<RXGate>(0, M_PI/4));
        ansatz.addGate(std::make_unique<CNOTGate>(0, 1));
        ansatz.addGate(std::make_unique<RYGate>(1, M_PI/3));
        
        std::vector<double> params = {0.1, 0.2, 0.3};
        auto circuit = QuantumAlgorithms::vqe(hamiltonian, ansatz, params);
        
        QuantumSimulator sim(2);
        sim.simulate(circuit);
        printState(sim);
    }
    
    // Example 5: QSVM for Classification
    {
        std::cout << "\n=== QSVM Example ===" << std::endl;
        std::cout << "This example demonstrates Quantum Support Vector Machine (QSVM).\n"
                  << "We classify 2D data points using a quantum kernel.\n"
                  << "The kernel uses the cosine of the dot product as a similarity measure.\n";
        
        // Example training data
        std::vector<std::vector<double>> training_data = {
            {1.0, 0.0},  // Class 0
            {0.0, 1.0},  // Class 0
            {1.0, 1.0},  // Class 1
            {0.0, 0.0}   // Class 1
        };
        
        // Define quantum kernel
        auto kernel = [](const std::vector<double>& x, const std::vector<double>& y) {
            double dot_product = 0.0;
            for (size_t i = 0; i < x.size(); ++i) {
                dot_product += x[i] * y[i];
            }
            return std::cos(dot_product);
        };
        
        auto circuit = QuantumAlgorithms::qsvm(training_data, kernel);
        QuantumSimulator sim(circuit.getNumQubits());
        sim.simulate(circuit);
        printState(sim);
    }
    
    // Example 6: Quantum PCA
    {
        std::cout << "\n=== Quantum PCA Example ===" << std::endl;
        std::cout << "This example demonstrates Quantum Principal Component Analysis (QPCA).\n"
                  << "We reduce the dimensionality of a 3D dataset to 2 principal components.\n"
                  << "The algorithm uses quantum phase estimation to find the principal components.\n";
        
        // Example data matrix
        std::vector<std::vector<double>> data = {
            {1.0, 2.0, 3.0},
            {2.0, 3.0, 4.0},
            {3.0, 4.0, 5.0},
            {4.0, 5.0, 6.0}
        };
        
        size_t num_components = 2;
        auto circuit = QuantumAlgorithms::quantumPCA(data, num_components);
        QuantumSimulator sim(circuit.getNumQubits());
        sim.simulate(circuit);
        printState(sim);
    }
    
    // Example 7: Quantum k-means
    {
        std::cout << "\n=== Quantum k-means Example ===" << std::endl;
        std::cout << "This example demonstrates Quantum k-means clustering.\n"
                  << "We cluster 2D data points into 2 groups using quantum distance calculations.\n"
                  << "The algorithm uses quantum superposition to evaluate distances in parallel.\n";
        
        // Example data points
        std::vector<std::vector<double>> data = {
            {1.0, 1.0},
            {1.5, 2.0},
            {3.0, 4.0},
            {5.0, 7.0},
            {3.5, 5.0},
            {4.5, 5.0},
            {3.5, 4.5}
        };
        
        size_t k = 2; // Number of clusters
        auto circuit = QuantumAlgorithms::quantumKMeans(data, k);
        QuantumSimulator sim(circuit.getNumQubits());
        sim.simulate(circuit);
        printState(sim);
    }
    
    // Example 8: Quantum Arithmetic
    {
        std::cout << "\n=== Quantum Arithmetic Example ===" << std::endl;
        std::cout << "This example demonstrates quantum arithmetic operations.\n"
                  << "We perform a simple addition: 1 + 1 using quantum gates.\n"
                  << "The result is stored in the last two qubits.\n";
        
        QuantumCircuit circuit(4); // 2 qubits for each number
        std::vector<size_t> result = {2, 3}; // Result qubits
        
        // Add 1 + 1
        QuantumAlgorithms::quantumArithmetic(circuit, 0, 1, result);
        
        QuantumSimulator sim(4);
        sim.simulate(circuit);
        printState(sim);
    }
    
    return 0;
} 