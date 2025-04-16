// Quantum Fourier Transform
OPENQASM 2.0;
include "qelib1.inc";

// Define a controlled phase gate
gate cphase(theta) a,b {
    p(theta) b;
    cx a,b;
    p(-theta) b;
    cx a,b;
}

// Define the QFT gate
gate qft(n) q {
    for i in [0:n-1] {
        h q[i];
        for j in [1:n-1-i] {
            cphase(pi/pow(2,j)) q[i],q[i+j];
        }
    }
    // Swap qubits
    for i in [0:floor(n/2)-1] {
        swap q[i],q[n-1-i];
    }
}

// Main circuit
qreg q[3];
creg c[3];

// Apply QFT
qft(3) q;

// Measure
measure q -> c; 