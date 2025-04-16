#include "google_quantum_backend.hpp"
#include <grpcpp/create_channel.h>
#include <google/protobuf/util/json_util.h>
#include <sstream>
#include <stdexcept>

namespace qasm {

GoogleQuantumBackend::GoogleQuantumBackend() : is_initialized(false) {
    initializeGrpc();
}

GoogleQuantumBackend::~GoogleQuantumBackend() {
    cleanupGrpc();
}

void GoogleQuantumBackend::initializeGrpc() {
    auto channel = grpc::CreateChannel("quantum.googleapis.com", grpc::GoogleDefaultCredentials());
    stub = google::cloud::quantum::v1::QuantumEngineService::NewStub(channel);
    is_initialized = true;
}

void GoogleQuantumBackend::cleanupGrpc() {
    stub.reset();
    is_initialized = false;
}

void GoogleQuantumBackend::configure(const BackendConfig& config) {
    this->config = config;
    if (config.api_key.empty()) {
        throw std::runtime_error("API key is required for Google Quantum backend");
    }
}

BackendConfig GoogleQuantumBackend::getConfig() const {
    return config;
}

void GoogleQuantumBackend::execute(const QuantumCircuit& circuit, QuantumState& state) {
    if (!is_initialized) {
        throw std::runtime_error("Backend not initialized");
    }

    google::cloud::quantum::v1::RunQuantumProgramRequest request;
    request.set_name("projects/" + config.project_id + "/programs/qasm_circuit");
    request.set_processor(selected_processor);
    request.set_program(convertCircuitToGoogleFormat(circuit));

    google::cloud::quantum::v1::RunQuantumProgramResponse response;
    grpc::ClientContext context;
    context.AddMetadata("authorization", "Bearer " + config.api_key);

    grpc::Status status = stub->RunQuantumProgram(&context, request, &response);
    if (!status.ok()) {
        handleGrpcError(status);
    }
}

std::vector<int> GoogleQuantumBackend::measure(const QuantumCircuit& circuit, int shots) {
    if (!is_initialized) {
        throw std::runtime_error("Backend not initialized");
    }

    google::cloud::quantum::v1::RunQuantumProgramRequest request;
    request.set_name("projects/" + config.project_id + "/programs/qasm_circuit");
    request.set_processor(selected_processor);
    request.set_program(convertCircuitToGoogleFormat(circuit));
    request.set_shots(shots);

    google::cloud::quantum::v1::RunQuantumProgramResponse response;
    grpc::ClientContext context;
    context.AddMetadata("authorization", "Bearer " + config.api_key);

    grpc::Status status = stub->RunQuantumProgram(&context, request, &response);
    if (!status.ok()) {
        handleGrpcError(status);
    }

    return parseMeasurementResults(response);
}

int GoogleQuantumBackend::getMaxQubits() const {
    if (selected_processor.empty()) {
        return 53; // Default for Google's Sycamore processor
    }

    google::cloud::quantum::v1::GetQuantumProcessorRequest request;
    request.set_name("projects/" + config.project_id + "/processors/" + selected_processor);

    google::cloud::quantum::v1::QuantumProcessor response;
    grpc::ClientContext context;
    context.AddMetadata("authorization", "Bearer " + config.api_key);

    grpc::Status status = stub->GetQuantumProcessor(&context, request, &response);
    if (status.ok()) {
        return response.qubit_count();
    }
    return 53;
}

std::vector<std::string> GoogleQuantumBackend::getSupportedGates() const {
    return {
        "h", "x", "y", "z", "s", "t", "cx", "cz", "swap",
        "rx", "ry", "rz", "fsim", "syc", "iswap"
    };
}

bool GoogleQuantumBackend::isAvailable() const {
    if (!is_initialized) return false;
    
    google::cloud::quantum::v1::GetQuantumProcessorRequest request;
    request.set_name("projects/" + config.project_id + "/processors/" + selected_processor);

    google::cloud::quantum::v1::QuantumProcessor response;
    grpc::ClientContext context;
    context.AddMetadata("authorization", "Bearer " + config.api_key);

    grpc::Status status = stub->GetQuantumProcessor(&context, request, &response);
    return status.ok();
}

std::vector<std::string> GoogleQuantumBackend::getAvailableProcessors() const {
    google::cloud::quantum::v1::ListQuantumProcessorsRequest request;
    request.set_parent("projects/" + config.project_id);

    google::cloud::quantum::v1::ListQuantumProcessorsResponse response;
    grpc::ClientContext context;
    context.AddMetadata("authorization", "Bearer " + config.api_key);

    grpc::Status status = stub->ListQuantumProcessors(&context, request, &response);
    if (!status.ok()) {
        handleGrpcError(status);
    }

    std::vector<std::string> processors;
    for (const auto& processor : response.processors()) {
        processors.push_back(processor.name());
    }
    return processors;
}

void GoogleQuantumBackend::selectProcessor(const std::string& processor_name) {
    auto processors = getAvailableProcessors();
    if (std::find(processors.begin(), processors.end(), processor_name) == processors.end()) {
        throw std::runtime_error("Processor not available");
    }
    selected_processor = processor_name;
    updateProcessorCalibration();
}

double GoogleQuantumBackend::getProcessorFidelity() const {
    if (selected_processor.empty()) {
        return 0.0;
    }

    google::cloud::quantum::v1::GetQuantumProcessorRequest request;
    request.set_name("projects/" + config.project_id + "/processors/" + selected_processor);

    google::cloud::quantum::v1::QuantumProcessor response;
    grpc::ClientContext context;
    context.AddMetadata("authorization", "Bearer " + config.api_key);

    grpc::Status status = stub->GetQuantumProcessor(&context, request, &response);
    if (status.ok()) {
        return response.average_fidelity();
    }
    return 0.0;
}

int GoogleQuantumBackend::getProcessorQueueLength() const {
    if (selected_processor.empty()) {
        return 0;
    }

    google::cloud::quantum::v1::GetQuantumProcessorRequest request;
    request.set_name("projects/" + config.project_id + "/processors/" + selected_processor);

    google::cloud::quantum::v1::QuantumProcessor response;
    grpc::ClientContext context;
    context.AddMetadata("authorization", "Bearer " + config.api_key);

    grpc::Status status = stub->GetQuantumProcessor(&context, request, &response);
    if (status.ok()) {
        return response.queue_length();
    }
    return 0;
}

std::vector<double> GoogleQuantumBackend::getProcessorCalibrationData() const {
    if (selected_processor.empty()) {
        return {};
    }

    google::cloud::quantum::v1::GetQuantumProcessorRequest request;
    request.set_name("projects/" + config.project_id + "/processors/" + selected_processor);

    google::cloud::quantum::v1::QuantumProcessor response;
    grpc::ClientContext context;
    context.AddMetadata("authorization", "Bearer " + config.api_key);

    grpc::Status status = stub->GetQuantumProcessor(&context, request, &response);
    if (!status.ok()) {
        return {};
    }

    std::vector<double> calibration_data;
    for (const auto& calibration : response.calibration_data()) {
        calibration_data.push_back(calibration.value());
    }
    return calibration_data;
}

void GoogleQuantumBackend::handleGrpcError(const grpc::Status& status) {
    last_error = status.error_message();
    throw std::runtime_error("Google Quantum API error: " + last_error);
}

std::string GoogleQuantumBackend::convertCircuitToGoogleFormat(const QuantumCircuit& circuit) const {
    google::cloud::quantum::v1::QuantumProgram program;
    program.set_name("qasm_circuit");
    program.set_language("OPENQASM");
    program.set_source(circuit.toQASM());

    std::string output;
    google::protobuf::util::JsonPrintOptions options;
    options.add_whitespace = true;
    google::protobuf::util::MessageToJsonString(program, &output, options);
    return output;
}

std::vector<int> GoogleQuantumBackend::parseMeasurementResults(
    const google::cloud::quantum::v1::RunQuantumProgramResponse& response) const {
    std::vector<int> results;
    for (const auto& measurement : response.measurements()) {
        results.push_back(measurement.result());
    }
    return results;
}

void GoogleQuantumBackend::updateProcessorCalibration() {
    // This method would typically fetch the latest calibration data
    // and update internal state accordingly
}

} // namespace qasm 