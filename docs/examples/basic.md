# Basic Examples

This document provides basic examples of using the OpenQASM Compiler.

## Bell State Creation

### C++ Implementation
```cpp
#include "quantum_circuit.hpp"
#include "quantum_backend.hpp"

int main() {
    // Create a quantum circuit with 2 qubits
    QuantumCircuit circuit(2);
    
    // Add Hadamard gate to first qubit
    circuit.addGate(GateType::H, {0});
    
    // Add CNOT gate between qubits
    circuit.addGate(GateType::CNOT, {0, 1});
    
    // Create local backend
    auto backend = QuantumBackendFactory::createBackend("local");
    BackendConfig config;
    config.max_qubits = 2;
    backend->configure(config);
    
    // Execute circuit
    QuantumState state(2);
    backend->execute(circuit, state);
    
    // Measure 1000 times
    auto results = backend->measure(circuit, 1000);
    
    // Print results
    for (const auto& count : results) {
        std::cout << "State " << count.first << ": "
                 << count.second << " counts" << std::endl;
    }
    
    return 0;
}
```

### OpenQASM Implementation
```qasm
// Bell state preparation
OPENQASM 2.0;
include "qelib1.inc";

qreg q[2];
creg c[2];

// Create superposition
h q[0];
// Entangle qubits
cx q[0],q[1];

// Measure
measure q -> c;
```

## Quantum Fourier Transform

### C++ Implementation
```cpp
int main() {
    const int n_qubits = 3;
    QuantumCircuit circuit(n_qubits);
    
    // Helper function for controlled phase rotation
    auto cphase = [&circuit](double theta, int control, int target) {
        circuit.addGate(GateType::P, {target}, theta);
        circuit.addGate(GateType::CNOT, {control, target});
        circuit.addGate(GateType::P, {target}, -theta);
        circuit.addGate(GateType::CNOT, {control, target});
    };
    
    // Implement QFT
    for (int i = 0; i < n_qubits; i++) {
        // Hadamard on current qubit
        circuit.addGate(GateType::H, {i});
        
        // Controlled phase rotations
        for (int j = i+1; j < n_qubits; j++) {
            double theta = M_PI / std::pow(2, j-i);
            cphase(theta, i, j);
        }
    }
    
    // Swap qubits (reverse order)
    for (int i = 0; i < n_qubits/2; i++) {
        circuit.addGate(GateType::SWAP, {i, n_qubits-1-i});
    }
    
    // Execute
    auto backend = QuantumBackendFactory::createBackend("local");
    QuantumState state(n_qubits);
    backend->execute(circuit, state);
    
    return 0;
}
```

### OpenQASM Implementation
```qasm
// Quantum Fourier Transform
OPENQASM 2.0;
include "qelib1.inc";

// Define a controlled phase gate
gate cphase(theta) a,b {
    p(theta) b;
    cx a,b;
    p(-theta) b;
    cx a,b;
}

// Define the QFT gate
gate qft(n) q {
    for i in [0:n-1] {
        h q[i];
        for j in [1:n-1-i] {
            cphase(pi/pow(2,j)) q[i],q[i+j];
        }
    }
    // Swap qubits
    for i in [0:floor(n/2)-1] {
        swap q[i],q[n-1-i];
    }
}

// Main circuit
qreg q[3];
creg c[3];

// Apply QFT
qft(3) q;

// Measure
measure q -> c;
```

## Grover's Algorithm
```cpp
int main() {
    const int n_qubits = 3;
    QuantumCircuit circuit(n_qubits);
    
    // Initialize all qubits in superposition
    for (int i = 0; i < n_qubits; i++) {
        circuit.addGate(GateType::H, {i});
    }
    
    // Number of iterations (pi/4 * sqrt(N))
    const int iterations = std::floor(M_PI/4.0 * std::sqrt(1 << n_qubits));
    
    for (int iter = 0; iter < iterations; iter++) {
        // Oracle (mark solution state |101⟩)
        circuit.addGate(GateType::X, {1});  // NOT on middle qubit
        circuit.addGate(GateType::CCZ, {0, 1, 2});  // Controlled-Z on marked state
        circuit.addGate(GateType::X, {1});  // Restore middle qubit
        
        // Diffusion operator
        // Hadamard on all qubits
        for (int i = 0; i < n_qubits; i++) {
            circuit.addGate(GateType::H, {i});
        }
        
        // Phase inversion about zero
        for (int i = 0; i < n_qubits; i++) {
            circuit.addGate(GateType::X, {i});
        }
        circuit.addGate(GateType::CCZ, {0, 1, 2});
        for (int i = 0; i < n_qubits; i++) {
            circuit.addGate(GateType::X, {i});
        }
        
        // Hadamard on all qubits
        for (int i = 0; i < n_qubits; i++) {
            circuit.addGate(GateType::H, {i});
        }
    }
    
    // Execute and measure
    auto backend = QuantumBackendFactory::createBackend("local");
    QuantumState state(n_qubits);
    backend->execute(circuit, state);
    auto results = backend->measure(circuit, 1000);
    
    return 0;
}
```

## Phase Estimation
```cpp
int main() {
    const int precision_qubits = 3;
    const int target_qubits = 1;
    QuantumCircuit circuit(precision_qubits + target_qubits);
    
    // Initialize precision qubits in superposition
    for (int i = 0; i < precision_qubits; i++) {
        circuit.addGate(GateType::H, {i});
    }
    
    // Prepare eigenstate on target qubit
    circuit.addGate(GateType::X, {precision_qubits});
    
    // Controlled unitary operations
    for (int i = 0; i < precision_qubits; i++) {
        int power = 1 << i;
        for (int j = 0; j < power; j++) {
            // Controlled-U operation
            circuit.addGate(GateType::CP, {i, precision_qubits}, M_PI/2);
        }
    }
    
    // Inverse QFT on precision qubits
    for (int i = 0; i < precision_qubits/2; i++) {
        circuit.addGate(GateType::SWAP, {i, precision_qubits-1-i});
    }
    for (int i = precision_qubits-1; i >= 0; i--) {
        circuit.addGate(GateType::H, {i});
        for (int j = 0; j < i; j++) {
            double theta = -M_PI/std::pow(2, i-j);
            circuit.addGate(GateType::CP, {j, i}, theta);
        }
    }
    
    // Execute and measure precision qubits
    auto backend = QuantumBackendFactory::createBackend("local");
    QuantumState state(precision_qubits + target_qubits);
    backend->execute(circuit, state);
    auto results = backend->measure(circuit, 1000);
    
    return 0;
}
```

## Error Mitigation Example
```cpp
int main() {
    QuantumCircuit circuit(2);
    
    // Enable error mitigation
    BackendConfig config;
    config.error_mitigation = true;
    config.mitigation_methods = {
        MitigationMethod::MEASUREMENT_ERROR,
        MitigationMethod::ZERO_NOISE_EXTRAPOLATION
    };
    
    // Create noisy backend
    auto backend = QuantumBackendFactory::createBackend("local");
    backend->configure(config);
    
    // Add gates
    circuit.addGate(GateType::H, {0});
    circuit.addGate(GateType::CNOT, {0, 1});
    
    // Execute with error mitigation
    QuantumState state(2);
    backend->execute(circuit, state);
    
    // Get mitigated results
    auto results = backend->measure(circuit, 1000);
    
    return 0;
}
```

## Next Steps

- Check out [Advanced Examples](advanced.md) for more complex circuits
- Learn about [Cloud Integration](cloud-integration.md)
- Read the [API Reference](../api/README.md) for detailed documentation 