// Quantum Fourier Transform on 3 qubits
OPENQASM 2.0;
include "qelib1.inc";

qreg q[3];
creg c[3];

// Apply QFT
h q[0];
cp(pi/2) q[1], q[0];
cp(pi/4) q[2], q[0];

h q[1];
cp(pi/2) q[2], q[1];

h q[2];

// Swap qubits
swap q[0], q[2];

// Measure
measure q[0] -> c[0];
measure q[1] -> c[1];
measure q[2] -> c[2]; 