// Quantum K-Means example
OPENQASM 2.0;
include "qelib1.inc";

qreg q[4];  // 2 qubits for each data point
creg c[4];  // Classical bits for measurement

// Initialize first data point
rx(theta[0]) q[0];
ry(theta[1]) q[0];
rx(theta[2]) q[1];
ry(theta[3]) q[1];

// Initialize second data point
rx(theta[4]) q[2];
ry(theta[5]) q[2];
rx(theta[6]) q[3];
ry(theta[7]) q[3];

// Calculate distance between points
// Apply controlled gates for distance calculation
cx q[0], q[2];
cx q[1], q[3];

// Apply phase gates for distance metric
rz(phi[0]) q[2];
rz(phi[1]) q[3];

// Reverse CNOT gates
cx q[0], q[2];
cx q[1], q[3];

// Measure all qubits
measure q[0] -> c[0];
measure q[1] -> c[1];
measure q[2] -> c[2];
measure q[3] -> c[3]; 