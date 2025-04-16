# Cloud Backend Integration

This guide explains how to use cloud-based quantum computing backends with OpenQASM Compiler.

## Supported Cloud Providers

### IBM Quantum

IBM Quantum provides access to real quantum computers and simulators through IBM Quantum Experience.

#### Configuration

```cpp
auto backend = QuantumBackendFactory::createBackend("ibm");
QuantumBackend::BackendConfig config;
config.api_key = "your-ibm-api-key";
config.device = "ibmq_manila";  // or other available devices
config.shots = 1000;
backend->configure(config);
```

#### Available Devices
- `ibmq_manila`: 5-qubit quantum processor
- `ibmq_quito`: 5-qubit quantum processor
- `ibmq_lima`: 5-qubit quantum processor
- `ibm_nairobi`: 7-qubit quantum processor
- `ibm_oslo`: 7-qubit quantum processor
- `ibm_cairo`: 27-qubit quantum processor
- `ibm_hanoi`: 27-qubit quantum processor
- `ibm_auckland`: 27-qubit quantum processor

### Google Quantum

Access to Google's Quantum Computing Service through Cloud API.

#### Configuration

```cpp
auto backend = QuantumBackendFactory::createBackend("google");
QuantumBackend::BackendConfig config;
config.api_key = "your-google-api-key";
config.project_id = "your-project-id";
config.processor = "weber";  // or other available processors
config.shots = 1000;
backend->configure(config);
```

#### Available Processors
- `weber`: 53-qubit quantum processor
- `rainbow`: 23-qubit quantum processor
- `simulator`: High-performance quantum simulator

### Amazon Braket

Amazon's Quantum Computing Service providing access to multiple quantum hardware providers.

#### Configuration

```cpp
auto backend = QuantumBackendFactory::createBackend("braket");
QuantumBackend::BackendConfig config;
config.api_key = "your-aws-api-key";
config.region = "us-west-1";
config.device = "SV1";  // or other available devices
config.shots = 1000;
backend->configure(config);
```

#### Available Devices
- `SV1`: State vector simulator
- `TN1`: Tensor network simulator
- `Rigetti`: Rigetti quantum processors
- `IonQ`: IonQ quantum processors
- `OQC`: Oxford Quantum Circuits processors

## Usage Examples

### Basic Circuit Execution

```cpp
// Create and configure backend
auto backend = QuantumBackendFactory::createBackend("ibm");
QuantumBackend::BackendConfig config;
config.api_key = getenv("IBM_QUANTUM_API_KEY");
config.device = "ibmq_manila";
config.shots = 1000;
backend->configure(config);

// Create circuit
QuantumCircuit circuit(2);
circuit.addGate(QuantumCircuit::GateType::H, {0});
circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 1});

// Execute circuit
QuantumState state(2);
backend->execute(circuit, state);

// Get results
auto results = backend->measure(circuit, 1000);
```

### Error Mitigation

```cpp
// Enable error mitigation
QuantumBackend::BackendConfig config;
config.api_key = getenv("IBM_QUANTUM_API_KEY");
config.device = "ibmq_manila";
config.error_mitigation = true;
config.shots = 1000;

auto backend = QuantumBackendFactory::createBackend("ibm");
backend->configure(config);

// Execute with error mitigation
QuantumCircuit circuit(2);
circuit.addGate(QuantumCircuit::GateType::H, {0});
circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 1});

QuantumState state(2);
backend->execute(circuit, state);
```

### Batch Execution

```cpp
// Create multiple circuits
std::vector<QuantumCircuit> circuits;
for (int i = 0; i < 10; i++) {
    QuantumCircuit circuit(2);
    circuit.addGate(QuantumCircuit::GateType::H, {0});
    circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 1});
    circuits.push_back(circuit);
}

// Execute batch
auto backend = QuantumBackendFactory::createBackend("ibm");
backend->configure(config);
auto results = backend->executeBatch(circuits, 1000);
```

### Device Querying

```cpp
// Get available devices
auto backend = QuantumBackendFactory::createBackend("ibm");
backend->configure(config);
auto devices = backend->getAvailableDevices();

// Get device properties
auto properties = backend->getDeviceProperties("ibmq_manila");
```

## Best Practices

1. **API Key Management**
   - Store API keys securely
   - Use environment variables
   - Never hardcode keys in source code

2. **Error Handling**
   ```cpp
   try {
       backend->execute(circuit, state);
   } catch (const BackendError& e) {
       std::cerr << "Backend error: " << e.what() << std::endl;
   } catch (const std::exception& e) {
       std::cerr << "Error: " << e.what() << std::endl;
   }
   ```

3. **Resource Management**
   - Check queue times before submission
   - Use batch execution for multiple circuits
   - Monitor costs for paid services

4. **Circuit Optimization**
   - Optimize circuits before sending to cloud
   - Use local simulator for testing
   - Validate results with small shot counts

## Troubleshooting

### Common Issues

1. **Authentication Failures**
   - Verify API key is correct
   - Check key permissions
   - Ensure key is not expired

2. **Execution Failures**
   - Check circuit complexity
   - Verify qubit connectivity
   - Monitor backend status

3. **Performance Issues**
   - Use appropriate shot count
   - Consider local simulation
   - Check network connectivity

### Getting Help

- Check provider documentation
- Use provider support channels
- Report issues on GitHub

## Additional Resources

- [IBM Quantum Documentation](https://quantum-computing.ibm.com/docs/)
- [Google Quantum Documentation](https://quantumai.google/cirq)
- [Amazon Braket Documentation](https://docs.aws.amazon.com/braket/)
- [OpenQASM Compiler Examples](examples.md)
- [Performance Optimization Guide](optimization.md) 