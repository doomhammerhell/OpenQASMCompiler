# Quantum State API Reference

## Class `QuantumState`

The `QuantumState` class represents the quantum state vector of a quantum system, providing methods for state manipulation and measurement.

### Constructor

```cpp
QuantumState(int num_qubits);
QuantumState(const std::vector<std::complex<double>>& state_vector);
```

Creates a new quantum state.

**Parameters:**
- `num_qubits`: Number of qubits in the state
- `state_vector`: Initial state vector

### State Operations

#### Basic Operations

```cpp
void reset();
void normalize();
std::complex<double> amplitude(size_t index) const;
double probability(size_t index) const;
```

Basic state manipulation methods.

#### SIMD-Optimized Operations

```cpp
void simdMatrixMultiply(const ComplexMatrix& matrix);
void simdStateAdd(const QuantumState& other);
void simdStateMultiply(const QuantumState& other);
```

SIMD-accelerated state operations.

### Gate Applications

```cpp
void applySingleQubitGate(const ComplexMatrix& matrix, int qubit);
void applyTwoQubitGate(const ComplexMatrix& matrix, int qubit1, int qubit2);
void applyControlledGate(const ComplexMatrix& matrix, int control, int target);
```

Methods for applying quantum gates.

**Parameters:**
- `matrix`: Gate matrix
- `qubit`, `qubit1`, `qubit2`: Target qubits
- `control`: Control qubit

### Specific Gates

```cpp
void applyHadamard(int qubit);
void applyPauliX(int qubit);
void applyPauliY(int qubit);
void applyPauliZ(int qubit);
void applyCNOT(int control, int target);
void applySWAP(int qubit1, int qubit2);
void applyPhase(int qubit, double phase);
void applyRotation(int qubit, double theta, double phi);
```

Optimized implementations of common gates.

### Measurement

```cpp
int measure();
int measureQubit(int qubit);
std::vector<int> measureAll();
std::map<int, double> getProbabilities() const;
```

Methods for quantum measurement.

### State Analysis

```cpp
double fidelity(const QuantumState& other) const;
double entropy() const;
double entanglement(int qubit1, int qubit2) const;
std::vector<double> blochVector(int qubit) const;
```

Methods for analyzing the quantum state.

### State Caching

```cpp
void cacheState(const std::string& key);
void restoreState(const std::string& key);
void clearCache();
bool hasCache(const std::string& key) const;
```

Methods for state caching and restoration.

### State Information

```cpp
int numQubits() const;
size_t dimension() const;
std::vector<std::complex<double>> getVector() const;
std::string toString() const;
```

Methods for querying state properties.

### Example Usage

```cpp
// Create a 2-qubit state
QuantumState state(2);

// Apply gates
state.applyHadamard(0);
state.applyCNOT(0, 1);

// Analyze state
auto probs = state.getProbabilities();
double ent = state.entanglement(0, 1);

// Cache and restore
state.cacheState("bell_state");
state.reset();
state.restoreState("bell_state");

// Measure
int result = state.measure();
```

### Error Handling

The following exceptions may be thrown:

- `StateError`: General state operation errors
- `InvalidQubitError`: Invalid qubit indices
- `NormalizationError`: State normalization errors
- `CacheError`: State caching/restoration errors

### Thread Safety

The `QuantumState` class is not thread-safe. Multiple threads should not modify the same state instance simultaneously.

### Performance Considerations

1. State Vector Operations
   - O(2ⁿ) space complexity
   - O(2ⁿ) time complexity for general operations
   - O(2ⁿ/k) time complexity for SIMD operations (k depends on SIMD width)

2. Gate Applications
   - O(2ⁿ) for general gates
   - O(2ⁿ⁻¹) for single-qubit gates
   - O(2ⁿ⁻²) for two-qubit gates with SIMD

3. Measurements
   - O(2ⁿ) for full state measurement
   - O(2ⁿ⁻¹) for single qubit measurement

### Memory Management

```cpp
class QuantumState {
private:
    // Aligned allocation for SIMD
    alignas(32) std::vector<double> state_vector_real;
    alignas(32) std::vector<double> state_vector_imag;
    
    // Cache management
    std::unordered_map<std::string, StateVector> state_cache;
    size_t max_cache_size = 1000;
};
```

### Best Practices

1. State Creation and Management
   ```cpp
   // Good: Clear state lifecycle
   QuantumState state(3);
   state.applyHadamard(0);
   int result = state.measure();
   state.reset();
   
   // Bad: Unnecessary state copies
   std::vector<QuantumState> states;
   for (int i = 0; i < 1000; i++) {
       states.push_back(state);  // Expensive copy
   }
   ```

2. SIMD Usage
   ```cpp
   // Good: Use SIMD operations when possible
   state.simdMatrixMultiply(matrix);
   
   // Bad: Manual loop implementation
   for (size_t i = 0; i < state.dimension(); i++) {
       // Manual matrix multiplication
   }
   ```

3. Cache Management
   ```cpp
   // Good: Clear cache when no longer needed
   state.cacheState("important_state");
   // ... use state ...
   state.clearCache();
   
   // Bad: Unbounded cache growth
   for (int i = 0; i < 1000000; i++) {
       state.cacheState("state_" + std::to_string(i));
   }
   ```

### Related Classes

- [`QuantumCircuit`](circuit.md): Quantum circuit representation
- [`QuantumBackend`](backend.md): Backend interface
- [`QuantumDebugger`](debugger.md): Debugging tools 