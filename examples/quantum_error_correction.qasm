// Quantum Error Correction: 3-qubit bit-flip code
OPENQASM 2.0;
include "qelib1.inc";

qreg q[6];  // 3 qubits for encoding, 3 ancilla qubits for syndrome measurement
creg c[3];  // Classical bits for syndrome measurement
creg d[1];  // Classical bit for final measurement

// Encode logical |1⟩ state
x q[0];  // Prepare |1⟩
h q[0];  // Create superposition
cx q[0], q[1];  // First CNOT
cx q[0], q[2];  // Second CNOT

// Simulate bit-flip error on first qubit
x q[0];  // Error

// Syndrome measurement
cx q[0], q[3];
cx q[1], q[3];
cx q[1], q[4];
cx q[2], q[4];
cx q[0], q[5];
cx q[2], q[5];

// Measure syndrome
measure q[3] -> c[0];
measure q[4] -> c[1];
measure q[5] -> c[2];

// Error correction based on syndrome
// If syndrome indicates error on q[0]
if (c[0] == 1 && c[1] == 0 && c[2] == 1) x q[0];
// If syndrome indicates error on q[1]
if (c[0] == 1 && c[1] == 1 && c[2] == 0) x q[1];
// If syndrome indicates error on q[2]
if (c[0] == 0 && c[1] == 1 && c[2] == 1) x q[2];

// Decode and measure
cx q[0], q[2];
cx q[0], q[1];
h q[0];
measure q[0] -> d[0]; 