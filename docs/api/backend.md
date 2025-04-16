# Backend API Reference

## Overview

The Backend API provides a unified interface for executing quantum circuits on different quantum computing platforms, including local simulators and cloud-based quantum computers.

## Core Classes

### QuantumBackend

Base class for all quantum computation backends.

```cpp
class QuantumBackend {
public:
    virtual ~QuantumBackend() = default;
    
    // Configuration
    virtual void configure(const BackendConfig& config) = 0;
    virtual BackendConfig getConfig() const = 0;
    
    // Execution
    virtual void execute(const QuantumCircuit& circuit, QuantumState& state) = 0;
    virtual std::vector<int> measure(const QuantumCircuit& circuit, int shots = 1024) = 0;
    
    // Information
    virtual BackendType getType() const = 0;
    virtual std::string getName() const = 0;
    virtual int getMaxQubits() const = 0;
    virtual std::vector<std::string> getSupportedGates() const = 0;
    
    // Error handling
    virtual std::string getLastError() const = 0;
    virtual bool isAvailable() const = 0;
};
```

### BackendConfig

Configuration structure for quantum backends.

```cpp
struct BackendConfig {
    int max_qubits = 0;
    int shots = 1000;
    bool error_mitigation = false;
    std::vector<MitigationMethod> mitigation_methods;
    std::string api_key;
    std::string device;
    std::string region;
    NoiseModel noise_model;
    std::map<std::string, std::string> custom_parameters;
};
```

## Implementations

### LocalBackend
Local quantum circuit simulator.

### IBMQuantumBackend
Interface to IBM Quantum Experience.
- Device selection
- Job management
- Error rate queries
- Queue monitoring

### GoogleQuantumBackend
Interface to Google Quantum Computing Service.
- Processor selection
- Job scheduling
- Calibration data
- Resource estimation

### AmazonBraketBackend
Interface to Amazon Braket service.
- Multiple hardware providers
- Task management
- Device selection
- Cost estimation

## Usage Examples

### Basic Usage

```cpp
// Create and configure backend
auto backend = QuantumBackendFactory::createBackend("local");
BackendConfig config;
config.max_qubits = 2;
backend->configure(config);

// Execute circuit
QuantumCircuit circuit(2);
circuit.addGate(GateType::H, {0});
circuit.addGate(GateType::CNOT, {0, 1});

QuantumState state(2);
backend->execute(circuit, state);

// Measure results
auto results = backend->measure(circuit, 1000);
```

### Cloud Backend Usage

```cpp
// Configure IBM Quantum backend
auto backend = QuantumBackendFactory::createBackend("ibm");
BackendConfig config;
config.api_key = getenv("IBM_QUANTUM_API_KEY");
config.device = "ibmq_manila";
config.shots = 1000;
backend->configure(config);

// Execute and get results
auto results = backend->measure(circuit, 1000);
```

## Error Handling

```cpp
try {
    backend->execute(circuit, state);
} catch (const BackendError& e) {
    std::cerr << "Backend error: " << e.what() << std::endl;
    std::cerr << "Details: " << backend->getLastError() << std::endl;
}
```

## Best Practices

1. Backend Selection
   - Use local backend for development and testing
   - Use cloud backends for production runs
   - Consider queue times and costs

2. Error Mitigation
   - Enable error mitigation for noisy devices
   - Use appropriate mitigation methods
   - Validate results with different methods

3. Resource Management
   - Check backend availability before submission
   - Monitor job status and queue position
   - Handle connection failures gracefully

4. Performance Optimization
   - Batch similar circuits together
   - Reuse backend instances when possible
   - Cache results for repeated measurements

## See Also

- [Circuit API](circuit.md)
- [State API](state.md)
- [Noise API](noise.md)
- [Cloud Integration Guide](../guides/cloud-integration.md) 