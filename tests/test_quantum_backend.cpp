#include <gtest/gtest.h>
#include "../src/interpreter/quantum_backend.hpp"
#include "../src/interpreter/quantum_circuit.hpp"
#include <memory>

using namespace qasm;

TEST(QuantumBackendTest, LocalBackendCreation) {
    auto backend = QuantumBackendFactory::createBackend(QuantumBackend::BackendType::SIMULATOR);
    EXPECT_EQ(backend->getType(), QuantumBackend::BackendType::SIMULATOR);
    EXPECT_EQ(backend->getName(), "Local Simulator");
    EXPECT_TRUE(backend->isAvailable());
}

TEST(QuantumBackendTest, BackendConfiguration) {
    auto backend = QuantumBackendFactory::createBackend("local");
    
    QuantumBackend::BackendConfig config;
    config.max_qubits = 16;
    config.supported_gates = {"h", "x", "cx"};
    
    backend->configure(config);
    auto retrieved_config = backend->getConfig();
    
    EXPECT_EQ(retrieved_config.max_qubits, 16);
    EXPECT_EQ(retrieved_config.supported_gates.size(), 3);
}

TEST(QuantumBackendTest, CircuitExecution) {
    auto backend = QuantumBackendFactory::createBackend("local");
    QuantumCircuit circuit(2);
    circuit.addGate(QuantumCircuit::GateType::H, {0});
    circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 1});
    
    QuantumState state(2);
    backend->execute(circuit, state);
    
    // Verify Bell state
    const double inv_sqrt2 = 1.0 / std::sqrt(2.0);
    EXPECT_NEAR(std::abs(state.getState()[0]), inv_sqrt2, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[1]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[2]), 0.0, 1e-10);
    EXPECT_NEAR(std::abs(state.getState()[3]), inv_sqrt2, 1e-10);
}

TEST(QuantumBackendTest, Measurement) {
    auto backend = QuantumBackendFactory::createBackend("local");
    QuantumCircuit circuit(2);
    circuit.addGate(QuantumCircuit::GateType::H, {0});
    circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 1});
    
    auto results = backend->measure(circuit, 1000);
    EXPECT_EQ(results.size(), 1000);
    
    // Count results
    int zero_count = 0;
    int three_count = 0;
    for (int result : results) {
        if (result == 0) zero_count++;
        if (result == 3) three_count++;
    }
    
    // Verify approximately equal distribution
    EXPECT_NEAR(zero_count / 1000.0, 0.5, 0.1);
    EXPECT_NEAR(three_count / 1000.0, 0.5, 0.1);
}

TEST(QuantumBackendTest, ErrorHandling) {
    auto backend = QuantumBackendFactory::createBackend("local");
    
    // Test invalid circuit size
    QuantumCircuit circuit(33); // More than max qubits
    QuantumState state(33);
    EXPECT_THROW(backend->execute(circuit, state), std::runtime_error);
    
    // Test invalid measurement shots
    EXPECT_THROW(backend->measure(circuit, 0), std::runtime_error);
}

TEST(QuantumBackendTest, SupportedGates) {
    auto backend = QuantumBackendFactory::createBackend("local");
    auto gates = backend->getSupportedGates();
    
    EXPECT_GT(gates.size(), 0);
    EXPECT_NE(std::find(gates.begin(), gates.end(), "h"), gates.end());
    EXPECT_NE(std::find(gates.begin(), gates.end(), "cx"), gates.end());
    EXPECT_NE(std::find(gates.begin(), gates.end(), "measure"), gates.end());
} 