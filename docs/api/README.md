# API Documentation

## Overview

This document provides detailed API documentation for the OpenQASM Compiler. The API is organized into several main components:

1. Parser
2. AST (Abstract Syntax Tree)
3. IR (Intermediate Representation)
4. Optimizer
5. Quantum Circuit
6. Simulator

## Parser API

### `Parser` Class

```cpp
class Parser {
public:
    // Constructors
    Parser();
    explicit Parser(const std::string& filename);
    
    // Main parsing methods
    ASTNode* parse(const std::string& input);
    ASTNode* parseFile(const std::string& filename);
    
    // Error handling
    std::vector<Error> getErrors() const;
    bool hasErrors() const;
};
```

### Key Methods

- `parse(const std::string& input)`: Parses OpenQASM code from a string
- `parseFile(const std::string& filename)`: Parses OpenQASM code from a file
- `getErrors()`: Returns a list of parsing errors
- `hasErrors()`: Checks if any errors occurred during parsing

## AST API

### `ASTNode` Class

```cpp
class ASTNode {
public:
    // Node type enumeration
    enum class Type {
        Module,
        Include,
        GateDeclaration,
        GateCall,
        QubitDeclaration,
        ClassicalDeclaration,
        // ... other types
    };
    
    // Constructors
    ASTNode(Type type);
    ASTNode(Type type, const std::string& value);
    
    // Node manipulation
    void addChild(std::unique_ptr<ASTNode> child);
    const std::vector<std::unique_ptr<ASTNode>>& getChildren() const;
    
    // Node properties
    Type getType() const;
    std::string getValue() const;
    void setValue(const std::string& value);
};
```

### Key Methods

- `addChild()`: Adds a child node
- `getChildren()`: Returns all child nodes
- `getType()`: Returns the node type
- `getValue()`: Returns the node value

## IR API

### `IRBuilder` Class

```cpp
class IRBuilder {
public:
    // Construction
    IRBuilder(ASTNode* root);
    
    // IR generation
    std::unique_ptr<IRModule> build();
    
    // Optimization
    void optimize();
};
```

### Key Methods

- `build()`: Builds IR from AST
- `optimize()`: Applies optimizations to IR

## Quantum Circuit API

### `QuantumCircuit` Class

```cpp
class QuantumCircuit {
public:
    // Construction
    explicit QuantumCircuit(size_t num_qubits);
    
    // Gate operations
    void addGate(std::unique_ptr<QuantumGate> gate);
    void apply();
    
    // Measurement
    std::vector<bool> measure();
    std::vector<double> getProbabilities() const;
    
    // Circuit properties
    size_t getNumQubits() const;
    size_t getDepth() const;
    size_t getWidth() const;
};
```

### Key Methods

- `addGate()`: Adds a quantum gate to the circuit
- `apply()`: Applies all gates in the circuit
- `measure()`: Performs measurement on all qubits
- `getProbabilities()`: Returns probability distribution

## Simulator API

### `QuantumSimulator` Class

```cpp
class QuantumSimulator {
public:
    // Construction
    explicit QuantumSimulator(size_t num_qubits);
    
    // Simulation
    void simulate(const QuantumCircuit& circuit);
    void simulate(const QuantumCircuit& circuit, size_t shots);
    
    // State access
    std::vector<std::complex<double>> getState() const;
    double getExpectationValue(const Observable& observable) const;
    
    // Noise models
    void setNoiseModel(NoiseModel model, double parameter);
};
```

### Key Methods

- `simulate()`: Simulates a quantum circuit
- `getState()`: Returns the current quantum state
- `getExpectationValue()`: Calculates expectation value
- `setNoiseModel()`: Configures noise model

## Performance Considerations

1. **Memory Usage**
   - State vector size grows exponentially with qubit count
   - Use sparse representations for large circuits
   - Consider using GPU acceleration for large simulations

2. **Computation Time**
   - Gate application complexity: O(2^n) for n qubits
   - Measurement complexity: O(2^n)
   - Optimization can reduce circuit depth

3. **Optimization Tips**
   - Use circuit optimization before simulation
   - Consider approximate methods for large circuits
   - Use appropriate noise models for realistic simulation

## Example Usage

```cpp
#include "OpenQASMCompiler/parser.hpp"
#include "OpenQASMCompiler/quantum_circuit.hpp"
#include "OpenQASMCompiler/quantum_simulator.hpp"

int main() {
    // Parse OpenQASM code
    Parser parser;
    auto ast = parser.parse("OPENQASM 2.0; include \"qelib1.inc\"; qreg q[2]; h q[0]; cx q[0],q[1];");
    
    // Build quantum circuit
    QuantumCircuit circuit(2);
    // ... add gates from AST
    
    // Simulate circuit
    QuantumSimulator simulator(2);
    simulator.simulate(circuit, 1000);
    
    // Get results
    auto probabilities = simulator.getProbabilities();
    return 0;
}
``` 