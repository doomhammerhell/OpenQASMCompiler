# OpenQASM Compiler Examples

This document provides practical examples of using OpenQASM Compiler for various quantum computing tasks.

## Basic Examples

### 1. Quantum Random Number Generator

```cpp
#include "quantum_circuit.hpp"
#include "quantum_backend.hpp"
#include <iostream>
#include <vector>

int generateRandomBit() {
    QuantumCircuit circuit(1);
    circuit.addGate(QuantumCircuit::GateType::H, {0});
    
    auto backend = QuantumBackendFactory::createBackend("local");
    QuantumState state(1);
    backend->execute(circuit, state);
    
    auto results = backend->measure(circuit, 1);
    return results[0].first;
}

std::vector<int> generateRandomBits(int n) {
    std::vector<int> bits;
    for (int i = 0; i < n; i++) {
        bits.push_back(generateRandomBit());
    }
    return bits;
}
```

### 2. Quantum Fourier Transform

```cpp
QuantumCircuit createQFT(int n_qubits) {
    QuantumCircuit circuit(n_qubits);
    
    // Apply QFT
    for (int i = 0; i < n_qubits; i++) {
        // Hadamard gate
        circuit.addGate(QuantumCircuit::GateType::H, {i});
        
        // Controlled phase rotations
        for (int j = i + 1; j < n_qubits; j++) {
            double angle = M_PI / std::pow(2, j - i);
            circuit.addControlledPhase({i, j}, angle);
        }
    }
    
    // Swap qubits
    for (int i = 0; i < n_qubits / 2; i++) {
        circuit.addGate(QuantumCircuit::GateType::SWAP, {i, n_qubits - 1 - i});
    }
    
    return circuit;
}
```

### 3. Grover's Algorithm

```cpp
QuantumCircuit createGroverCircuit(int n_qubits, std::vector<int> marked_states) {
    QuantumCircuit circuit(n_qubits);
    
    // Initialize superposition
    for (int i = 0; i < n_qubits; i++) {
        circuit.addGate(QuantumCircuit::GateType::H, {i});
    }
    
    // Number of iterations
    int iterations = std::floor(M_PI / 4.0 * std::sqrt(1 << n_qubits));
    
    for (int iter = 0; iter < iterations; iter++) {
        // Oracle
        for (int marked : marked_states) {
            circuit.addMultiControlledZ(marked);
        }
        
        // Diffusion operator
        for (int i = 0; i < n_qubits; i++) {
            circuit.addGate(QuantumCircuit::GateType::H, {i});
        }
        circuit.addMultiControlledZ(0);  // Zero state phase flip
        for (int i = 0; i < n_qubits; i++) {
            circuit.addGate(QuantumCircuit::GateType::H, {i});
        }
    }
    
    return circuit;
}
```

## Intermediate Examples

### 4. Quantum Phase Estimation

```cpp
QuantumCircuit createPhaseEstimation(int precision_qubits, const ComplexMatrix& U) {
    int n_qubits = U.rows();
    QuantumCircuit circuit(precision_qubits + n_qubits);
    
    // Initialize precision qubits
    for (int i = 0; i < precision_qubits; i++) {
        circuit.addGate(QuantumCircuit::GateType::H, {i});
    }
    
    // Controlled-U operations
    for (int i = 0; i < precision_qubits; i++) {
        int power = 1 << i;
        for (int j = 0; j < power; j++) {
            circuit.addControlledUnitary({i}, U, {precision_qubits, precision_qubits + n_qubits - 1});
        }
    }
    
    // Inverse QFT on precision qubits
    auto qft = createQFT(precision_qubits);
    qft.inverse();
    circuit.append(qft);
    
    return circuit;
}
```

### 5. Quantum Error Correction

```cpp
class BitFlipCode {
public:
    static QuantumCircuit encode() {
        QuantumCircuit circuit(3);
        
        // Encode logical |0⟩ into |000⟩ or |1⟩ into |111⟩
        circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 1});
        circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 2});
        
        return circuit;
    }
    
    static QuantumCircuit correct() {
        QuantumCircuit circuit(3);
        
        // Syndrome measurement
        circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 3});
        circuit.addGate(QuantumCircuit::GateType::CNOT, {1, 3});
        circuit.addGate(QuantumCircuit::GateType::CNOT, {1, 4});
        circuit.addGate(QuantumCircuit::GateType::CNOT, {2, 4});
        
        // Error correction based on syndrome
        circuit.addClassicalControl(QuantumCircuit::GateType::X, {0}, {3});
        circuit.addClassicalControl(QuantumCircuit::GateType::X, {1}, {3, 4});
        circuit.addClassicalControl(QuantumCircuit::GateType::X, {2}, {4});
        
        return circuit;
    }
};
```

## Advanced Examples

### 6. Variational Quantum Eigensolver

```cpp
class VQE {
public:
    VQE(const ComplexMatrix& hamiltonian, int n_qubits)
        : hamiltonian_(hamiltonian), n_qubits_(n_qubits) {}
    
    double optimize(std::vector<double>& parameters) {
        double energy = std::numeric_limits<double>::max();
        
        for (int iter = 0; iter < max_iterations_; iter++) {
            // Create ansatz circuit
            auto circuit = createAnsatzCircuit(parameters);
            
            // Measure energy
            double new_energy = measureEnergy(circuit);
            
            // Update parameters using gradient descent
            updateParameters(parameters, circuit);
            
            if (std::abs(energy - new_energy) < tolerance_) {
                break;
            }
            energy = new_energy;
        }
        
        return energy;
    }
    
private:
    QuantumCircuit createAnsatzCircuit(const std::vector<double>& parameters) {
        QuantumCircuit circuit(n_qubits_);
        
        // Hardware-efficient ansatz
        for (int layer = 0; layer < n_layers_; layer++) {
            for (int i = 0; i < n_qubits_; i++) {
                circuit.addGate(QuantumCircuit::GateType::RY, {i}, parameters[layer * n_qubits_ + i]);
            }
            for (int i = 0; i < n_qubits_ - 1; i++) {
                circuit.addGate(QuantumCircuit::GateType::CNOT, {i, i + 1});
            }
        }
        
        return circuit;
    }
    
    double measureEnergy(const QuantumCircuit& circuit) {
        auto backend = QuantumBackendFactory::createBackend("local");
        QuantumState state(n_qubits_);
        backend->execute(circuit, state);
        
        return state.expectationValue(hamiltonian_);
    }
    
    void updateParameters(std::vector<double>& parameters, const QuantumCircuit& circuit) {
        // Implement parameter update using gradient descent
    }
    
    ComplexMatrix hamiltonian_;
    int n_qubits_;
    int n_layers_ = 2;
    int max_iterations_ = 100;
    double tolerance_ = 1e-6;
};
```

### 7. Quantum Machine Learning

```cpp
class QuantumNeuralNetwork {
public:
    QuantumNeuralNetwork(int n_qubits, int n_layers)
        : n_qubits_(n_qubits), n_layers_(n_layers) {}
    
    QuantumCircuit createCircuit(const std::vector<double>& input_data,
                               const std::vector<double>& parameters) {
        QuantumCircuit circuit(n_qubits_);
        
        // Encode classical data
        for (int i = 0; i < n_qubits_; i++) {
            circuit.addGate(QuantumCircuit::GateType::RY, {i}, input_data[i]);
        }
        
        // Variational layers
        int param_idx = 0;
        for (int layer = 0; layer < n_layers_; layer++) {
            // Single-qubit rotations
            for (int i = 0; i < n_qubits_; i++) {
                circuit.addGate(QuantumCircuit::GateType::RZ, {i}, parameters[param_idx++]);
                circuit.addGate(QuantumCircuit::GateType::RY, {i}, parameters[param_idx++]);
                circuit.addGate(QuantumCircuit::GateType::RZ, {i}, parameters[param_idx++]);
            }
            
            // Entangling layers
            for (int i = 0; i < n_qubits_ - 1; i++) {
                circuit.addGate(QuantumCircuit::GateType::CNOT, {i, i + 1});
            }
        }
        
        return circuit;
    }
    
    std::vector<double> predict(const std::vector<double>& input_data,
                              const std::vector<double>& parameters) {
        auto circuit = createCircuit(input_data, parameters);
        
        auto backend = QuantumBackendFactory::createBackend("local");
        QuantumState state(n_qubits_);
        backend->execute(circuit, state);
        
        // Measure output
        std::vector<double> predictions;
        auto results = backend->measure(circuit, 1000);
        // Process results to get predictions
        
        return predictions;
    }
    
private:
    int n_qubits_;
    int n_layers_;
};
```

## Running the Examples

To compile and run these examples:

1. Create a new file with the desired example code
2. Include necessary headers
3. Compile with OpenQASM Compiler:
   ```bash
   g++ -std=c++17 example.cpp -I/path/to/OpenQASMCompiler/include -L/path/to/OpenQASMCompiler/lib -lqasmcompiler -o example
   ```
4. Run the executable:
   ```bash
   ./example
   ```

## Additional Resources

- [API Reference](quantum_circuit.md)
- [Tutorial](tutorial.md)
- [Performance Optimization](optimization.md)
- [Cloud Backend Integration](cloud_backends.md) 