// Bell state circuit
OPENQASM 2.0;
include "qelib1.inc";

qreg q[2];
creg c[2];

// Create Bell state
h q[0];
cx q[0], q[1];

// Measure
measure q[0] -> c[0];
measure q[1] -> c[1]; 