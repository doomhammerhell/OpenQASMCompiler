// Quantum Phase Estimation example
OPENQASM 2.0;
include "qelib1.inc";

qreg q[5];  // 3 qubits for estimation, 2 qubits for eigenstate
creg c[3];  // Classical bits for measurement

// Prepare eigenstate |u⟩ (example: |1⟩)
x q[3];

// Initialize estimation qubits in superposition
h q[0];
h q[1];
h q[2];

// Controlled-U operations
// First qubit (controlled by q[0])
cu1(pi/2) q[0], q[3];
cu1(pi/2) q[0], q[4];

// Second qubit (controlled by q[1])
cu1(pi) q[1], q[3];
cu1(pi) q[1], q[4];

// Third qubit (controlled by q[2])
cu1(2*pi) q[2], q[3];
cu1(2*pi) q[2], q[4];

// Inverse QFT
// Swap qubits
swap q[0], q[2];

// Apply inverse QFT
h q[2];
cp(-pi/2) q[1], q[2];
cp(-pi/4) q[0], q[2];

h q[1];
cp(-pi/2) q[0], q[1];

h q[0];

// Measure estimation qubits
measure q[0] -> c[0];
measure q[1] -> c[1];
measure q[2] -> c[2]; 