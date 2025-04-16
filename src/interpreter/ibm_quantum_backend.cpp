#include "ibm_quantum_backend.hpp"
#include <json/json.h>
#include <sstream>
#include <stdexcept>

namespace qasm {

IBMQuantumBackend::IBMQuantumBackend() : curl_handle(nullptr), is_initialized(false) {
    initializeCurl();
}

IBMQuantumBackend::~IBMQuantumBackend() {
    cleanupCurl();
}

void IBMQuantumBackend::initializeCurl() {
    curl_handle = curl_easy_init();
    if (!curl_handle) {
        throw std::runtime_error("Failed to initialize CURL");
    }
    is_initialized = true;
}

void IBMQuantumBackend::cleanupCurl() {
    if (curl_handle) {
        curl_easy_cleanup(curl_handle);
        curl_handle = nullptr;
    }
    is_initialized = false;
}

void IBMQuantumBackend::configure(const BackendConfig& config) {
    this->config = config;
    if (config.api_key.empty()) {
        throw std::runtime_error("API key is required for IBM Quantum backend");
    }
}

BackendConfig IBMQuantumBackend::getConfig() const {
    return config;
}

void IBMQuantumBackend::execute(const QuantumCircuit& circuit, QuantumState& state) {
    if (!is_initialized) {
        throw std::runtime_error("Backend not initialized");
    }

    std::string circuit_json = convertCircuitToIBMFormat(circuit);
    std::string response = makeApiRequest("/jobs", "POST", circuit_json);
    
    // Parse response and update state
    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(response, root)) {
        throw std::runtime_error("Failed to parse IBM Quantum response");
    }
}

std::vector<int> IBMQuantumBackend::measure(const QuantumCircuit& circuit, int shots) {
    if (!is_initialized) {
        throw std::runtime_error("Backend not initialized");
    }

    std::string circuit_json = convertCircuitToIBMFormat(circuit);
    std::stringstream ss;
    ss << "/jobs?shots=" << shots;
    std::string response = makeApiRequest(ss.str(), "POST", circuit_json);
    
    return parseMeasurementResults(response);
}

int IBMQuantumBackend::getMaxQubits() const {
    if (selected_device.empty()) {
        return 5; // Default for IBM Quantum devices
    }
    // Query device capabilities
    std::string response = makeApiRequest("/devices/" + selected_device);
    Json::Value root;
    Json::Reader reader;
    if (reader.parse(response, root)) {
        return root["n_qubits"].asInt();
    }
    return 5;
}

std::vector<std::string> IBMQuantumBackend::getSupportedGates() const {
    return {
        "h", "x", "y", "z", "s", "sdg", "t", "tdg",
        "cx", "cz", "swap", "ccx", "cswap",
        "rx", "ry", "rz", "u1", "u2", "u3"
    };
}

bool IBMQuantumBackend::isAvailable() const {
    if (!is_initialized) return false;
    
    try {
        std::string response = makeApiRequest("/version");
        return !response.empty();
    } catch (...) {
        return false;
    }
}

std::vector<std::string> IBMQuantumBackend::getAvailableDevices() const {
    std::string response = makeApiRequest("/devices");
    Json::Value root;
    Json::Reader reader;
    std::vector<std::string> devices;
    
    if (reader.parse(response, root)) {
        for (const auto& device : root) {
            devices.push_back(device["name"].asString());
        }
    }
    
    return devices;
}

void IBMQuantumBackend::selectDevice(const std::string& device_name) {
    auto devices = getAvailableDevices();
    if (std::find(devices.begin(), devices.end(), device_name) == devices.end()) {
        throw std::runtime_error("Device not available");
    }
    selected_device = device_name;
}

double IBMQuantumBackend::getDeviceErrorRate() const {
    if (selected_device.empty()) {
        return 0.0;
    }
    
    std::string response = makeApiRequest("/devices/" + selected_device);
    Json::Value root;
    Json::Reader reader;
    
    if (reader.parse(response, root)) {
        return root["error_rate"].asDouble();
    }
    
    return 0.0;
}

int IBMQuantumBackend::getDeviceQueueLength() const {
    if (selected_device.empty()) {
        return 0;
    }
    
    std::string response = makeApiRequest("/devices/" + selected_device + "/queue");
    Json::Value root;
    Json::Reader reader;
    
    if (reader.parse(response, root)) {
        return root["length"].asInt();
    }
    
    return 0;
}

std::string IBMQuantumBackend::makeApiRequest(const std::string& endpoint, const std::string& method, const std::string& data) {
    if (!curl_handle) {
        throw std::runtime_error("CURL not initialized");
    }

    std::string url = "https://api.ibm.com/quantum/v1" + endpoint;
    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, method.c_str());
    
    if (!data.empty()) {
        curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, data.c_str());
    }
    
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("Authorization: Bearer " + config.api_key).c_str());
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
    
    std::string response;
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, [](void* contents, size_t size, size_t nmemb, std::string* userp) {
        userp->append((char*)contents, size * nmemb);
        return size * nmemb;
    });
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &response);
    
    CURLcode res = curl_easy_perform(curl_handle);
    curl_slist_free_all(headers);
    
    if (res != CURLE_OK) {
        handleApiError(curl_easy_strerror(res));
    }
    
    return response;
}

void IBMQuantumBackend::handleApiError(const std::string& error) {
    last_error = error;
    throw std::runtime_error("IBM Quantum API error: " + error);
}

std::string IBMQuantumBackend::convertCircuitToIBMFormat(const QuantumCircuit& circuit) const {
    Json::Value root;
    root["name"] = "qasm_circuit";
    root["shots"] = 1024;
    
    Json::Value qasm;
    qasm["qasm"] = circuit.toQASM();
    root["qasm"] = qasm;
    
    Json::Value config;
    config["backend"] = Json::Value(selected_device);
    root["config"] = config;
    
    Json::FastWriter writer;
    return writer.write(root);
}

std::vector<int> IBMQuantumBackend::parseMeasurementResults(const std::string& response) const {
    Json::Value root;
    Json::Reader reader;
    std::vector<int> results;
    
    if (reader.parse(response, root)) {
        const Json::Value& counts = root["results"][0]["data"]["counts"];
        for (const auto& key : counts.getMemberNames()) {
            int value = std::stoi(key, nullptr, 2);
            int count = counts[key].asInt();
            for (int i = 0; i < count; ++i) {
                results.push_back(value);
            }
        }
    }
    
    return results;
}

} // namespace qasm 