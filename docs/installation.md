# Installation Guide

This guide provides detailed instructions for installing OpenQASM Compiler on various platforms.

## System Requirements

### Minimum Requirements
- CPU: x86_64 processor with SSE4.2
- RAM: 4GB
- Storage: 1GB free space
- OS: Linux, macOS, or Windows 10/11

### Recommended Requirements
- CPU: x86_64 processor with AVX2 support
- RAM: 8GB or more
- Storage: 2GB free space
- OS: Ubuntu 20.04+, macOS 12+, or Windows 11

## Dependencies

### Required Dependencies
- C++17 compatible compiler (GCC 7+, Clang 5+, or MSVC 2019+)
- CMake 3.10 or later
- Git
- SIMD libraries (usually included with compiler)

### Optional Dependencies
- CURL (for cloud backends)
- gRPC (for Google Quantum)
- AWS SDK (for Amazon Braket)
- OpenSSL (for secure communications)
- Doxygen (for building documentation)

## Installation Steps

### Linux (Ubuntu/Debian)

1. Install system dependencies:
   ```bash
   sudo apt update
   sudo apt install build-essential cmake git libcurl4-openssl-dev libssl-dev
   ```

2. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/OpenQASMCompiler.git
   cd OpenQASMCompiler
   ```

3. Build and install:
   ```bash
   mkdir build && cd build
   cmake ..
   make -j$(nproc)
   sudo make install
   ```

### macOS

1. Install Homebrew if not already installed:
   ```bash
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```

2. Install dependencies:
   ```bash
   brew install cmake curl openssl
   ```

3. Clone and build:
   ```bash
   git clone https://github.com/yourusername/OpenQASMCompiler.git
   cd OpenQASMCompiler
   mkdir build && cd build
   cmake ..
   make -j$(sysctl -n hw.ncpu)
   sudo make install
   ```

### Windows

1. Install prerequisites:
   - Visual Studio 2019 or later with C++ workload
   - [CMake](https://cmake.org/download/)
   - [Git](https://git-scm.com/download/win)

2. Clone the repository:
   ```cmd
   git clone https://github.com/yourusername/OpenQASMCompiler.git
   cd OpenQASMCompiler
   ```

3. Build using CMake:
   ```cmd
   mkdir build
   cd build
   cmake .. -G "Visual Studio 16 2019" -A x64
   cmake --build . --config Release
   ```

## Optional Components

### Cloud Backend Support

#### IBM Quantum
```bash
cmake -DENABLE_IBM_QUANTUM=ON ..
```

#### Google Quantum
```bash
cmake -DENABLE_GOOGLE_QUANTUM=ON ..
```

#### Amazon Braket
```bash
cmake -DENABLE_AWS_BRAKET=ON ..
```

### Documentation
```bash
cmake -DBUILD_DOCUMENTATION=ON ..
make docs
```

## Verifying Installation

1. Run tests:
   ```bash
   cd build
   ctest --output-on-failure
   ```

2. Run example program:
   ```bash
   ./examples/bell_state
   ```

## Troubleshooting

### Common Issues

1. CMake can't find dependencies
   - Ensure all required packages are installed
   - Set CMAKE_PREFIX_PATH to point to dependency locations

2. Compilation errors
   - Verify compiler version supports C++17
   - Check for missing dependencies
   - Update to latest compiler version

3. SIMD-related errors
   - Check CPU supports required SIMD instructions
   - Enable/disable specific SIMD features via CMake options

### Getting Help

If you encounter issues:
1. Check the [FAQ](https://github.com/yourusername/OpenQASMCompiler/wiki/FAQ)
2. Search existing [issues](https://github.com/yourusername/OpenQASMCompiler/issues)
3. Ask in [discussions](https://github.com/yourusername/OpenQASMCompiler/discussions)

## Uninstallation

To uninstall:
```bash
cd build
sudo make uninstall
```

## Next Steps

- Read the [Getting Started Guide](getting_started.md)
- Try the [Tutorial](tutorial.md)
- Explore [Examples](examples.md) 