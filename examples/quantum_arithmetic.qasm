// Quantum Arithmetic: Addition of two 2-qubit numbers
OPENQASM 2.0;
include "qelib1.inc";

qreg q[6];  // 2 qubits for first number, 2 for second number, 2 for result
creg c[6];  // Classical bits for measurement

// Initialize first number (|01⟩ = 1)
x q[1];

// Initialize second number (|10⟩ = 2)
x q[2];

// Quantum addition circuit
// First step: Compute sum without carry
cx q[0], q[4];
cx q[2], q[4];

// Second step: Compute carry
ccx q[0], q[2], q[5];
cx q[0], q[2];

// Third step: Add carry to sum
cx q[5], q[4];

// Fourth step: Final carry
ccx q[1], q[3], q[5];
cx q[1], q[3];
cx q[5], q[4];

// Measure all qubits
measure q[0] -> c[0];  // First number LSB
measure q[1] -> c[1];  // First number MSB
measure q[2] -> c[2];  // Second number LSB
measure q[3] -> c[3];  // Second number MSB
measure q[4] -> c[4];  // Result LSB
measure q[5] -> c[5];  // Result MSB 