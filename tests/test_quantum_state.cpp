#include <gtest/gtest.h>
#include "../src/interpreter/quantum_state.hpp"
#include <complex>
#include <cmath>

using namespace qasm;

TEST(QuantumStateTest, Initialization) {
    QuantumState state(2);
    EXPECT_EQ(state.getNumQubits(), 2);
    EXPECT_EQ(state.getState().size(), 4);
    EXPECT_DOUBLE_EQ(std::norm(state.getState()[0]), 1.0);
    EXPECT_DOUBLE_EQ(std::norm(state.getState()[1]), 0.0);
    EXPECT_DOUBLE_EQ(std::norm(state.getState()[2]), 0.0);
    EXPECT_DOUBLE_EQ(std::norm(state.getState()[3]), 0.0);
}

TEST(QuantumStateTest, HadamardGate) {
    QuantumState state(1);
    state.applyHadamard(0);
    
    const double inv_sqrt2 = 1.0 / std::sqrt(2.0);
    EXPECT_NEAR(std::abs(state.getState()[0]), inv_sqrt2, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[1]), inv_sqrt2, 1e-10);
}

TEST(QuantumStateTest, CNOTGate) {
    QuantumState state(2);
    state.applyHadamard(0);
    state.applyCNOT(0, 1);
    
    const double inv_sqrt2 = 1.0 / std::sqrt(2.0);
    EXPECT_NEAR(std::abs(state.getState()[0]), inv_sqrt2, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[1]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[2]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[3]), inv_sqrt2, 1e-10);
}

TEST(QuantumStateTest, PauliGates) {
    QuantumState state(1);
    
    // Test X gate
    state.applyX(0);
    EXPECT_NEAR(std::abs(state.getState()[0]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[1]), 1.0, 1e-10);
    
    // Reset state
    state = QuantumState(1);
    
    // Test Y gate
    state.applyY(0);
    EXPECT_NEAR(std::abs(state.getState()[0]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[1]), 1.0, 1e-10);
    
    // Reset state
    state = QuantumState(1);
    
    // Test Z gate
    state.applyHadamard(0);
    state.applyZ(0);
    EXPECT_NEAR(std::abs(state.getState()[0]), 1.0/std::sqrt(2.0), 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[1]), -1.0/std::sqrt(2.0), 1e-10);
}

TEST(QuantumStateTest, RotationGates) {
    QuantumState state(1);
    const double angle = M_PI/4;
    
    // Test Rx gate
    state.applyRx(0, angle);
    EXPECT_NEAR(std::abs(state.getState()[0]), std::cos(angle/2), 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[1]), -std::sin(angle/2), 1e-10);
    
    // Reset state
    state = QuantumState(1);
    
    // Test Ry gate
    state.applyRy(0, angle);
    EXPECT_NEAR(std::abs(state.getState()[0]), std::cos(angle/2), 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[1]), std::sin(angle/2), 1e-10);
    
    // Reset state
    state = QuantumState(1);
    
    // Test Rz gate
    state.applyHadamard(0);
    state.applyRz(0, angle);
    EXPECT_NEAR(std::abs(state.getState()[0]), 1.0/std::sqrt(2.0), 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[1]), 1.0/std::sqrt(2.0), 1e-10);
}

TEST(QuantumStateTest, Measurement) {
    QuantumState state(1);
    state.applyHadamard(0);
    
    // Perform multiple measurements to verify probabilities
    const int num_measurements = 10000;
    int count_0 = 0;
    int count_1 = 0;
    
    for (int i = 0; i < num_measurements; ++i) {
        QuantumState temp_state = state;
        if (temp_state.measure(0)) {
            count_1++;
        } else {
            count_0++;
        }
    }
    
    // Check if measurements follow expected probabilities
    double prob_0 = static_cast<double>(count_0) / num_measurements;
    double prob_1 = static_cast<double>(count_1) / num_measurements;
    
    EXPECT_NEAR(prob_0, 0.5, 0.05);
    EXPECT_NEAR(prob_1, 0.5, 0.05);
}

TEST(QuantumStateTest, Normalization) {
    QuantumState state(1);
    state.applyHadamard(0);
    
    // Verify state is normalized
    double norm = 0.0;
    for (const auto& amplitude : state.getState()) {
        norm += std::norm(amplitude);
    }
    EXPECT_NEAR(norm, 1.0, 1e-10);
    
    // Test normalization after measurement
    state.measure(0);
    norm = 0.0;
    for (const auto& amplitude : state.getState()) {
        norm += std::norm(amplitude);
    }
    EXPECT_NEAR(norm, 1.0, 1e-10);
}

TEST(QuantumStateTest, ThreeQubitGates) {
    QuantumState state(3);
    
    // Test Toffoli gate
    state.applyHadamard(0);
    state.applyHadamard(1);
    state.applyToffoli(0, 1, 2);
    
    // Verify state is in superposition
    const double inv_sqrt2 = 1.0 / std::sqrt(2.0);
    EXPECT_NEAR(std::abs(state.getState()[0]), inv_sqrt2, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[1]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[2]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[3]), inv_sqrt2, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[4]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[5]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[6]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[7]), 0.0, 1e-10);
    
    // Test Fredkin gate
    state = QuantumState(3);
    state.applyHadamard(0);
    state.applyFredkin(0, 1, 2);
    
    // Verify state is in superposition
    EXPECT_NEAR(std::abs(state.getState()[0]), inv_sqrt2, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[1]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[2]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[3]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[4]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[5]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[6]), inv_sqrt2, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[7]), 0.0, 1e-10);
}

TEST(QuantumStateTest, StateCache) {
    QuantumState state(2);
    
    // Apply some gates
    state.applyHadamard(0);
    state.applyCNOT(0, 1);
    
    // Save state
    state.saveState("entangled");
    
    // Modify state
    state.applyX(0);
    state.applyX(1);
    
    // Load saved state
    state.loadState("entangled");
    
    // Verify state is restored
    const double inv_sqrt2 = 1.0 / std::sqrt(2.0);
    EXPECT_NEAR(std::abs(state.getState()[0]), inv_sqrt2, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[1]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[2]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[3]), inv_sqrt2, 1e-10);
    
    // Test error handling
    EXPECT_THROW(state.loadState("nonexistent"), std::runtime_error);
    
    // Clear cache
    state.clearCache();
    EXPECT_THROW(state.loadState("entangled"), std::runtime_error);
} 