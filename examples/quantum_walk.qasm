// Quantum Walk on a line
OPENQASM 2.0;
include "qelib1.inc";

qreg q[4];  // 2 qubits for position, 2 qubits for coin
creg c[4];  // Classical bits for measurement

// Initialize coin state (|0⟩ + i|1⟩)/√2
h q[0];
s q[0];

// Initialize position state (|0⟩)
// No initialization needed as q[2] and q[3] start in |0⟩

// Quantum walk steps
// Step 1
h q[0];  // Coin flip
cx q[0], q[2];  // Controlled shift
cx q[1], q[3];

// Step 2
h q[0];
cx q[0], q[2];
cx q[1], q[3];

// Step 3
h q[0];
cx q[0], q[2];
cx q[1], q[3];

// Measure position and coin
measure q[0] -> c[0];  // Coin
measure q[1] -> c[1];  // Coin
measure q[2] -> c[2];  // Position
measure q[3] -> c[3];  // Position 