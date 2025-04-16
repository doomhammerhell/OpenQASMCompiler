#include "OpenQASMCompiler/quantum_circuit.hpp"
#include "OpenQASMCompiler/quantum_gates.hpp"
#include <gtest/gtest.h>
#include <complex>
#include <cmath>

namespace qasm {
namespace test {

class QuantumCircuitTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a simple circuit for testing
        circuit = std::make_unique<QuantumCircuit>(2); // 2 qubits
        
        // Add gates to create a Bell state
        circuit->addGate(std::make_unique<HGate>(0));
        circuit->addGate(std::make_unique<CXGate>(0, 1));
    }
    
    std::unique_ptr<QuantumCircuit> circuit;
};

TEST_F(QuantumCircuitTest, CircuitInitialization) {
    EXPECT_EQ(circuit->getNumQubits(), 2);
    EXPECT_EQ(circuit->getGates().size(), 2);
}

TEST_F(QuantumCircuitTest, GateApplication) {
    // Create a new circuit with known initial state
    auto testCircuit = std::make_unique<QuantumCircuit>(1);
    testCircuit->addGate(std::make_unique<HGate>(0));
    
    // Apply the circuit
    testCircuit->apply();
    
    // Verify the state after H gate
    auto state = testCircuit->getState();
    EXPECT_NEAR(std::abs(state[0]), 1.0 / std::sqrt(2.0), 1e-10);
    EXPECT_NEAR(std::abs(state[1]), 1.0 / std::sqrt(2.0), 1e-10);
}

TEST_F(QuantumCircuitTest, BellStateCreation) {
    // Apply the circuit
    circuit->apply();
    
    // Verify Bell state |Φ⁺⟩ = (|00⟩ + |11⟩)/√2
    auto state = circuit->getState();
    EXPECT_NEAR(std::abs(state[0]), 1.0 / std::sqrt(2.0), 1e-10);
    EXPECT_NEAR(std::abs(state[1]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state[2]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state[3]), 1.0 / std::sqrt(2.0), 1e-10);
}

TEST_F(QuantumCircuitTest, Measurement) {
    // Apply the circuit
    circuit->apply();
    
    // Perform multiple measurements
    std::vector<int> results(4, 0);
    const int numShots = 1000;
    
    for (int i = 0; i < numShots; ++i) {
        auto measurement = circuit->measure();
        results[measurement]++;
    }
    
    // Verify measurement statistics
    EXPECT_NEAR(results[0] / static_cast<double>(numShots), 0.5, 0.1);
    EXPECT_NEAR(results[1] / static_cast<double>(numShots), 0.0, 0.1);
    EXPECT_NEAR(results[2] / static_cast<double>(numShots), 0.0, 0.1);
    EXPECT_NEAR(results[3] / static_cast<double>(numShots), 0.5, 0.1);
}

TEST_F(QuantumCircuitTest, GateInverse) {
    // Create a circuit with a gate and its inverse
    auto testCircuit = std::make_unique<QuantumCircuit>(1);
    testCircuit->addGate(std::make_unique<HGate>(0));
    testCircuit->addGate(std::make_unique<HGate>(0)); // H† = H
    
    // Apply the circuit
    testCircuit->apply();
    
    // Verify we're back to |0⟩
    auto state = testCircuit->getState();
    EXPECT_NEAR(std::abs(state[0]), 1.0, 1e-10);
    EXPECT_NEAR(std::abs(state[1]), 0.0, 1e-10);
}

TEST_F(QuantumCircuitTest, ControlledGates) {
    // Create a circuit with controlled gates
    auto testCircuit = std::make_unique<QuantumCircuit>(2);
    testCircuit->addGate(std::make_unique<XGate>(0)); // |10⟩
    testCircuit->addGate(std::make_unique<CXGate>(0, 1)); // |11⟩
    
    // Apply the circuit
    testCircuit->apply();
    
    // Verify final state
    auto state = testCircuit->getState();
    EXPECT_NEAR(std::abs(state[0]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state[1]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state[2]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state[3]), 1.0, 1e-10);
}

TEST_F(QuantumCircuitTest, ParameterizedGates) {
    // Create a circuit with parameterized gates
    auto testCircuit = std::make_unique<QuantumCircuit>(1);
    testCircuit->addGate(std::make_unique<RXGate>(0, M_PI/2)); // Rotation by π/2
    
    // Apply the circuit
    testCircuit->apply();
    
    // Verify final state
    auto state = testCircuit->getState();
    EXPECT_NEAR(std::abs(state[0]), std::cos(M_PI/4), 1e-10);
    EXPECT_NEAR(std::abs(state[1]), -std::sin(M_PI/4), 1e-10);
}

TEST_F(QuantumCircuitTest, CircuitDepth) {
    EXPECT_EQ(circuit->getDepth(), 2);
    
    // Add more gates
    circuit->addGate(std::make_unique<HGate>(0));
    circuit->addGate(std::make_unique<HGate>(1));
    
    EXPECT_EQ(circuit->getDepth(), 3);
}

TEST_F(QuantumCircuitTest, CircuitWidth) {
    EXPECT_EQ(circuit->getWidth(), 2);
    
    // Create a wider circuit
    auto wideCircuit = std::make_unique<QuantumCircuit>(4);
    EXPECT_EQ(wideCircuit->getWidth(), 4);
}

TEST_F(QuantumCircuitTest, CircuitReset) {
    // Apply the circuit
    circuit->apply();
    
    // Reset the circuit
    circuit->reset();
    
    // Verify we're back to |00⟩
    auto state = circuit->getState();
    EXPECT_NEAR(std::abs(state[0]), 1.0, 1e-10);
    EXPECT_NEAR(std::abs(state[1]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state[2]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state[3]), 0.0, 1e-10);
}

} // namespace test
} // namespace qasm 