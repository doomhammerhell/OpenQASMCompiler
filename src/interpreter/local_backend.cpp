#include "local_backend.hpp"
#include <algorithm>
#include <stdexcept>

namespace qasm {

LocalBackend::LocalBackend() : max_qubits(32) {
    initializeSupportedGates();
}

void LocalBackend::configure(const BackendConfig& config) {
    this->config = config;
    max_qubits = config.max_qubits;
}

BackendConfig LocalBackend::getConfig() const {
    return config;
}

void LocalBackend::execute(const QuantumCircuit& circuit, QuantumState& state) {
    if (circuit.getNumQubits() > max_qubits) {
        last_error = "Circuit requires more qubits than available";
        throw std::runtime_error(last_error);
    }

    try {
        circuit.execute(state);
    } catch (const std::exception& e) {
        last_error = e.what();
        throw;
    }
}

std::vector<int> LocalBackend::measure(const QuantumCircuit& circuit, int shots) {
    if (shots <= 0) {
        last_error = "Number of shots must be positive";
        throw std::runtime_error(last_error);
    }

    QuantumState state(circuit.getNumQubits());
    circuit.execute(state);

    std::vector<int> results;
    results.reserve(shots);

    for (int i = 0; i < shots; ++i) {
        int result = 0;
        for (int qubit = 0; qubit < circuit.getNumQubits(); ++qubit) {
            if (state.measure(qubit)) {
                result |= (1 << qubit);
            }
        }
        results.push_back(result);
    }

    return results;
}

std::vector<std::string> LocalBackend::getSupportedGates() const {
    return supported_gates;
}

void LocalBackend::initializeSupportedGates() {
    supported_gates = {
        "h",    // Hadamard
        "x",    // Pauli-X
        "y",    // Pauli-Y
        "z",    // Pauli-Z
        "cx",   // CNOT
        "swap", // SWAP
        "cz",   // CZ
        "ccx",  // Toffoli
        "cswap",// Fredkin
        "rx",   // Rotation X
        "ry",   // Rotation Y
        "rz",   // Rotation Z
        "measure" // Measurement
    };
}

} // namespace qasm 