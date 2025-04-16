// GHZ state circuit
OPENQASM 2.0;
include "qelib1.inc";

qreg q[3];
creg c[3];

// Create GHZ state
h q[0];
cx q[0], q[1];
cx q[1], q[2];

// Measure
measure q[0] -> c[0];
measure q[1] -> c[1];
measure q[2] -> c[2]; 