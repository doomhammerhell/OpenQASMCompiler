# QuantumState Class

The `QuantumState` class represents the state of a quantum system and provides methods for manipulating it.

## Features

- SIMD-optimized matrix multiplication
- Support for single-qubit gates (X, Y, Z, H, S, T)
- Support for two-qubit gates (CNOT, SWAP, CZ)
- Support for three-qubit gates (Toffoli, Fredkin, CCNOT, CSWAP)
- State caching for efficient state management
- Measurement and normalization operations
- Quantum noise simulation with various noise models

## Three-Qubit Gates

### Toffoli Gate (CCNOT)
The Toffoli gate is a three-qubit gate that flips the target qubit if both control qubits are in state |1⟩.

```cpp
void applyToffoli(int control1, int control2, int target);
```

### Fredkin Gate (CSWAP)
The Fredkin gate is a three-qubit gate that swaps the target qubits if the control qubit is in state |1⟩.

```cpp
void applyFredkin(int control, int target1, int target2);
```

### CCNOT Gate
Alias for the Toffoli gate.

```cpp
void applyCCNOT(int control1, int control2, int target);
```

### CSWAP Gate
Alias for the Fredkin gate.

```cpp
void applyCSWAP(int control, int target1, int target2);
```

## State Caching

The `QuantumState` class provides methods for caching and restoring quantum states.

### Saving a State
```cpp
void saveState(const std::string& name);
```
Saves the current state with the given name.

### Loading a State
```cpp
void loadState(const std::string& name);
```
Loads a previously saved state. Throws `std::runtime_error` if the state doesn't exist.

### Clearing Cache
```cpp
void clearCache();
```
Clears all saved states from the cache.

## Quantum Noise

The `QuantumState` class supports various types of quantum noise through the `QuantumNoise` class.

### Noise Types
- **Depolarizing Noise**: Random Pauli errors (X, Y, Z) with equal probability
- **Amplitude Damping**: Energy dissipation to the environment
- **Phase Damping**: Loss of quantum coherence
- **Bit Flip**: Random X errors
- **Phase Flip**: Random Z errors
- **Bit-Phase Flip**: Random Y errors

### Enabling Noise
```cpp
void enableNoise(double error_rate = 0.01);
```
Enables quantum noise with the specified error rate.

### Disabling Noise
```cpp
void disableNoise();
```
Disables quantum noise.

### Setting Noise Type
```cpp
void setNoiseType(QuantumNoise::NoiseType type);
```
Sets the type of quantum noise to apply.

### Custom Noise
```cpp
void setCustomNoise(const std::vector<std::vector<std::complex<double>>>& noise_matrix);
```
Sets a custom noise matrix for specialized noise models.

## Example Usage

```cpp
QuantumState state(3);

// Enable noise
state.enableNoise(0.01); // 1% error rate
state.setNoiseType(QuantumNoise::NoiseType::DEPOLARIZING);

// Create an entangled state
state.applyHadamard(0);
state.applyCNOT(0, 1);
state.applyToffoli(0, 1, 2);

// Save the state
state.saveState("entangled");

// Modify the state
state.applyX(0);
state.applyX(1);
state.applyX(2);

// Restore the original state
state.loadState("entangled");
```

## Performance Considerations

- The implementation uses SIMD instructions for matrix multiplication
- State caching can improve performance when the same state needs to be restored multiple times
- Three-qubit gates are implemented efficiently using tensor product operations
- Noise simulation is optimized using Kraus operators and efficient matrix multiplication 