#include "interpreter.hpp"
#include <stdexcept>
#include <cmath>
#include <random>

namespace qasm {

Interpreter::Interpreter() {}

Interpreter::~Interpreter() {}

void Interpreter::execute(const std::shared_ptr<Program>& program) {
    visitProgram(program);
}

void Interpreter::visitProgram(const std::shared_ptr<Program>& node) {
    for (const auto& stmt : node->statements) {
        if (auto qreg = std::dynamic_pointer_cast<QRegDecl>(stmt)) {
            visitQRegDecl(qreg);
        } else if (auto creg = std::dynamic_pointer_cast<CRegDecl>(stmt)) {
            visitCRegDecl(creg);
        } else if (auto gate = std::dynamic_pointer_cast<GateDecl>(stmt)) {
            visitGateDecl(gate);
        } else if (auto call = std::dynamic_pointer_cast<GateCall>(stmt)) {
            visitGateCall(call);
        } else if (auto meas = std::dynamic_pointer_cast<Measurement>(stmt)) {
            visitMeasurement(meas);
        }
    }
}

void Interpreter::visitQRegDecl(const std::shared_ptr<QRegDecl>& node) {
    if (qregisters.find(node->name) != qregisters.end()) {
        throw std::runtime_error("Quantum register '" + node->name + "' already declared");
    }
    qregisters[node->name] = std::vector<bool>(node->size, false);
    initializeQuantumState(node->name, node->size);
}

void Interpreter::visitCRegDecl(const std::shared_ptr<CRegDecl>& node) {
    if (cregisters.find(node->name) != cregisters.end()) {
        throw std::runtime_error("Classical register '" + node->name + "' already declared");
    }
    cregisters[node->name] = std::vector<bool>(node->size, false);
}

void Interpreter::visitGateDecl(const std::shared_ptr<GateDecl>& node) {
    gates[node->name] = *node;
}

void Interpreter::visitGateCall(const std::shared_ptr<GateCall>& node) {
    if (node->name == "h") {
        applyHadamard(node->qubits[0]);
    } else if (node->name == "cx") {
        applyCNOT(node->qubits[0], node->qubits[1]);
    } else if (node->name == "x") {
        applyX(node->qubits[0]);
    } else if (node->name == "y") {
        applyY(node->qubits[0]);
    } else if (node->name == "z") {
        applyZ(node->qubits[0]);
    } else if (node->name == "p") {
        double angle = std::dynamic_pointer_cast<Real>(node->args[0])->value;
        applyPhase(node->qubits[0], angle);
    } else if (node->name == "rx") {
        double angle = std::dynamic_pointer_cast<Real>(node->args[0])->value;
        applyRx(node->qubits[0], angle);
    } else if (node->name == "ry") {
        double angle = std::dynamic_pointer_cast<Real>(node->args[0])->value;
        applyRy(node->qubits[0], angle);
    } else if (node->name == "rz") {
        double angle = std::dynamic_pointer_cast<Real>(node->args[0])->value;
        applyRz(node->qubits[0], angle);
    } else {
        throw std::runtime_error("Unknown gate: " + node->name);
    }
}

void Interpreter::visitMeasurement(const std::shared_ptr<Measurement>& node) {
    // Get the quantum state
    auto& state = quantum_states[node->qubit->reg];
    int index = node->qubit->index;
    
    // Calculate probabilities
    double prob0 = std::norm(state[2 * index]);
    double prob1 = std::norm(state[2 * index + 1]);
    
    // Random measurement
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    bool result = dis(gen) > prob0;
    
    // Update classical register
    cregisters[node->cbit->reg][node->cbit->index] = result;
    
    // Collapse quantum state
    if (result) {
        state[2 * index] = 0.0;
        state[2 * index + 1] = 1.0;
    } else {
        state[2 * index] = 1.0;
        state[2 * index + 1] = 0.0;
    }
}

void Interpreter::initializeQuantumState(const std::string& reg, int size) {
    std::vector<std::complex<double>> state(1 << size, 0.0);
    state[0] = 1.0; // Initialize to |0⟩
    quantum_states[reg] = state;
}

void Interpreter::updateQuantumState(const std::string& reg, int index, 
                                   const std::vector<std::complex<double>>& new_state) {
    auto& state = quantum_states[reg];
    int offset = index * 2;
    state[offset] = new_state[0];
    state[offset + 1] = new_state[1];
}

std::vector<std::complex<double>> Interpreter::tensorProduct(
    const std::vector<std::complex<double>>& a,
    const std::vector<std::complex<double>>& b) {
    std::vector<std::complex<double>> result(a.size() * b.size());
    for (size_t i = 0; i < a.size(); ++i) {
        for (size_t j = 0; j < b.size(); ++j) {
            result[i * b.size() + j] = a[i] * b[j];
        }
    }
    return result;
}

std::vector<std::complex<double>> Interpreter::matrixMultiply(
    const std::vector<std::complex<double>>& matrix,
    const std::vector<std::complex<double>>& state) {
    std::vector<std::complex<double>> result(state.size());
    for (size_t i = 0; i < state.size(); ++i) {
        result[i] = 0.0;
        for (size_t j = 0; j < state.size(); ++j) {
            result[i] += matrix[i * state.size() + j] * state[j];
        }
    }
    return result;
}

// Quantum gate implementations
void Interpreter::applyHadamard(const std::shared_ptr<Qubit>& qubit) {
    const double inv_sqrt2 = 1.0 / std::sqrt(2.0);
    std::vector<std::complex<double>> h_matrix = {
        inv_sqrt2, inv_sqrt2,
        inv_sqrt2, -inv_sqrt2
    };
    updateQuantumState(qubit->reg, qubit->index, 
                      matrixMultiply(h_matrix, {1.0, 0.0}));
}

void Interpreter::applyCNOT(const std::shared_ptr<Qubit>& control, 
                          const std::shared_ptr<Qubit>& target) {
    // TODO: Implement CNOT gate
}

void Interpreter::applyX(const std::shared_ptr<Qubit>& qubit) {
    std::vector<std::complex<double>> x_matrix = {
        0.0, 1.0,
        1.0, 0.0
    };
    updateQuantumState(qubit->reg, qubit->index,
                      matrixMultiply(x_matrix, {1.0, 0.0}));
}

void Interpreter::applyY(const std::shared_ptr<Qubit>& qubit) {
    std::vector<std::complex<double>> y_matrix = {
        0.0, std::complex<double>(0.0, -1.0),
        std::complex<double>(0.0, 1.0), 0.0
    };
    updateQuantumState(qubit->reg, qubit->index,
                      matrixMultiply(y_matrix, {1.0, 0.0}));
}

void Interpreter::applyZ(const std::shared_ptr<Qubit>& qubit) {
    std::vector<std::complex<double>> z_matrix = {
        1.0, 0.0,
        0.0, -1.0
    };
    updateQuantumState(qubit->reg, qubit->index,
                      matrixMultiply(z_matrix, {1.0, 0.0}));
}

void Interpreter::applyPhase(const std::shared_ptr<Qubit>& qubit, double angle) {
    std::vector<std::complex<double>> p_matrix = {
        1.0, 0.0,
        0.0, std::exp(std::complex<double>(0.0, angle))
    };
    updateQuantumState(qubit->reg, qubit->index,
                      matrixMultiply(p_matrix, {1.0, 0.0}));
}

void Interpreter::applyRx(const std::shared_ptr<Qubit>& qubit, double angle) {
    double c = std::cos(angle/2);
    double s = std::sin(angle/2);
    std::vector<std::complex<double>> rx_matrix = {
        c, std::complex<double>(0.0, -s),
        std::complex<double>(0.0, -s), c
    };
    updateQuantumState(qubit->reg, qubit->index,
                      matrixMultiply(rx_matrix, {1.0, 0.0}));
}

void Interpreter::applyRy(const std::shared_ptr<Qubit>& qubit, double angle) {
    double c = std::cos(angle/2);
    double s = std::sin(angle/2);
    std::vector<std::complex<double>> ry_matrix = {
        c, -s,
        s, c
    };
    updateQuantumState(qubit->reg, qubit->index,
                      matrixMultiply(ry_matrix, {1.0, 0.0}));
}

void Interpreter::applyRz(const std::shared_ptr<Qubit>& qubit, double angle) {
    std::vector<std::complex<double>> rz_matrix = {
        std::exp(std::complex<double>(0.0, -angle/2)), 0.0,
        0.0, std::exp(std::complex<double>(0.0, angle/2))
    };
    updateQuantumState(qubit->reg, qubit->index,
                      matrixMultiply(rz_matrix, {1.0, 0.0}));
}

} // namespace qasm 