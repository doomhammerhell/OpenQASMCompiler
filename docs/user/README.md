# User Guide

## Installation

### Prerequisites

- C++17 compatible compiler
- CMake 3.10 or higher
- Eigen3
- CLI11

### Installation Steps

1. **Download the Source**
```bash
git clone https://github.com/yourusername/OpenQASMCompiler.git
cd OpenQASMCompiler
```

2. **Build and Install**
```bash
mkdir build
cd build
cmake ..
cmake --build .
sudo cmake --install .
```

### Package Managers

#### Ubuntu/Debian
```bash
sudo apt-get install libeigen3-dev libcli11-dev
```

#### macOS
```bash
brew install eigen cli11
```

#### Windows
```bash
vcpkg install eigen3:x64-windows cli11:x64-windows
```

## Basic Usage

### Command Line Interface

The basic command format is:
```bash
qasmc [options] input.qasm
```

### Common Options

- `-o, --output`: Specify output file
- `--optimize`: Enable circuit optimization
- `--visualize`: Generate circuit visualization
- `--simulate`: Run circuit simulation
- `--shots`: Number of simulation shots (default: 1000)

### Examples

1. **Basic Compilation**
```bash
qasmc circuit.qasm -o optimized.qasm
```

2. **With Optimization**
```bash
qasmc circuit.qasm -o optimized.qasm --optimize
```

3. **Generate Visualization**
```bash
qasmc circuit.qasm --visualize
```

4. **Run Simulation**
```bash
qasmc circuit.qasm --simulate --shots 10000
```

## OpenQASM Examples

### 1. Bell State Creation
```qasm
OPENQASM 2.0;
include "qelib1.inc";

qreg q[2];
creg c[2];

h q[0];
cx q[0], q[1];

measure q[0] -> c[0];
measure q[1] -> c[1];
```

### 2. Quantum Fourier Transform
```qasm
OPENQASM 2.0;
include "qelib1.inc";

qreg q[3];
creg c[3];

h q[0];
h q[1];
h q[2];

crz(pi/2) q[1],q[0];
crz(pi/4) q[2],q[0];
crz(pi/2) q[2],q[1];

swap q[0],q[2];

measure q[0] -> c[0];
measure q[1] -> c[1];
measure q[2] -> c[2];
```

### 3. Grover's Search
```qasm
OPENQASM 2.0;
include "qelib1.inc";

qreg q[2];
creg c[2];

h q[0];
h q[1];

// Oracle for |11⟩
x q[0];
x q[1];
h q[1];
cx q[0],q[1];
h q[1];
x q[0];
x q[1];

// Diffusion operator
h q[0];
h q[1];
x q[0];
x q[1];
h q[1];
cx q[0],q[1];
h q[1];
x q[0];
x q[1];
h q[0];
h q[1];

measure q[0] -> c[0];
measure q[1] -> c[1];
```

## Advanced Features

### Circuit Optimization

The compiler supports several optimization techniques:

1. **Gate Merging**
   - Combines consecutive single-qubit gates
   - Optimizes controlled gates
   - Reduces circuit depth

2. **Circuit Simplification**
   - Removes redundant gates
   - Simplifies gate sequences
   - Optimizes measurement operations

3. **Resource Estimation**
   - Estimates qubit requirements
   - Calculates circuit depth
   - Predicts execution time

### Simulation Options

1. **Noise Models**
   - Depolarizing noise
   - Amplitude damping
   - Phase damping
   - Custom noise models

2. **Measurement Options**
   - Single-shot measurement
   - Multiple shots with statistics
   - Partial measurement
   - Conditional measurement

3. **State Analysis**
   - State vector inspection
   - Probability distribution
   - Expectation values
   - Entanglement measures

## Troubleshooting

### Common Issues

1. **Parser Errors**
```
Error: Syntax error at line 5
Solution: Check OpenQASM syntax and include statements
```

2. **Circuit Errors**
```
Error: Invalid qubit index
Solution: Verify qubit register size and indices
```

3. **Simulation Errors**
```
Error: Out of memory
Solution: Reduce circuit size or use optimization
```

### Debugging Tips

1. **Verbose Output**
```bash
qasmc circuit.qasm --verbose
```

2. **Circuit Validation**
```bash
qasmc circuit.qasm --validate
```

3. **Performance Profiling**
```bash
qasmc circuit.qasm --profile
```

## Best Practices

1. **Circuit Design**
   - Use meaningful qubit names
   - Document circuit purpose
   - Include comments
   - Test with small examples

2. **Optimization**
   - Start with simple circuits
   - Use optimization flags
   - Monitor resource usage
   - Profile performance

3. **Simulation**
   - Choose appropriate shot count
   - Use relevant noise models
   - Verify results
   - Document assumptions 