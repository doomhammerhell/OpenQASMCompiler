# Quantum Noise Model API Reference

## Class `NoiseModel`

The `NoiseModel` class represents a quantum noise model that can be applied to quantum circuits and states, supporting various types of quantum noise and error channels.

### Noise Types

```cpp
enum class NoiseType {
    DEPOLARIZING,    // Depolarizing noise
    AMPLITUDE_DAMPING, // Amplitude damping
    PHASE_DAMPING,   // Phase damping
    BIT_FLIP,        // Bit flip
    PHASE_FLIP,      // Phase flip
    BIT_PHASE_FLIP,  // Combined bit and phase flip
    PAULI_CHANNEL,   // General Pauli channel
    KRAUS_CHANNEL    // General Kraus channel
};
```

### Noise Parameters

```cpp
struct NoiseParameters {
    double probability = 0.0;
    double damping_rate = 0.0;
    double dephasing_rate = 0.0;
    std::vector<double> pauli_probabilities;
    std::vector<ComplexMatrix> kraus_operators;
    std::map<std::string, double> custom_parameters;
};
```

### Constructor

```cpp
NoiseModel();
NoiseModel(const std::vector<NoiseType>& types,
           const std::vector<NoiseParameters>& parameters);
```

Create a new noise model.

### Noise Configuration

```cpp
void addNoise(NoiseType type, const NoiseParameters& parameters);
void setNoise(NoiseType type, const NoiseParameters& parameters);
void removeNoise(NoiseType type);
void clearNoise();
```

Methods for configuring noise channels.

### Noise Application

```cpp
void applyNoise(QuantumState& state, int qubit) const;
void applyNoise(QuantumCircuit& circuit) const;
void applyNoise(QuantumGate& gate) const;
```

Methods for applying noise to quantum objects.

### Noise Characterization

```cpp
NoiseModel characterizeNoise(const QuantumBackend& backend,
                           int shots = 1000) const;
std::map<std::string, double> getNoiseMetrics() const;
```

Methods for characterizing and analyzing noise.

### Example Usage

```cpp
// Create noise model
NoiseModel noise_model;

// Add depolarizing noise
NoiseParameters depol_params;
depol_params.probability = 0.01;
noise_model.addNoise(NoiseType::DEPOLARIZING, depol_params);

// Add amplitude damping
NoiseParameters amp_damp_params;
amp_damp_params.damping_rate = 0.05;
noise_model.addNoise(NoiseType::AMPLITUDE_DAMPING, amp_damp_params);

// Apply noise to circuit
QuantumCircuit circuit(2);
circuit.addGate(GateType::H, {0});
circuit.addGate(GateType::CNOT, {0, 1});
noise_model.applyNoise(circuit);

// Characterize noise
auto metrics = noise_model.getNoiseMetrics();
```

### Error Handling

The following exceptions may be thrown:

- `NoiseError`: General noise model errors
- `InvalidNoiseError`: Invalid noise parameters
- `CharacterizationError`: Noise characterization errors

### Thread Safety

The `NoiseModel` class is thread-safe for concurrent noise application.

### Performance Considerations

1. Noise Application
   - O(1) for single-qubit noise
   - O(n) for n-qubit noise
   - O(k) for k Kraus operators

2. Noise Characterization
   - O(s × n) for s shots and n qubits
   - O(m) for m metrics

### Best Practices

1. Noise Configuration
   ```cpp
   // Good: Clear noise model structure
   NoiseModel noise_model;
   noise_model.addNoise(NoiseType::DEPOLARIZING, {0.01});
   noise_model.addNoise(NoiseType::AMPLITUDE_DAMPING, {0.05});
   
   // Bad: Overlapping noise channels
   noise_model.addNoise(NoiseType::DEPOLARIZING, {0.5});
   noise_model.addNoise(NoiseType::BIT_FLIP, {0.5});  // Too much noise
   ```

2. Noise Characterization
   ```cpp
   // Good: Characterize noise before use
   auto noise_model = NoiseModel::characterizeNoise(backend, 1000);
   circuit.setNoiseModel(noise_model);
   
   // Bad: Use uncharacterized noise
   circuit.setNoiseModel(NoiseModel());  // Default noise may not match device
   ```

3. Resource Management
   ```cpp
   // Good: Clear noise when no longer needed
   circuit.setNoiseModel(NoiseModel());  // Reset to no noise
   
   // Bad: Keep unnecessary noise
   noise_model.addNoise(NoiseType::DEPOLARIZING, {0.01});
   // ... use noise_model ...
   noise_model.addNoise(NoiseType::DEPOLARIZING, {0.01});  // Duplicate
   ```

### Custom Noise Channels

```cpp
class CustomNoiseChannel : public NoiseChannel {
public:
    void apply(QuantumState& state, int qubit) override {
        // Implement custom noise
    }
    
    NoiseParameters getParameters() const override {
        return parameters_;
    }
    
private:
    NoiseParameters parameters_;
};

// Register custom noise
NoiseModel::registerNoiseChannel("custom", std::make_unique<CustomNoiseChannel>());
```

### Related Classes

- [`QuantumCircuit`](circuit.md): Circuit representation
- [`QuantumState`](state.md): State representation
- [`QuantumBackend`](backend.md): Backend interface
- [`ErrorMitigation`](error_mitigation.md): Error mitigation techniques 