# QuantumCircuit Class

The `QuantumCircuit` class provides a high-level interface for creating and visualizing quantum circuits.

## Features

- Support for single-qubit gates (H, X, Y, Z, RX, RY, RZ)
- Support for two-qubit gates (CNOT, SWAP, CZ)
- Support for three-qubit gates (Toffoli, Fredkin)
- Multiple output formats (QASM, LaTeX, ASCII)
- Circuit execution on quantum states
- Measurement operations

## Circuit Creation

### Basic Usage
```cpp
QuantumCircuit circuit(2); // Create a 2-qubit circuit

// Add gates
circuit.addGate(QuantumCircuit::GateType::H, {0});
circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 1});
```

### Single-Qubit Gates
```cpp
// Hadamard gate
circuit.addGate(QuantumCircuit::GateType::H, {0});

// Pauli gates
circuit.addGate(QuantumCircuit::GateType::X, {0});
circuit.addGate(QuantumCircuit::GateType::Y, {0});
circuit.addGate(QuantumCircuit::GateType::Z, {0});

// Rotation gates
circuit.addGate(QuantumCircuit::GateType::RX, {0}, M_PI/2);
circuit.addGate(QuantumCircuit::GateType::RY, {0}, M_PI/4);
circuit.addGate(QuantumCircuit::GateType::RZ, {0}, M_PI/8);
```

### Two-Qubit Gates
```cpp
// CNOT gate
circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 1});

// SWAP gate
circuit.addGate(QuantumCircuit::GateType::SWAP, {0, 1});

// CZ gate
circuit.addGate(QuantumCircuit::GateType::CZ, {0, 1});
```

### Three-Qubit Gates
```cpp
// Toffoli gate
circuit.addGate(QuantumCircuit::GateType::TOFFOLI, {0, 1, 2});

// Fredkin gate
circuit.addGate(QuantumCircuit::GateType::FREDKIN, {0, 1, 2});
```

### Measurement
```cpp
circuit.addGate(QuantumCircuit::GateType::MEASURE, {0});
```

## Circuit Visualization

### QASM Output
```cpp
std::string qasm = circuit.toQASM();
```
Example output:
```
OPENQASM 2.0;
include "qelib1.inc";
qreg q[2];
creg c[2];

h q[0];
cx q[0] q[1];
```

### LaTeX Output
```cpp
std::string latex = circuit.toLatex();
```
Example output:
```
\begin{quantikz}
\lstick{$q_0$} & \gate{H} & \ctrl{0} & \\
\lstick{$q_1$} & & \targ{1} & \\
\end{quantikz}
```

### ASCII Output
```cpp
std::string ascii = circuit.toASCII();
```
Example output:
```
H
CNOT(0,1)
```

## Circuit Execution

### Basic Execution
```cpp
QuantumState state(2);
circuit.execute(state);
```

### Example: Creating a Bell State
```cpp
QuantumCircuit circuit(2);
circuit.addGate(QuantumCircuit::GateType::H, {0});
circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 1});

QuantumState state(2);
circuit.execute(state);
```

## Performance Considerations

- Circuit execution is optimized using SIMD instructions
- Gate operations are performed in sequence
- Measurement operations collapse the quantum state
- Circuit visualization is efficient and supports multiple formats 