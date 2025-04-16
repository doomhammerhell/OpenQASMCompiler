#ifndef GOOGLE_QUANTUM_BACKEND_HPP
#define GOOGLE_QUANTUM_BACKEND_HPP

#include "quantum_backend.hpp"
#include <string>
#include <vector>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "google/cloud/quantum/v1/quantum_engine.grpc.pb.h"

namespace qasm {

class GoogleQuantumBackend : public QuantumBackend {
public:
    GoogleQuantumBackend();
    ~GoogleQuantumBackend() override;

    // Backend configuration
    void configure(const BackendConfig& config) override;
    BackendConfig getConfig() const override;

    // Circuit execution
    void execute(const QuantumCircuit& circuit, QuantumState& state) override;
    std::vector<int> measure(const QuantumCircuit& circuit, int shots = 1024) override;

    // Backend information
    BackendType getType() const override { return BackendType::GOOGLE_QUANTUM; }
    std::string getName() const override { return "Google Quantum"; }
    int getMaxQubits() const override;
    std::vector<std::string> getSupportedGates() const override;

    // Error handling
    std::string getLastError() const override { return last_error; }
    bool isAvailable() const override;

    // Google Quantum specific methods
    std::vector<std::string> getAvailableProcessors() const;
    void selectProcessor(const std::string& processor_name);
    double getProcessorFidelity() const;
    int getProcessorQueueLength() const;
    std::vector<double> getProcessorCalibrationData() const;

private:
    BackendConfig config;
    std::string last_error;
    std::string selected_processor;
    std::unique_ptr<google::cloud::quantum::v1::QuantumEngineService::Stub> stub;
    bool is_initialized;

    // Helper methods
    void initializeGrpc();
    void cleanupGrpc();
    void handleGrpcError(const grpc::Status& status);
    std::string convertCircuitToGoogleFormat(const QuantumCircuit& circuit) const;
    std::vector<int> parseMeasurementResults(const google::cloud::quantum::v1::RunQuantumProgramResponse& response) const;
    void updateProcessorCalibration();
};

} // namespace qasm

#endif // GOOGLE_QUANTUM_BACKEND_HPP 