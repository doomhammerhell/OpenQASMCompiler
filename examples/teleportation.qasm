// Quantum teleportation circuit
OPENQASM 2.0;
include "qelib1.inc";

qreg q[3];  // q[0]: state to teleport, q[1]: Alice's qubit, q[2]: Bob's qubit
creg c[2];  // Classical bits for measurement results

// Prepare state to teleport (|1⟩)
x q[0];

// Create Bell pair between Alice and Bob
h q[1];
cx q[1], q[2];

// Alice's operations
cx q[0], q[1];
h q[0];

// Measure Alice's qubits
measure q[0] -> c[0];
measure q[1] -> c[1];

// Bob's operations (classically controlled)
if (c[0] == 1) x q[2];
if (c[1] == 1) z q[2]; 