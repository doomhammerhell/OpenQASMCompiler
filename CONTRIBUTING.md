# Contributing to OpenQASM Compiler

Thank you for your interest in contributing to OpenQASM Compiler! This document provides guidelines and instructions for contributing to the project.

## Code of Conduct

Please read and follow our [Code of Conduct](CODE_OF_CONDUCT.md) in all your interactions with the project.

## How to Contribute

1. Fork the repository
2. Create a new branch for your feature/fix
3. Make your changes
4. Run tests to ensure everything works
5. Submit a pull request

## Development Setup

1. Clone your fork:
```bash
git clone https://github.com/your-username/OpenQASMCompiler.git
cd OpenQASMCompiler
```

2. Install dependencies:
```bash
mkdir build && cd build
cmake ..
make
```

3. Run tests:
```bash
ctest
```

## Coding Standards

- Follow the existing code style
- Write clear, concise commit messages
- Include tests for new features
- Update documentation as needed
- Use English for all comments and documentation

## Pull Request Process

1. Update the README.md with details of changes if needed
2. Update the documentation if needed
3. The PR must pass all tests
4. The PR must be reviewed by at least one maintainer

## Reporting Issues

When reporting issues, please include:
- A clear description of the problem
- Steps to reproduce
- Expected vs actual behavior
- Environment details (OS, compiler version, etc.)

## Feature Requests

For feature requests:
- Describe the feature in detail
- Explain why it would be useful
- Provide example use cases if possible

## Questions?

Feel free to open an issue for any questions about contributing to the project. 