#include <gtest/gtest.h>
#include "OpenQASMCompiler/quantum_simulator.hpp"
#include "OpenQASMCompiler/quantum_circuit.hpp"
#include "OpenQASMCompiler/quantum_gates.hpp"
#include <complex>
#include <cmath>

using namespace qasm;

class QuantumSimulatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        simulator = std::make_unique<QuantumSimulator>(2);
        circuit = std::make_unique<QuantumCircuit>(2);
    }

    std::unique_ptr<QuantumSimulator> simulator;
    std::unique_ptr<QuantumCircuit> circuit;
};

TEST_F(QuantumSimulatorTest, InitialState) {
    const auto& state = simulator->getStateVector();
    EXPECT_EQ(state.size(), 4);
    EXPECT_DOUBLE_EQ(std::norm(state[0]), 1.0);
    for (size_t i = 1; i < 4; ++i) {
        EXPECT_DOUBLE_EQ(std::norm(state[i]), 0.0);
    }
}

TEST_F(QuantumSimulatorTest, HadamardGate) {
    circuit->addGate(std::make_unique<HGate>(0));
    simulator->simulate(*circuit);
    
    const auto& state = simulator->getStateVector();
    EXPECT_NEAR(std::norm(state[0]), 0.5, 1e-10);
    EXPECT_NEAR(std::norm(state[1]), 0.5, 1e-10);
    EXPECT_NEAR(std::norm(state[2]), 0.0, 1e-10);
    EXPECT_NEAR(std::norm(state[3]), 0.0, 1e-10);
}

TEST_F(QuantumSimulatorTest, CNOTGate) {
    circuit->addGate(std::make_unique<HGate>(0));
    circuit->addGate(std::make_unique<CNOTGate>(0, 1));
    simulator->simulate(*circuit);
    
    const auto& state = simulator->getStateVector();
    EXPECT_NEAR(std::norm(state[0]), 0.5, 1e-10);
    EXPECT_NEAR(std::norm(state[1]), 0.0, 1e-10);
    EXPECT_NEAR(std::norm(state[2]), 0.0, 1e-10);
    EXPECT_NEAR(std::norm(state[3]), 0.5, 1e-10);
}

TEST_F(QuantumSimulatorTest, Measurement) {
    circuit->addGate(std::make_unique<HGate>(0));
    simulator->simulate(*circuit);
    
    std::vector<bool> state = {false, false};
    double prob = simulator->getProbability(state);
    EXPECT_NEAR(prob, 0.5, 1e-10);
    
    state = {true, false};
    prob = simulator->getProbability(state);
    EXPECT_NEAR(prob, 0.5, 1e-10);
}

TEST_F(QuantumSimulatorTest, MeasurementStats) {
    circuit->addGate(std::make_unique<HGate>(0));
    simulator->simulate(*circuit);
    
    auto stats = simulator->getMeasurementStats(1000);
    EXPECT_NEAR(stats["00"] / 1000.0, 0.5, 0.1);
    EXPECT_NEAR(stats["01"] / 1000.0, 0.0, 0.1);
    EXPECT_NEAR(stats["10"] / 1000.0, 0.5, 0.1);
    EXPECT_NEAR(stats["11"] / 1000.0, 0.0, 0.1);
}

TEST_F(QuantumSimulatorTest, NoiseModels) {
    // Test depolarizing noise
    simulator = std::make_unique<QuantumSimulator>(1, NoiseModel::DEPOLARIZING, 0.1);
    circuit = std::make_unique<QuantumCircuit>(1);
    circuit->addGate(std::make_unique<HGate>(0));
    simulator->simulate(*circuit, 1000);
    
    auto stats = simulator->getMeasurementStats(1000);
    EXPECT_NEAR(stats["0"] / 1000.0, 0.5, 0.1);
    EXPECT_NEAR(stats["1"] / 1000.0, 0.5, 0.1);
    
    // Test amplitude damping noise
    simulator = std::make_unique<QuantumSimulator>(1, NoiseModel::AMPLITUDE_DAMPING, 0.1);
    circuit = std::make_unique<QuantumCircuit>(1);
    circuit->addGate(std::make_unique<HGate>(0));
    simulator->simulate(*circuit, 1000);
    
    stats = simulator->getMeasurementStats(1000);
    EXPECT_GT(stats["0"] / 1000.0, 0.5);  // More likely to measure |0⟩
}

TEST_F(QuantumSimulatorTest, DensityMatrix) {
    circuit->addGate(std::make_unique<HGate>(0));
    simulator->simulate(*circuit);
    
    auto rho = simulator->getDensityMatrix();
    EXPECT_NEAR(std::real(rho[0][0]), 0.5, 1e-10);
    EXPECT_NEAR(std::real(rho[0][1]), 0.5, 1e-10);
    EXPECT_NEAR(std::real(rho[1][0]), 0.5, 1e-10);
    EXPECT_NEAR(std::real(rho[1][1]), 0.5, 1e-10);
}

TEST_F(QuantumSimulatorTest, ParameterizedGates) {
    circuit->addGate(std::make_unique<U3Gate>(0, M_PI/2, 0, 0));
    simulator->simulate(*circuit);
    
    const auto& state = simulator->getStateVector();
    EXPECT_NEAR(std::norm(state[0]), 0.5, 1e-10);
    EXPECT_NEAR(std::norm(state[1]), 0.5, 1e-10);
}

TEST_F(QuantumSimulatorTest, Reset) {
    circuit->addGate(std::make_unique<HGate>(0));
    simulator->simulate(*circuit);
    simulator->reset();
    
    const auto& state = simulator->getStateVector();
    EXPECT_DOUBLE_EQ(std::norm(state[0]), 1.0);
    for (size_t i = 1; i < 4; ++i) {
        EXPECT_DOUBLE_EQ(std::norm(state[i]), 0.0);
    }
}

TEST_F(QuantumSimulatorTest, ExpectationValue) {
    circuit->addGate(std::make_unique<HGate>(0));
    simulator->simulate(*circuit);
    
    // Pauli X observable
    std::vector<std::vector<std::complex<double>>> X = {
        {0, 1},
        {1, 0}
    };
    
    double expectation = simulator->getExpectationValue(X);
    EXPECT_NEAR(expectation, 1.0, 1e-10);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 