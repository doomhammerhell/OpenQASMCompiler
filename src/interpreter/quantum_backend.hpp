#ifndef QUANTUM_BACKEND_HPP
#define QUANTUM_BACKEND_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "quantum_circuit.hpp"
#include "quantum_state.hpp"

namespace qasm {

class QuantumBackend {
public:
    enum class BackendType {
        SIMULATOR,      // Local simulator
        IBM_QUANTUM,   // IBM Quantum Experience
        GOOGLE_QUANTUM,// Google Quantum Computing
        AMAZON_BRAKET, // Amazon Braket
        CUSTOM         // Custom backend
    };

    struct BackendConfig {
        std::string api_key;
        std::string endpoint;
        int max_qubits;
        std::vector<std::string> supported_gates;
        std::map<std::string, std::string> additional_config;
    };

    virtual ~QuantumBackend() = default;

    // Backend configuration
    virtual void configure(const BackendConfig& config) = 0;
    virtual BackendConfig getConfig() const = 0;

    // Circuit execution
    virtual void execute(const QuantumCircuit& circuit, QuantumState& state) = 0;
    virtual std::vector<int> measure(const QuantumCircuit& circuit, int shots = 1024) = 0;

    // Backend information
    virtual BackendType getType() const = 0;
    virtual std::string getName() const = 0;
    virtual int getMaxQubits() const = 0;
    virtual std::vector<std::string> getSupportedGates() const = 0;

    // Error handling
    virtual std::string getLastError() const = 0;
    virtual bool isAvailable() const = 0;
};

// Factory for creating backend instances
class QuantumBackendFactory {
public:
    static std::unique_ptr<QuantumBackend> createBackend(BackendType type);
    static std::unique_ptr<QuantumBackend> createBackend(const std::string& name);
};

} // namespace qasm

#endif // QUANTUM_BACKEND_HPP 