// Quantum PCA example
OPENQASM 2.0;
include "qelib1.inc";

qreg q[3];  // 3 qubits for data encoding and QFT
creg c[3];  // Classical bits for measurement

// Data encoding
// Apply rotations to encode data
rx(theta[0]) q[0];
ry(theta[1]) q[0];
rx(theta[2]) q[1];
ry(theta[3]) q[1];
rx(theta[4]) q[2];
ry(theta[5]) q[2];

// Entangling gates for correlation
cx q[0], q[1];
cx q[1], q[2];

// Quantum Fourier Transform
// Apply Hadamard gates
h q[0];
h q[1];
h q[2];

// Controlled phase rotations
cp(pi/2) q[1], q[0];
cp(pi/4) q[2], q[0];
cp(pi/2) q[2], q[1];

// Swap qubits
swap q[0], q[2];

// Measure all qubits
measure q[0] -> c[0];
measure q[1] -> c[1];
measure q[2] -> c[2]; 