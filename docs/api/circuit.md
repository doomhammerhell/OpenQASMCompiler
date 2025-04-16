# Quantum Circuit API Reference

## Class `QuantumCircuit`

The `QuantumCircuit` class represents a quantum circuit, consisting of quantum gates and measurements.

### Constructor

```cpp
QuantumCircuit(int num_qubits);
```

Creates a new quantum circuit with the specified number of qubits.

**Parameters:**
- `num_qubits`: Number of qubits in the circuit

### Gate Operations

#### Adding Gates

```cpp
void addGate(GateType type, const std::vector<int>& qubits);
void addGate(GateType type, const std::vector<int>& qubits, double parameter);
void addCustomGate(const std::string& name, const ComplexMatrix& matrix, const std::vector<int>& qubits);
```

Add a quantum gate to the circuit.

**Parameters:**
- `type`: Type of the gate (H, X, Y, Z, CNOT, etc.)
- `qubits`: Indices of qubits the gate acts on
- `parameter`: Optional parameter for parameterized gates
- `name`: Name of the custom gate
- `matrix`: Unitary matrix representing the custom gate

#### Gate Types

```cpp
enum class GateType {
    H,      // Hadamard gate
    X,      // Pauli-X gate
    Y,      // Pauli-Y gate
    Z,      // Pauli-Z gate
    CNOT,   // Controlled-NOT gate
    SWAP,   // SWAP gate
    T,      // T gate
    Tdg,    // T-dagger gate
    S,      // S gate
    Sdg,    // S-dagger gate
    RX,     // Rotation around X-axis
    RY,     // Rotation around Y-axis
    RZ,     // Rotation around Z-axis
    U1,     // U1 gate
    U2,     // U2 gate
    U3,     // U3 gate
    CZ,     // Controlled-Z gate
    CCX,    // Toffoli gate (CCX)
    CSWAP   // Fredkin gate (CSWAP)
};
```

### Measurements

```cpp
void addMeasurement(const std::vector<int>& qubits);
void addMeasurement(int qubit);
```

Add measurement operations to the circuit.

**Parameters:**
- `qubits`: Indices of qubits to measure
- `qubit`: Single qubit to measure

### Circuit Manipulation

```cpp
void clear();
void inverse();
void append(const QuantumCircuit& other);
QuantumCircuit slice(size_t start, size_t end) const;
```

Methods for manipulating the circuit structure.

**Parameters:**
- `other`: Another quantum circuit to append
- `start`: Starting gate index for slice
- `end`: Ending gate index for slice

### Circuit Properties

```cpp
int numQubits() const;
size_t numGates() const;
std::vector<Gate> gates() const;
double depth() const;
```

Methods for querying circuit properties.

### Circuit Optimization

```cpp
void optimize();
void setOptimizationLevel(int level);
```

Methods for optimizing the circuit.

**Parameters:**
- `level`: Optimization level (0-3)

### Classical Control

```cpp
void addClassicalControl(GateType type, const std::vector<int>& qubits, const std::vector<int>& conditions);
```

Add classically controlled operations.

**Parameters:**
- `type`: Gate type to apply
- `qubits`: Target qubits
- `conditions`: Classical conditions

### Circuit Conversion

```cpp
std::string toQASM() const;
std::string toLatex() const;
std::string toASCII() const;
```

Methods for converting the circuit to different representations.

### Error Mitigation

```cpp
void setErrorMitigation(bool enable);
void setNoiseModel(const NoiseModel& model);
```

Configure error mitigation settings.

**Parameters:**
- `enable`: Enable/disable error mitigation
- `model`: Noise model for simulation

### Example Usage

```cpp
// Create a 2-qubit circuit
QuantumCircuit circuit(2);

// Add gates to create a Bell state
circuit.addGate(GateType::H, {0});
circuit.addGate(GateType::CNOT, {0, 1});

// Add measurements
circuit.addMeasurement({0, 1});

// Optimize the circuit
circuit.setOptimizationLevel(2);
circuit.optimize();

// Convert to QASM
std::string qasm = circuit.toQASM();
```

### Error Handling

The following exceptions may be thrown:

- `CircuitError`: General circuit operation errors
- `InvalidQubitError`: Invalid qubit indices
- `InvalidGateError`: Invalid gate operations
- `OptimizationError`: Circuit optimization errors

### Thread Safety

The `QuantumCircuit` class is not thread-safe. Multiple threads should not modify the same circuit instance simultaneously.

### Performance Considerations

1. Gate Addition
   - O(1) time complexity
   - Memory grows linearly with number of gates

2. Circuit Optimization
   - O(n log n) time complexity for most optimizations
   - O(n²) for some advanced optimizations

3. Circuit Conversion
   - O(n) time complexity for QASM and ASCII
   - O(n log n) for LaTeX with optimization

### Best Practices

1. Circuit Creation
   ```cpp
   // Good: Clear circuit structure
   QuantumCircuit circuit(3);
   circuit.addGate(GateType::H, {0});
   circuit.addGate(GateType::CNOT, {0, 1});
   circuit.addGate(GateType::CNOT, {1, 2});
   
   // Bad: Unnecessary gates
   circuit.addGate(GateType::X, {0});
   circuit.addGate(GateType::X, {0});  // Cancels out
   ```

2. Optimization
   ```cpp
   // Good: Optimize once after building
   circuit.setOptimizationLevel(2);
   circuit.optimize();
   
   // Bad: Optimize after every gate
   for (const auto& gate : gates) {
       circuit.addGate(gate.type, gate.qubits);
       circuit.optimize();  // Unnecessary
   }
   ```

3. Resource Management
   ```cpp
   // Good: Clear unused circuits
   {
       QuantumCircuit temp(10);
       // ... use temp ...
   }  // temp is destroyed
   
   // Bad: Keep large circuits in memory
   std::vector<QuantumCircuit> all_circuits;
   for (int i = 0; i < 1000; i++) {
       all_circuits.push_back(QuantumCircuit(100));
   }
   ```

### Related Classes

- [`QuantumState`](state.md): Quantum state representation
- [`QuantumBackend`](backend.md): Backend interface
- [`QuantumDebugger`](debugger.md): Circuit debugging tools 