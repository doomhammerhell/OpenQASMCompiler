// VQE example for finding ground state of H2 molecule
OPENQASM 2.0;
include "qelib1.inc";

qreg q[2];  // Two qubits for H2 molecule
creg c[2];  // Classical bits for measurement

// Parameterized ansatz circuit
// First layer
rx(theta[0]) q[0];
ry(theta[1]) q[0];
rx(theta[2]) q[1];
ry(theta[3]) q[1];

// Entangling layer
cx q[0], q[1];

// Second layer
rx(theta[4]) q[0];
ry(theta[5]) q[0];
rx(theta[6]) q[1];
ry(theta[7]) q[1];

// Measure in Pauli bases for energy estimation
// Z basis measurement
measure q[0] -> c[0];
measure q[1] -> c[1];

// X basis measurement (requires basis rotation)
h q[0];
h q[1];
measure q[0] -> c[0];
measure q[1] -> c[1];

// Y basis measurement (requires basis rotation)
sdg q[0];
sdg q[1];
h q[0];
h q[1];
measure q[0] -> c[0];
measure q[1] -> c[1]; 