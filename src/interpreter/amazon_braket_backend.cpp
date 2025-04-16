#include "amazon_braket_backend.hpp"
#include <aws/core/utils/json/JsonSerializer.h>
#include <sstream>
#include <stdexcept>

namespace qasm {

AmazonBraketBackend::AmazonBraketBackend() : is_initialized(false) {
    initializeAws();
}

AmazonBraketBackend::~AmazonBraketBackend() {
    cleanupAws();
}

void AmazonBraketBackend::initializeAws() {
    Aws::InitAPI(options);
    Aws::Client::ClientConfiguration config;
    config.region = "us-west-2"; // Default region for Braket
    client = std::make_unique<Aws::Braket::BraketClient>(config);
    is_initialized = true;
}

void AmazonBraketBackend::cleanupAws() {
    client.reset();
    Aws::ShutdownAPI(options);
    is_initialized = false;
}

void AmazonBraketBackend::configure(const BackendConfig& config) {
    this->config = config;
    if (config.api_key.empty()) {
        throw std::runtime_error("API key is required for Amazon Braket backend");
    }
}

BackendConfig AmazonBraketBackend::getConfig() const {
    return config;
}

void AmazonBraketBackend::execute(const QuantumCircuit& circuit, QuantumState& state) {
    if (!is_initialized) {
        throw std::runtime_error("Backend not initialized");
    }

    Aws::Braket::Model::CreateQuantumTaskRequest request;
    request.SetDeviceArn(selected_device);
    request.SetProgram(convertCircuitToBraketFormat(circuit));

    auto outcome = client->CreateQuantumTask(request);
    if (!outcome.IsSuccess()) {
        handleAwsError(outcome.GetError());
    }
}

std::vector<int> AmazonBraketBackend::measure(const QuantumCircuit& circuit, int shots) {
    if (!is_initialized) {
        throw std::runtime_error("Backend not initialized");
    }

    Aws::Braket::Model::CreateQuantumTaskRequest request;
    request.SetDeviceArn(selected_device);
    request.SetProgram(convertCircuitToBraketFormat(circuit));
    request.SetShots(shots);

    auto outcome = client->CreateQuantumTask(request);
    if (!outcome.IsSuccess()) {
        handleAwsError(outcome.GetError());
    }

    // Wait for task completion
    std::string task_id = outcome.GetResult().GetQuantumTaskArn();
    while (getTaskStatus(task_id) == "RUNNING") {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    Aws::Braket::Model::GetQuantumTaskRequest get_request;
    get_request.SetQuantumTaskArn(task_id);
    auto get_outcome = client->GetQuantumTask(get_request);
    if (!get_outcome.IsSuccess()) {
        handleAwsError(get_outcome.GetError());
    }

    return parseMeasurementResults(get_outcome.GetResult());
}

int AmazonBraketBackend::getMaxQubits() const {
    if (selected_device.empty()) {
        return 30; // Default for Amazon Braket devices
    }

    Aws::Braket::Model::GetDeviceRequest request;
    request.SetDeviceArn(selected_device);

    auto outcome = client->GetDevice(request);
    if (outcome.IsSuccess()) {
        return outcome.GetResult().GetDeviceCapabilities().GetQubitCount();
    }
    return 30;
}

std::vector<std::string> AmazonBraketBackend::getSupportedGates() const {
    return {
        "h", "x", "y", "z", "s", "t", "cx", "cz", "swap",
        "rx", "ry", "rz", "cnot", "ccnot", "cswap"
    };
}

bool AmazonBraketBackend::isAvailable() const {
    if (!is_initialized) return false;
    
    Aws::Braket::Model::GetDeviceRequest request;
    request.SetDeviceArn(selected_device);

    auto outcome = client->GetDevice(request);
    return outcome.IsSuccess();
}

std::vector<std::string> AmazonBraketBackend::getAvailableDevices() const {
    Aws::Braket::Model::SearchDevicesRequest request;
    auto outcome = client->SearchDevices(request);
    if (!outcome.IsSuccess()) {
        handleAwsError(outcome.GetError());
    }

    std::vector<std::string> devices;
    for (const auto& device : outcome.GetResult().GetDevices()) {
        devices.push_back(device.GetDeviceArn());
    }
    return devices;
}

void AmazonBraketBackend::selectDevice(const std::string& device_name) {
    auto devices = getAvailableDevices();
    if (std::find(devices.begin(), devices.end(), device_name) == devices.end()) {
        throw std::runtime_error("Device not available");
    }
    selected_device = device_name;
    updateDeviceStatus();
}

double AmazonBraketBackend::getDeviceErrorRate() const {
    if (selected_device.empty()) {
        return 0.0;
    }

    Aws::Braket::Model::GetDeviceRequest request;
    request.SetDeviceArn(selected_device);

    auto outcome = client->GetDevice(request);
    if (outcome.IsSuccess()) {
        return outcome.GetResult().GetDeviceCapabilities().GetErrorRate();
    }
    return 0.0;
}

int AmazonBraketBackend::getDeviceQueueLength() const {
    if (selected_device.empty()) {
        return 0;
    }

    Aws::Braket::Model::GetDeviceRequest request;
    request.SetDeviceArn(selected_device);

    auto outcome = client->GetDevice(request);
    if (outcome.IsSuccess()) {
        return outcome.GetResult().GetDeviceCapabilities().GetQueueLength();
    }
    return 0;
}

std::vector<std::string> AmazonBraketBackend::getAvailableTasks() const {
    Aws::Braket::Model::SearchQuantumTasksRequest request;
    auto outcome = client->SearchQuantumTasks(request);
    if (!outcome.IsSuccess()) {
        handleAwsError(outcome.GetError());
    }

    std::vector<std::string> tasks;
    for (const auto& task : outcome.GetResult().GetQuantumTasks()) {
        tasks.push_back(task.GetQuantumTaskArn());
    }
    return tasks;
}

std::string AmazonBraketBackend::getTaskStatus(const std::string& task_id) const {
    Aws::Braket::Model::GetQuantumTaskRequest request;
    request.SetQuantumTaskArn(task_id);

    auto outcome = client->GetQuantumTask(request);
    if (!outcome.IsSuccess()) {
        handleAwsError(outcome.GetError());
    }

    return outcome.GetResult().GetStatus();
}

void AmazonBraketBackend::cancelTask(const std::string& task_id) {
    Aws::Braket::Model::CancelQuantumTaskRequest request;
    request.SetQuantumTaskArn(task_id);

    auto outcome = client->CancelQuantumTask(request);
    if (!outcome.IsSuccess()) {
        handleAwsError(outcome.GetError());
    }
}

void AmazonBraketBackend::handleAwsError(const Aws::Client::AWSError<Aws::Braket::BraketErrors>& error) {
    last_error = error.GetMessage();
    throw std::runtime_error("Amazon Braket API error: " + last_error);
}

std::string AmazonBraketBackend::convertCircuitToBraketFormat(const QuantumCircuit& circuit) const {
    Aws::Utils::Json::JsonValue json;
    json.WithString("name", "qasm_circuit");
    json.WithString("language", "OPENQASM");
    json.WithString("source", circuit.toQASM());

    return json.View().WriteCompact();
}

std::vector<int> AmazonBraketBackend::parseMeasurementResults(
    const Aws::Braket::Model::GetQuantumTaskResult& result) const {
    std::vector<int> results;
    const auto& measurements = result.GetMeasurements();
    for (const auto& measurement : measurements) {
        results.push_back(measurement.GetResult());
    }
    return results;
}

void AmazonBraketBackend::updateDeviceStatus() {
    // This method would typically fetch the latest device status
    // and update internal state accordingly
}

} // namespace qasm 