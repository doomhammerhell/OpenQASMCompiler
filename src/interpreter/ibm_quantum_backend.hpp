#ifndef IBM_QUANTUM_BACKEND_HPP
#define IBM_QUANTUM_BACKEND_HPP

#include "quantum_backend.hpp"
#include <string>
#include <vector>
#include <memory>
#include <curl/curl.h>

namespace qasm {

class IBMQuantumBackend : public QuantumBackend {
public:
    IBMQuantumBackend();
    ~IBMQuantumBackend() override;

    // Backend configuration
    void configure(const BackendConfig& config) override;
    BackendConfig getConfig() const override;

    // Circuit execution
    void execute(const QuantumCircuit& circuit, QuantumState& state) override;
    std::vector<int> measure(const QuantumCircuit& circuit, int shots = 1024) override;

    // Backend information
    BackendType getType() const override { return BackendType::IBM_QUANTUM; }
    std::string getName() const override { return "IBM Quantum"; }
    int getMaxQubits() const override;
    std::vector<std::string> getSupportedGates() const override;

    // Error handling
    std::string getLastError() const override { return last_error; }
    bool isAvailable() const override;

    // IBM Quantum specific methods
    std::vector<std::string> getAvailableDevices() const;
    void selectDevice(const std::string& device_name);
    double getDeviceErrorRate() const;
    int getDeviceQueueLength() const;

private:
    BackendConfig config;
    std::string last_error;
    std::string selected_device;
    CURL* curl_handle;
    bool is_initialized;

    // Helper methods
    void initializeCurl();
    void cleanupCurl();
    std::string makeApiRequest(const std::string& endpoint, const std::string& method = "GET");
    void handleApiError(const std::string& error);
    std::string convertCircuitToIBMFormat(const QuantumCircuit& circuit) const;
    std::vector<int> parseMeasurementResults(const std::string& response) const;
};

} // namespace qasm

#endif // IBM_QUANTUM_BACKEND_HPP 