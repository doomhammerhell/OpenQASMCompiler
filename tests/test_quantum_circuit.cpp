#include <gtest/gtest.h>
#include "../src/interpreter/quantum_circuit.hpp"
#include <complex>
#include <cmath>

using namespace qasm;

TEST(QuantumCircuitTest, Initialization) {
    QuantumCircuit circuit(3);
    EXPECT_EQ(circuit.getNumQubits(), 3);
    EXPECT_TRUE(circuit.getGates().empty());
}

TEST(QuantumCircuitTest, AddGates) {
    QuantumCircuit circuit(3);
    
    // Add single-qubit gates
    circuit.addGate(QuantumCircuit::GateType::H, {0});
    circuit.addGate(QuantumCircuit::GateType::X, {1});
    circuit.addGate(QuantumCircuit::GateType::Y, {2});
    
    // Add two-qubit gates
    circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 1});
    circuit.addGate(QuantumCircuit::GateType::SWAP, {1, 2});
    
    // Add three-qubit gates
    circuit.addGate(QuantumCircuit::GateType::TOFFOLI, {0, 1, 2});
    circuit.addGate(QuantumCircuit::GateType::FREDKIN, {0, 1, 2});
    
    // Add rotation gates
    circuit.addGate(QuantumCircuit::GateType::RX, {0}, M_PI/2);
    circuit.addGate(QuantumCircuit::GateType::RY, {1}, M_PI/4);
    circuit.addGate(QuantumCircuit::GateType::RZ, {2}, M_PI/8);
    
    // Add measurement
    circuit.addGate(QuantumCircuit::GateType::MEASURE, {0});
    
    EXPECT_EQ(circuit.getGates().size(), 11);
}

TEST(QuantumCircuitTest, QASMOutput) {
    QuantumCircuit circuit(2);
    circuit.addGate(QuantumCircuit::GateType::H, {0});
    circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 1});
    
    std::string expected = 
        "OPENQASM 2.0;\n"
        "include \"qelib1.inc\";\n"
        "qreg q[2];\n"
        "creg c[2];\n\n"
        "h q[0];\n"
        "cx q[0] q[1];\n";
    
    EXPECT_EQ(circuit.toQASM(), expected);
}

TEST(QuantumCircuitTest, LatexOutput) {
    QuantumCircuit circuit(2);
    circuit.addGate(QuantumCircuit::GateType::H, {0});
    circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 1});
    
    std::string expected = 
        "\\begin{quantikz}\n"
        "\\lstick{$q_0$} & \\gate{H} & \\ctrl{0} & \\\\\n"
        "\\lstick{$q_1$} & & \\targ{1} & \\\\\n"
        "\\end{quantikz}";
    
    EXPECT_EQ(circuit.toLatex(), expected);
}

TEST(QuantumCircuitTest, ASCIIOutput) {
    QuantumCircuit circuit(2);
    circuit.addGate(QuantumCircuit::GateType::H, {0});
    circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 1});
    
    std::string expected = 
        "H\n"
        "CNOT(0,1)\n";
    
    EXPECT_EQ(circuit.toASCII(), expected);
}

TEST(QuantumCircuitTest, CircuitExecution) {
    QuantumCircuit circuit(2);
    circuit.addGate(QuantumCircuit::GateType::H, {0});
    circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 1});
    
    QuantumState state(2);
    circuit.execute(state);
    
    // Verify Bell state
    const double inv_sqrt2 = 1.0 / std::sqrt(2.0);
    EXPECT_NEAR(std::abs(state.getState()[0]), inv_sqrt2, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[1]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[2]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[3]), inv_sqrt2, 1e-10);
}

TEST(QuantumCircuitTest, RotationGates) {
    QuantumCircuit circuit(1);
    circuit.addGate(QuantumCircuit::GateType::RX, {0}, M_PI/2);
    circuit.addGate(QuantumCircuit::GateType::RY, {0}, M_PI/4);
    circuit.addGate(QuantumCircuit::GateType::RZ, {0}, M_PI/8);
    
    QuantumState state(1);
    circuit.execute(state);
    
    // Verify final state
    const double expected_real = std::cos(M_PI/8) * std::cos(M_PI/4);
    const double expected_imag = std::sin(M_PI/8) * std::cos(M_PI/4);
    EXPECT_NEAR(state.getState()[0].real(), expected_real, 1e-10);
    EXPECT_NEAR(state.getState()[0].imag(), expected_imag, 1e-10);
} 