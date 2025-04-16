# Tutorials

## Table of Contents

1. [Basic Quantum Circuits](#basic-quantum-circuits)
2. [Quantum Algorithms](#quantum-algorithms)
3. [Error Correction](#error-correction)
4. [Quantum Machine Learning](#quantum-machine-learning)
5. [Advanced Features](#advanced-features)

## Basic Quantum Circuits

### 1. Creating a Bell State
```qasm
OPENQASM 2.0;
include "qelib1.inc";

qreg q[2];
creg c[2];

h q[0];
cx q[0], q[1];

measure q[0] -> c[0];
measure q[1] -> c[1];
```

**Steps:**
1. Initialize two qubits
2. Apply Hadamard to first qubit
3. Apply CNOT between qubits
4. Measure both qubits

### 2. Quantum Teleportation
```qasm
OPENQASM 2.0;
include "qelib1.inc";

qreg q[3];
creg c[3];

// Prepare state to teleport
h q[0];

// Create Bell pair
h q[1];
cx q[1], q[2];

// Teleportation protocol
cx q[0], q[1];
h q[0];

measure q[0] -> c[0];
measure q[1] -> c[1];

// Conditional operations
if (c[0] == 1) x q[2];
if (c[1] == 1) z q[2];

measure q[2] -> c[2];
```

## Quantum Algorithms

### 1. Grover's Search
```qasm
OPENQASM 2.0;
include "qelib1.inc";

qreg q[2];
creg c[2];

// Initialization
h q[0];
h q[1];

// Oracle for |11⟩
x q[0];
x q[1];
h q[1];
cx q[0], q[1];
h q[1];
x q[0];
x q[1];

// Diffusion operator
h q[0];
h q[1];
x q[0];
x q[1];
h q[1];
cx q[0], q[1];
h q[1];
x q[0];
x q[1];
h q[0];
h q[1];

measure q[0] -> c[0];
measure q[1] -> c[1];
```

### 2. Quantum Fourier Transform
```qasm
OPENQASM 2.0;
include "qelib1.inc";

qreg q[3];
creg c[3];

// Apply Hadamard gates
h q[0];
h q[1];
h q[2];

// Apply controlled rotations
crz(pi/2) q[1], q[0];
crz(pi/4) q[2], q[0];
crz(pi/2) q[2], q[1];

// Swap qubits
swap q[0], q[2];

measure q[0] -> c[0];
measure q[1] -> c[1];
measure q[2] -> c[2];
```

## Error Correction

### 1. Bit Flip Code
```qasm
OPENQASM 2.0;
include "qelib1.inc";

qreg q[3];
creg c[3];

// Encoding
cx q[0], q[1];
cx q[0], q[2];

// Simulate error
x q[0];

// Syndrome measurement
cx q[0], q[1];
cx q[0], q[2];
ccx q[1], q[2], q[0];

measure q[0] -> c[0];
measure q[1] -> c[1];
measure q[2] -> c[2];
```

### 2. Phase Flip Code
```qasm
OPENQASM 2.0;
include "qelib1.inc";

qreg q[3];
creg c[3];

// Encoding
h q[0];
cx q[0], q[1];
cx q[0], q[2];
h q[0];
h q[1];
h q[2];

// Simulate error
z q[0];

// Syndrome measurement
h q[0];
h q[1];
h q[2];
cx q[0], q[1];
cx q[0], q[2];
ccx q[1], q[2], q[0];

measure q[0] -> c[0];
measure q[1] -> c[1];
measure q[2] -> c[2];
```

## Quantum Machine Learning

### 1. Quantum Neural Network
```qasm
OPENQASM 2.0;
include "qelib1.inc";

qreg q[4];
creg c[4];

// Input layer
h q[0];
h q[1];

// Hidden layer
crz(pi/4) q[0], q[2];
crz(pi/4) q[1], q[2];
crz(pi/4) q[0], q[3];
crz(pi/4) q[1], q[3];

// Output layer
h q[2];
h q[3];

measure q[0] -> c[0];
measure q[1] -> c[1];
measure q[2] -> c[2];
measure q[3] -> c[3];
```

### 2. Quantum Support Vector Machine
```qasm
OPENQASM 2.0;
include "qelib1.inc";

qreg q[3];
creg c[3];

// Feature map
h q[0];
h q[1];
crz(pi/2) q[0], q[1];

// Kernel computation
h q[2];
crz(pi/4) q[0], q[2];
crz(pi/4) q[1], q[2];
h q[2];

measure q[0] -> c[0];
measure q[1] -> c[1];
measure q[2] -> c[2];
```

## Advanced Features

### 1. Custom Gates
```qasm
OPENQASM 2.0;
include "qelib1.inc";

gate custom_gate(theta) a, b {
    u3(theta, 0, 0) a;
    cx a, b;
    u3(-theta, 0, 0) b;
}

qreg q[2];
creg c[2];

custom_gate(pi/2) q[0], q[1];

measure q[0] -> c[0];
measure q[1] -> c[1];
```

### 2. Parameterized Circuits
```qasm
OPENQASM 2.0;
include "qelib1.inc";

qreg q[2];
creg c[2];

// Parameterized rotation
u3(pi/4, 0, 0) q[0];
u3(pi/2, 0, 0) q[1];

// Controlled operation
cx q[0], q[1];

measure q[0] -> c[0];
measure q[1] -> c[1];
```

### 3. Noise Simulation
```qasm
OPENQASM 2.0;
include "qelib1.inc";

qreg q[2];
creg c[2];

// Apply gates with noise
h q[0];
// Simulate depolarizing noise
u3(0.1, 0, 0) q[0];
cx q[0], q[1];
// Simulate amplitude damping
u3(0.05, 0, 0) q[1];

measure q[0] -> c[0];
measure q[1] -> c[1];
``` 