# Quantum Debugger

The `QuantumDebugger` class provides debugging capabilities for quantum circuits, allowing step-by-step execution, breakpoints, and state analysis.

## Features

- Step-by-step circuit execution
- Multiple breakpoint types:
  - Gate breakpoints
  - State breakpoints
  - Probability breakpoints
  - Custom breakpoints
- State analysis and inspection
- Probability and entanglement calculations
- Circuit execution control

## Usage

### Basic Debugging

```cpp
QuantumCircuit circuit(2);
circuit.addGate(QuantumCircuit::GateType::H, {0});
circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 1});

QuantumState state(2);
QuantumDebugger debugger(circuit, state);

// Step through execution
debugger.step();  // Execute first gate
debugger.step();  // Execute second gate

// Continue execution until breakpoint
debugger.continueExecution();
```

### Breakpoints

```cpp
// Add gate breakpoint
debugger.addGateBreakpoint(1);  // Break at second gate

// Add state breakpoint
debugger.addStateBreakpoint(
    [](const QuantumState& state) { 
        return state.getProbability(0) > 0.5; 
    },
    "Probability threshold reached"
);

// Add probability breakpoint
debugger.addProbabilityBreakpoint(0, 0.5);  // Break when qubit 0 probability reaches 0.5
```

### State Analysis

```cpp
// Get current state probabilities
auto probs = debugger.getStateProbabilities();

// Get probability for specific qubit
double prob = debugger.getProbability(0, true);

// Get entanglement between qubits
double entanglement = debugger.getEntanglement(0, 1);

// Get formatted state information
std::string info = debugger.getStateInfo();
```

## Breakpoint Types

1. **Gate Breakpoints**
   - Break execution at specific gate indices
   - Useful for debugging specific circuit operations

2. **State Breakpoints**
   - Break based on quantum state conditions
   - Custom conditions using lambda functions
   - Example: break when entanglement exceeds threshold

3. **Probability Breakpoints**
   - Break when qubit probability reaches threshold
   - Useful for monitoring quantum state evolution

4. **Custom Breakpoints**
   - User-defined break conditions
   - Maximum flexibility for debugging scenarios

## State Analysis

The debugger provides several tools for analyzing the quantum state:

- **State Probabilities**: Get probability distribution for all basis states
- **Qubit Probabilities**: Get probability for specific qubits
- **Entanglement**: Calculate entanglement between qubits
- **State Information**: Get formatted string with current state details

## Performance Considerations

- Breakpoint evaluation is performed after each gate execution
- State analysis functions may be computationally intensive for large circuits
- Use breakpoints judiciously to minimize performance impact

## Example

```cpp
// Create circuit with Hadamard and CNOT
QuantumCircuit circuit(2);
circuit.addGate(QuantumCircuit::GateType::H, {0});
circuit.addGate(QuantumCircuit::GateType::CNOT, {0, 1});

// Initialize debugger
QuantumState state(2);
QuantumDebugger debugger(circuit, state);

// Add breakpoint at CNOT gate
debugger.addGateBreakpoint(1);

// Execute circuit
debugger.continueExecution();

// Analyze state
std::string info = debugger.getStateInfo();
std::cout << "State after breakpoint:\n" << info << std::endl;
```

## Error Handling

The debugger provides error handling for common scenarios:

- Invalid gate indices
- Invalid qubit indices
- Breakpoint evaluation errors
- State analysis errors

All errors are reported through exceptions with descriptive messages. 