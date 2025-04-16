#include <gtest/gtest.h>
#include "../src/interpreter/quantum_debugger.hpp"
#include <complex>
#include <cmath>

using namespace qasm;

TEST(QuantumDebuggerTest, Initialization) {
    QuantumCircuit circuit(2);
    QuantumState state(2);
    QuantumDebugger debugger(circuit, state);
    
    EXPECT_EQ(debugger.getCurrentGateIndex(), 0);
    EXPECT_TRUE(debugger.getBreakpoints().empty());
}

TEST(QuantumDebuggerTest, Breakpoints) {
    QuantumCircuit circuit(2);
    QuantumState state(2);
    QuantumDebugger debugger(circuit, state);
    
    // Add gate breakpoint
    debugger.addGateBreakpoint(1);
    EXPECT_EQ(debugger.getBreakpoints().size(), 1);
    
    // Add state breakpoint
    debugger.addStateBreakpoint(
        [](const QuantumState& state) { return true; },
        "Test breakpoint"
    );
    EXPECT_EQ(debugger.getBreakpoints().size(), 2);
    
    // Add probability breakpoint
    debugger.addProbabilityBreakpoint(0, 0.5);
    EXPECT_EQ(debugger.getBreakpoints().size(), 3);
    
    // Remove breakpoint
    debugger.removeBreakpoint(0);
    EXPECT_EQ(debugger.getBreakpoints().size(), 2);
    
    // Clear breakpoints
    debugger.clearBreakpoints();
    EXPECT_TRUE(debugger.getBreakpoints().empty());
}

TEST(QuantumDebuggerTest, StepExecution) {
    QuantumCircuit circuit(2);
    circuit.addGate(QuantumCircuit::GateType::H, {0});
    circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 1});
    
    QuantumState state(2);
    QuantumDebugger debugger(circuit, state);
    
    // Step through first gate
    debugger.step();
    EXPECT_EQ(debugger.getCurrentGateIndex(), 1);
    
    // Step through second gate
    debugger.step();
    EXPECT_EQ(debugger.getCurrentGateIndex(), 2);
}

TEST(QuantumDebuggerTest, ContinueExecution) {
    QuantumCircuit circuit(2);
    circuit.addGate(QuantumCircuit::GateType::H, {0});
    circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 1});
    
    QuantumState state(2);
    QuantumDebugger debugger(circuit, state);
    
    // Add breakpoint at second gate
    debugger.addGateBreakpoint(1);
    
    // Continue execution
    debugger.continueExecution();
    EXPECT_EQ(debugger.getCurrentGateIndex(), 1);
}

TEST(QuantumDebuggerTest, Reset) {
    QuantumCircuit circuit(2);
    circuit.addGate(QuantumCircuit::GateType::H, {0});
    
    QuantumState state(2);
    QuantumDebugger debugger(circuit, state);
    
    // Execute some gates
    debugger.step();
    EXPECT_EQ(debugger.getCurrentGateIndex(), 1);
    
    // Reset
    debugger.reset();
    EXPECT_EQ(debugger.getCurrentGateIndex(), 0);
}

TEST(QuantumDebuggerTest, StateAnalysis) {
    QuantumCircuit circuit(2);
    circuit.addGate(QuantumCircuit::GateType::H, {0});
    circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 1});
    
    QuantumState state(2);
    QuantumDebugger debugger(circuit, state);
    
    // Execute circuit
    debugger.continueExecution();
    
    // Check probabilities
    EXPECT_NEAR(debugger.getProbability(0, true), 0.5, 1e-10);
    EXPECT_NEAR(debugger.getProbability(1, true), 0.5, 1e-10);
    
    // Check entanglement
    EXPECT_NEAR(debugger.getEntanglement(0, 1), 1.0, 1e-10);
    
    // Check state probabilities
    auto probs = debugger.getStateProbabilities();
    EXPECT_EQ(probs.size(), 4);
    EXPECT_NEAR(probs[0], 0.5, 1e-10);
    EXPECT_NEAR(probs[1], 0.0, 1e-10);
    EXPECT_NEAR(probs[2], 0.0, 1e-10);
    EXPECT_NEAR(probs[3], 0.5, 1e-10);
}

TEST(QuantumDebuggerTest, StateInfo) {
    QuantumCircuit circuit(2);
    circuit.addGate(QuantumCircuit::GateType::H, {0});
    
    QuantumState state(2);
    QuantumDebugger debugger(circuit, state);
    
    // Execute circuit
    debugger.step();
    
    // Get state info
    std::string info = debugger.getStateInfo();
    EXPECT_FALSE(info.empty());
    EXPECT_NE(info.find("Current Gate: 1"), std::string::npos);
    EXPECT_NE(info.find("State Probabilities:"), std::string::npos);
} 