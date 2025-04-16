#include "quantum_backend.hpp"
#include "local_backend.hpp"
#include <stdexcept>

namespace qasm {

std::unique_ptr<QuantumBackend> QuantumBackendFactory::createBackend(BackendType type) {
    switch (type) {
        case BackendType::SIMULATOR:
            return std::make_unique<LocalBackend>();
        case BackendType::IBM_QUANTUM:
        case BackendType::GOOGLE_QUANTUM:
        case BackendType::AMAZON_BRAKET:
        case BackendType::CUSTOM:
            throw std::runtime_error("Backend type not implemented yet");
        default:
            throw std::runtime_error("Unknown backend type");
    }
}

std::unique_ptr<QuantumBackend> QuantumBackendFactory::createBackend(const std::string& name) {
    if (name == "local" || name == "simulator") {
        return std::make_unique<LocalBackend>();
    }
    // Add other backend names here as they are implemented
    throw std::runtime_error("Backend '" + name + "' not implemented yet");
}

} // namespace qasm 