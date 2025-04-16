// Quantum Neural Network example
OPENQASM 2.0;
include "qelib1.inc";

qreg q[4];  // 4 qubits for input and hidden layer
creg c[4];  // Classical bits for measurement

// Input layer
// Apply input data as rotations
rx(theta[0]) q[0];
ry(theta[1]) q[0];
rx(theta[2]) q[1];
ry(theta[3]) q[1];

// First layer of weights
// Entangling gates
cx q[0], q[2];
cx q[1], q[3];

// Single-qubit rotations
rx(theta[4]) q[2];
ry(theta[5]) q[2];
rx(theta[6]) q[3];
ry(theta[7]) q[3];

// Second layer of weights
// Entangling gates
cx q[2], q[0];
cx q[3], q[1];

// Final rotations
rx(theta[8]) q[0];
ry(theta[9]) q[0];
rx(theta[10]) q[1];
ry(theta[11]) q[1];

// Measure output qubits
measure q[0] -> c[0];
measure q[1] -> c[1]; 