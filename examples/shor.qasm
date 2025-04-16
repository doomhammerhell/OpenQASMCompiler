// Simplified Shor's algorithm for factoring 15
OPENQASM 2.0;
include "qelib1.inc";

qreg q[8];  // 4 qubits for period finding, 4 for modular exponentiation
creg c[4];  // Classical bits for measurement

// Initialize superposition
h q[0];
h q[1];
h q[2];
h q[3];

// Modular exponentiation for N=15, a=7
// First controlled-U
cx q[0], q[4];
cx q[0], q[5];
cx q[0], q[6];
cx q[0], q[7];

// Second controlled-U^2
cx q[1], q[4];
cx q[1], q[5];
cx q[1], q[6];
cx q[1], q[7];

// Third controlled-U^4
cx q[2], q[4];
cx q[2], q[5];
cx q[2], q[6];
cx q[2], q[7];

// Inverse QFT
h q[3];
cp(-pi/2) q[2], q[3];
cp(-pi/4) q[1], q[3];
cp(-pi/8) q[0], q[3];

h q[2];
cp(-pi/2) q[1], q[2];
cp(-pi/4) q[0], q[2];

h q[1];
cp(-pi/2) q[0], q[1];

h q[0];

// Measure period finding qubits
measure q[0] -> c[0];
measure q[1] -> c[1];
measure q[2] -> c[2];
measure q[3] -> c[3]; 