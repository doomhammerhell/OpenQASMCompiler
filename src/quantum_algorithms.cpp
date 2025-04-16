#include "OpenQASMCompiler/quantum_algorithms.hpp"
#include <cmath>
#include <algorithm>

namespace qasm {

QuantumCircuit QuantumAlgorithms::groversSearch(size_t num_qubits, 
                                              const std::function<bool(size_t)>& oracle) {
    QuantumCircuit circuit(num_qubits);
    
    // Initialize superposition
    for (size_t i = 0; i < num_qubits; ++i) {
        circuit.addGate(std::make_unique<HGate>(i));
    }
    
    // Number of iterations
    size_t iterations = static_cast<size_t>(M_PI/4 * std::sqrt(1ULL << num_qubits));
    
    for (size_t i = 0; i < iterations; ++i) {
        // Apply oracle
        for (size_t j = 0; j < (1ULL << num_qubits); ++j) {
            if (oracle(j)) {
                circuit.addGate(std::make_unique<ZGate>(0));
            }
        }
        
        // Apply diffusion operator
        for (size_t j = 0; j < num_qubits; ++j) {
            circuit.addGate(std::make_unique<HGate>(j));
            circuit.addGate(std::make_unique<XGate>(j));
        }
        
        circuit.addGate(std::make_unique<ZGate>(0));
        
        for (size_t j = 0; j < num_qubits; ++j) {
            circuit.addGate(std::make_unique<XGate>(j));
            circuit.addGate(std::make_unique<HGate>(j));
        }
    }
    
    return circuit;
}

QuantumCircuit QuantumAlgorithms::quantumFourierTransform(size_t num_qubits) {
    QuantumCircuit circuit(num_qubits);
    
    for (size_t i = 0; i < num_qubits; ++i) {
        circuit.addGate(std::make_unique<HGate>(i));
        
        for (size_t j = i + 1; j < num_qubits; ++j) {
            double angle = M_PI / (1ULL << (j - i));
            circuit.addGate(std::make_unique<CPGate>(i, j, angle));
        }
    }
    
    // Swap qubits
    for (size_t i = 0; i < num_qubits/2; ++i) {
        circuit.addGate(std::make_unique<SWAPGate>(i, num_qubits - 1 - i));
    }
    
    return circuit;
}

QuantumCircuit QuantumAlgorithms::shorsAlgorithm(size_t N) {
    // Find number of qubits needed
    size_t n = static_cast<size_t>(std::ceil(std::log2(N)));
    size_t num_qubits = 2 * n;
    
    QuantumCircuit circuit(num_qubits);
    
    // Initialize superposition
    for (size_t i = 0; i < n; ++i) {
        circuit.addGate(std::make_unique<HGate>(i));
    }
    
    // Modular exponentiation
    for (size_t i = 0; i < n; ++i) {
        size_t power = 1ULL << i;
        // Implement controlled-U^power
        for (size_t j = 0; j < power; ++j) {
            // Add modular multiplication gates
            // This is a simplified version
            circuit.addGate(std::make_unique<CCXGate>(i, n + j, n + (j + 1) % n));
        }
    }
    
    // Inverse QFT
    auto qft = quantumFourierTransform(n);
    for (const auto& gate : qft.getGates()) {
        circuit.addGate(gate->clone());
    }
    
    return circuit;
}

QuantumCircuit QuantumAlgorithms::quantumPhaseEstimation(
    const std::function<void(QuantumCircuit&, size_t)>& unitary,
    size_t precision) {
    QuantumCircuit circuit(precision + 1); // precision qubits + 1 eigenstate qubit
    
    // Initialize eigenstate qubit
    circuit.addGate(std::make_unique<HGate>(precision));
    
    // Apply controlled unitaries
    for (size_t i = 0; i < precision; ++i) {
        circuit.addGate(std::make_unique<HGate>(i));
        
        size_t power = 1ULL << i;
        for (size_t j = 0; j < power; ++j) {
            unitary(circuit, i);
        }
    }
    
    // Inverse QFT
    auto qft = quantumFourierTransform(precision);
    for (const auto& gate : qft.getGates()) {
        circuit.addGate(gate->clone());
    }
    
    return circuit;
}

QuantumCircuit QuantumAlgorithms::amplitudeAmplification(
    const QuantumCircuit& circuit,
    const std::function<bool(size_t)>& oracle,
    size_t iterations) {
    QuantumCircuit result = circuit;
    
    for (size_t i = 0; i < iterations; ++i) {
        // Apply oracle
        for (size_t j = 0; j < (1ULL << circuit.getNumQubits()); ++j) {
            if (oracle(j)) {
                result.addGate(std::make_unique<ZGate>(0));
            }
        }
        
        // Apply diffusion operator
        for (size_t j = 0; j < circuit.getNumQubits(); ++j) {
            result.addGate(std::make_unique<HGate>(j));
            result.addGate(std::make_unique<XGate>(j));
        }
        
        result.addGate(std::make_unique<ZGate>(0));
        
        for (size_t j = 0; j < circuit.getNumQubits(); ++j) {
            result.addGate(std::make_unique<XGate>(j));
            result.addGate(std::make_unique<HGate>(j));
        }
    }
    
    return result;
}

QuantumCircuit QuantumAlgorithms::qaoa(
    const std::function<double(const std::vector<bool>&)>& cost_hamiltonian,
    const std::function<void(QuantumCircuit&, size_t)>& mixer_hamiltonian,
    size_t p,
    const std::vector<double>& params) {
    size_t num_qubits = static_cast<size_t>(std::log2(cost_hamiltonian({})));
    QuantumCircuit circuit(num_qubits);
    
    // Initial state
    for (size_t i = 0; i < num_qubits; ++i) {
        circuit.addGate(std::make_unique<HGate>(i));
    }
    
    // QAOA layers
    for (size_t i = 0; i < p; ++i) {
        // Cost Hamiltonian
        for (size_t j = 0; j < (1ULL << num_qubits); ++j) {
            std::vector<bool> state(num_qubits);
            for (size_t k = 0; k < num_qubits; ++k) {
                state[k] = (j & (1ULL << k)) != 0;
            }
            double cost = cost_hamiltonian(state);
            circuit.addGate(std::make_unique<RZGate>(0, params[2*i] * cost));
        }
        
        // Mixer Hamiltonian
        mixer_hamiltonian(circuit, params[2*i + 1]);
    }
    
    return circuit;
}

QuantumCircuit QuantumAlgorithms::vqe(
    const std::function<double(const std::vector<bool>&)>& hamiltonian,
    const QuantumCircuit& ansatz,
    const std::vector<double>& params) {
    QuantumCircuit circuit = ansatz;
    
    // Apply parameterized gates
    size_t param_idx = 0;
    for (const auto& gate : circuit.getGates()) {
        if (gate->getType() == GateType::RX || 
            gate->getType() == GateType::RY || 
            gate->getType() == GateType::RZ) {
            circuit.addGate(std::make_unique<RXGate>(
                gate->getQubits()[0], params[param_idx++]));
        }
    }
    
    return circuit;
}

QuantumCircuit QuantumAlgorithms::qsvm(
    const std::vector<std::vector<double>>& training_data,
    const std::function<double(const std::vector<double>&, 
                             const std::vector<double>&)>& kernel) {
    size_t num_qubits = static_cast<size_t>(std::ceil(std::log2(training_data.size())));
    QuantumCircuit circuit(num_qubits);
    
    // Initialize superposition
    for (size_t i = 0; i < num_qubits; ++i) {
        circuit.addGate(std::make_unique<HGate>(i));
    }
    
    // Apply kernel
    for (size_t i = 0; i < training_data.size(); ++i) {
        for (size_t j = 0; j < training_data.size(); ++j) {
            double k = kernel(training_data[i], training_data[j]);
            circuit.addGate(std::make_unique<RZGate>(0, k));
        }
    }
    
    return circuit;
}

QuantumCircuit QuantumAlgorithms::quantumNeuralNetwork(
    size_t num_qubits,
    size_t num_layers,
    const std::vector<double>& params) {
    QuantumCircuit circuit(num_qubits);
    
    size_t param_idx = 0;
    for (size_t layer = 0; layer < num_layers; ++layer) {
        // Single-qubit rotations
        for (size_t i = 0; i < num_qubits; ++i) {
            circuit.addGate(std::make_unique<RXGate>(i, params[param_idx++]));
            circuit.addGate(std::make_unique<RYGate>(i, params[param_idx++]));
            circuit.addGate(std::make_unique<RZGate>(i, params[param_idx++]));
        }
        
        // Entangling gates
        for (size_t i = 0; i < num_qubits - 1; ++i) {
            circuit.addGate(std::make_unique<CNOTGate>(i, i + 1));
        }
    }
    
    return circuit;
}

QuantumCircuit QuantumAlgorithms::quantumPCA(
    const std::vector<std::vector<double>>& data,
    size_t num_components) {
    size_t num_qubits = static_cast<size_t>(std::ceil(std::log2(data.size())));
    QuantumCircuit circuit(num_qubits);
    
    // Initialize superposition
    for (size_t i = 0; i < num_qubits; ++i) {
        circuit.addGate(std::make_unique<HGate>(i));
    }
    
    // Apply data encoding
    for (size_t i = 0; i < data.size(); ++i) {
        for (size_t j = 0; j < data[i].size(); ++j) {
            circuit.addGate(std::make_unique<RXGate>(j, data[i][j]));
        }
    }
    
    // Apply QFT
    auto qft = quantumFourierTransform(num_qubits);
    for (const auto& gate : qft.getGates()) {
        circuit.addGate(gate->clone());
    }
    
    return circuit;
}

QuantumCircuit QuantumAlgorithms::quantumKMeans(
    const std::vector<std::vector<double>>& data,
    size_t k) {
    size_t num_qubits = static_cast<size_t>(std::ceil(std::log2(data.size())));
    QuantumCircuit circuit(num_qubits);
    
    // Initialize superposition
    for (size_t i = 0; i < num_qubits; ++i) {
        circuit.addGate(std::make_unique<HGate>(i));
    }
    
    // Apply distance calculation
    for (size_t i = 0; i < data.size(); ++i) {
        for (size_t j = 0; j < data.size(); ++j) {
            double distance = 0.0;
            for (size_t d = 0; d < data[i].size(); ++d) {
                distance += std::pow(data[i][d] - data[j][d], 2);
            }
            distance = std::sqrt(distance);
            circuit.addGate(std::make_unique<RZGate>(0, distance));
        }
    }
    
    return circuit;
}

void QuantumAlgorithms::controlledPhaseRotation(
    QuantumCircuit& circuit,
    size_t control,
    size_t target,
    double angle) {
    circuit.addGate(std::make_unique<CPGate>(control, target, angle));
}

void QuantumAlgorithms::quantumArithmetic(
    QuantumCircuit& circuit,
    size_t a,
    size_t b,
    const std::vector<size_t>& result) {
    // Implement quantum addition
    for (size_t i = 0; i < result.size(); ++i) {
        circuit.addGate(std::make_unique<CCXGate>(a, b, result[i]));
    }
}

} // namespace qasm 