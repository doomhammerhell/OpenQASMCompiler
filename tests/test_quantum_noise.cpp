#include <gtest/gtest.h>
#include "../src/interpreter/quantum_noise.hpp"
#include <complex>
#include <cmath>

using namespace qasm;

TEST(QuantumNoiseTest, Initialization) {
    QuantumNoise noise(0.1);
    EXPECT_DOUBLE_EQ(noise.getErrorRate(), 0.1);
    EXPECT_EQ(noise.getNoiseType(), QuantumNoise::NoiseType::DEPOLARIZING);
}

TEST(QuantumNoiseTest, NoiseTypes) {
    QuantumNoise noise(0.1);
    
    // Test depolarizing noise
    noise.setNoiseType(QuantumNoise::NoiseType::DEPOLARIZING);
    EXPECT_EQ(noise.getNoiseType(), QuantumNoise::NoiseType::DEPOLARIZING);
    
    // Test amplitude damping
    noise.setNoiseType(QuantumNoise::NoiseType::AMPLITUDE_DAMPING);
    EXPECT_EQ(noise.getNoiseType(), QuantumNoise::NoiseType::AMPLITUDE_DAMPING);
    
    // Test phase damping
    noise.setNoiseType(QuantumNoise::NoiseType::PHASE_DAMPING);
    EXPECT_EQ(noise.getNoiseType(), QuantumNoise::NoiseType::PHASE_DAMPING);
    
    // Test bit flip
    noise.setNoiseType(QuantumNoise::NoiseType::BIT_FLIP);
    EXPECT_EQ(noise.getNoiseType(), QuantumNoise::NoiseType::BIT_FLIP);
    
    // Test phase flip
    noise.setNoiseType(QuantumNoise::NoiseType::PHASE_FLIP);
    EXPECT_EQ(noise.getNoiseType(), QuantumNoise::NoiseType::PHASE_FLIP);
    
    // Test bit-phase flip
    noise.setNoiseType(QuantumNoise::NoiseType::BIT_PHASE_FLIP);
    EXPECT_EQ(noise.getNoiseType(), QuantumNoise::NoiseType::BIT_PHASE_FLIP);
}

TEST(QuantumNoiseTest, ErrorRate) {
    QuantumNoise noise(0.1);
    EXPECT_DOUBLE_EQ(noise.getErrorRate(), 0.1);
    
    noise.setErrorRate(0.2);
    EXPECT_DOUBLE_EQ(noise.getErrorRate(), 0.2);
}

TEST(QuantumNoiseTest, CustomNoise) {
    QuantumNoise noise(0.1);
    std::vector<std::vector<std::complex<double>>> custom_matrix = {
        {1.0, 0.0},
        {0.0, 1.0},
        {0.0, 1.0},
        {1.0, 0.0}
    };
    
    noise.setCustomNoise(custom_matrix);
    auto matrix = noise.getNoiseMatrix();
    EXPECT_EQ(matrix.size(), custom_matrix.size());
    for (size_t i = 0; i < matrix.size(); ++i) {
        EXPECT_DOUBLE_EQ(matrix[i][0].real(), custom_matrix[i][0].real());
        EXPECT_DOUBLE_EQ(matrix[i][0].imag(), custom_matrix[i][0].imag());
    }
}

TEST(QuantumNoiseTest, NoiseApplication) {
    QuantumNoise noise(1.0); // 100% error rate for testing
    
    // Test single qubit noise
    std::vector<std::complex<double>> state = {1.0, 0.0};
    noise.applyNoise(state, 0);
    EXPECT_NE(state[0], std::complex<double>(1.0, 0.0));
    
    // Test two-qubit noise
    state = {1.0, 0.0, 0.0, 0.0};
    noise.applyNoise(state, 0, 1);
    EXPECT_NE(state[0], std::complex<double>(1.0, 0.0));
    
    // Test three-qubit noise
    state = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    noise.applyNoise(state, 0, 1, 2);
    EXPECT_NE(state[0], std::complex<double>(1.0, 0.0));
}

TEST(QuantumNoiseTest, NoisePreservation) {
    QuantumNoise noise(0.0); // 0% error rate
    
    // Test single qubit noise
    std::vector<std::complex<double>> state = {1.0, 0.0};
    noise.applyNoise(state, 0);
    EXPECT_EQ(state[0], std::complex<double>(1.0, 0.0));
    
    // Test two-qubit noise
    state = {1.0, 0.0, 0.0, 0.0};
    noise.applyNoise(state, 0, 1);
    EXPECT_EQ(state[0], std::complex<double>(1.0, 0.0));
    
    // Test three-qubit noise
    state = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    noise.applyNoise(state, 0, 1, 2);
    EXPECT_EQ(state[0], std::complex<double>(1.0, 0.0));
} 