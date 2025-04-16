# OpenQASM Compiler Tutorial

This tutorial will guide you through creating and executing quantum circuits using OpenQASM Compiler.

## Basic Concepts

### Quantum States
A quantum state represents the state of a quantum system. In our compiler, quantum states are represented by the `QuantumState` class:

```cpp
QuantumState state(2);  // Create a 2-qubit state
```

### Quantum Circuits
Quantum circuits are sequences of quantum gates that manipulate quantum states:

```cpp
QuantumCircuit circuit(2);  // Create a 2-qubit circuit
```

### Quantum Gates
Gates are basic operations that modify quantum states:
- Single-qubit gates (H, X, Y, Z, etc.)
- Two-qubit gates (CNOT, SWAP, etc.)
- Three-qubit gates (Toffoli, Fredkin, etc.)

## Step-by-Step Examples

### 1. Creating a Bell State

```cpp
#include "quantum_circuit.hpp"
#include "quantum_backend.hpp"

int main() {
    // Create circuit
    QuantumCircuit circuit(2);
    
    // Add gates
    circuit.addGate(QuantumCircuit::GateType::H, {0});
    circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 1});
    
    // Execute
    auto backend = QuantumBackendFactory::createBackend("local");
    QuantumState state(2);
    backend->execute(circuit, state);
    
    // Measure
    auto results = backend->measure(circuit, 1000);
    
    return 0;
}
```

### 2. Quantum Teleportation

```cpp
int main() {
    QuantumCircuit circuit(3);
    
    // Prepare state to teleport
    circuit.addGate(QuantumCircuit::GateType::H, {0});
    circuit.addGate(QuantumCircuit::GateType::T, {0});
    
    // Create Bell pair
    circuit.addGate(QuantumCircuit::GateType::H, {1});
    circuit.addGate(QuantumCircuit::GateType::CNOT, {1, 2});
    
    // Teleportation protocol
    circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 1});
    circuit.addGate(QuantumCircuit::GateType::H, {0});
    
    // Measure and correct
    circuit.addMeasurement({0, 1});
    circuit.addClassicalControl(QuantumCircuit::GateType::X, {2}, {1});
    circuit.addClassicalControl(QuantumCircuit::GateType::Z, {2}, {0});
    
    auto backend = QuantumBackendFactory::createBackend("local");
    QuantumState state(3);
    backend->execute(circuit, state);
    
    return 0;
}
```

### 3. Using the Quantum Debugger

```cpp
int main() {
    QuantumCircuit circuit(2);
    circuit.addGate(QuantumCircuit::GateType::H, {0});
    circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 1});
    
    QuantumDebugger debugger(circuit);
    
    // Add breakpoints
    debugger.addGateBreakpoint(1);
    
    // Step through execution
    while (debugger.step()) {
        auto state = debugger.getState();
        auto probabilities = debugger.getStateProbabilities();
        
        // Print state information
        std::cout << "Current state: " << state << std::endl;
        std::cout << "Probabilities: " << probabilities << std::endl;
    }
    
    return 0;
}
```

### 4. Working with Cloud Backends

```cpp
int main() {
    QuantumCircuit circuit(2);
    circuit.addGate(QuantumCircuit::GateType::H, {0});
    circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 1});
    
    // Configure IBM Quantum backend
    auto backend = QuantumBackendFactory::createBackend("ibm");
    QuantumBackend::BackendConfig config;
    config.api_key = getenv("IBM_QUANTUM_API_KEY");
    config.device = "ibmq_manila";
    backend->configure(config);
    
    // Execute and get results
    QuantumState state(2);
    backend->execute(circuit, state);
    auto results = backend->measure(circuit, 1000);
    
    return 0;
}
```

### 5. Error Mitigation

```cpp
int main() {
    QuantumCircuit circuit(2);
    
    // Enable error mitigation
    circuit.setErrorMitigation(true);
    
    // Add gates
    circuit.addGate(QuantumCircuit::GateType::H, {0});
    circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 1});
    
    // Configure backend with noise model
    auto backend = QuantumBackendFactory::createBackend("local");
    QuantumBackend::BackendConfig config;
    config.noise_model = NoiseModel::defaultModel();
    backend->configure(config);
    
    // Execute with error mitigation
    QuantumState state(2);
    backend->execute(circuit, state);
    
    return 0;
}
```

## Advanced Topics

### Custom Gate Definition

```cpp
// Define custom gate matrix
ComplexMatrix U(2, 2);
U << 1/sqrt(2), 1/sqrt(2),
     1/sqrt(2), -1/sqrt(2);

// Add custom gate to circuit
circuit.addCustomGate("MyGate", U, {0});
```

### Circuit Optimization

```cpp
// Enable optimization
circuit.setOptimizationLevel(2);

// Add gates
circuit.addGate(QuantumCircuit::GateType::H, {0});
circuit.addGate(QuantumCircuit::GateType::X, {0});
circuit.addGate(QuantumCircuit::GateType::H, {0});

// Optimize circuit
circuit.optimize();
```

### State Visualization

```cpp
// Get state vector
auto state_vector = state.getVector();

// Plot state
state.plotState();

// Generate circuit diagram
circuit.toLatex("circuit.tex");
```

## Next Steps

- Explore the [API Reference](quantum_circuit.md) for detailed documentation
- Check out more [Examples](examples.md)
- Learn about [Cloud Backend Integration](cloud_backends.md)
- Study [Performance Optimization](optimization.md) techniques 