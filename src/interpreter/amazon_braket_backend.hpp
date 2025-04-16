#ifndef AMAZON_BRAKET_BACKEND_HPP
#define AMAZON_BRAKET_BACKEND_HPP

#include "quantum_backend.hpp"
#include <string>
#include <vector>
#include <memory>
#include <aws/core/Aws.h>
#include <aws/braket/BraketClient.h>

namespace qasm {

class AmazonBraketBackend : public QuantumBackend {
public:
    AmazonBraketBackend();
    ~AmazonBraketBackend() override;

    // Backend configuration
    void configure(const BackendConfig& config) override;
    BackendConfig getConfig() const override;

    // Circuit execution
    void execute(const QuantumCircuit& circuit, QuantumState& state) override;
    std::vector<int> measure(const QuantumCircuit& circuit, int shots = 1024) override;

    // Backend information
    BackendType getType() const override { return BackendType::AMAZON_BRAKET; }
    std::string getName() const override { return "Amazon Braket"; }
    int getMaxQubits() const override;
    std::vector<std::string> getSupportedGates() const override;

    // Error handling
    std::string getLastError() const override { return last_error; }
    bool isAvailable() const override;

    // Amazon Braket specific methods
    std::vector<std::string> getAvailableDevices() const;
    void selectDevice(const std::string& device_name);
    double getDeviceErrorRate() const;
    int getDeviceQueueLength() const;
    std::vector<std::string> getAvailableTasks() const;
    std::string getTaskStatus(const std::string& task_id) const;
    void cancelTask(const std::string& task_id);

private:
    BackendConfig config;
    std::string last_error;
    std::string selected_device;
    std::unique_ptr<Aws::Braket::BraketClient> client;
    Aws::SDKOptions options;
    bool is_initialized;

    // Helper methods
    void initializeAws();
    void cleanupAws();
    void handleAwsError(const Aws::Client::AWSError<Aws::Braket::BraketErrors>& error);
    std::string convertCircuitToBraketFormat(const QuantumCircuit& circuit) const;
    std::vector<int> parseMeasurementResults(const Aws::Braket::Model::GetQuantumTaskResult& result) const;
    void updateDeviceStatus();
};

} // namespace qasm

#endif // AMAZON_BRAKET_BACKEND_HPP 