#ifndef LOCAL_BACKEND_HPP
#define LOCAL_BACKEND_HPP

#include "quantum_backend.hpp"
#include "quantum_circuit.hpp"
#include "quantum_state.hpp"

namespace qasm {

class LocalBackend : public QuantumBackend {
public:
    LocalBackend();
    ~LocalBackend() override = default;

    // Backend configuration
    void configure(const BackendConfig& config) override;
    BackendConfig getConfig() const override;

    // Circuit execution
    void execute(const QuantumCircuit& circuit, QuantumState& state) override;
    std::vector<int> measure(const QuantumCircuit& circuit, int shots = 1024) override;

    // Backend information
    BackendType getType() const override { return BackendType::SIMULATOR; }
    std::string getName() const override { return "Local Simulator"; }
    int getMaxQubits() const override { return max_qubits; }
    std::vector<std::string> getSupportedGates() const override;

    // Error handling
    std::string getLastError() const override { return last_error; }
    bool isAvailable() const override { return true; }

private:
    BackendConfig config;
    int max_qubits;
    std::string last_error;
    std::vector<std::string> supported_gates;

    void initializeSupportedGates();
};

} // namespace qasm

#endif // LOCAL_BACKEND_HPP 