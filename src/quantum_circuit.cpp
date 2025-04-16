#include "OpenQASMCompiler/quantum_circuit.hpp"
#include "OpenQASMCompiler/quantum_gates.hpp"
#include "OpenQASMCompiler/circuit_optimizer.hpp"
#include <sstream>
#include <stdexcept>
#include <algorithm>

namespace qasm {

QuantumCircuit::QuantumCircuit(size_t num_qubits)
    : num_qubits_(num_qubits), num_classical_bits_(0) {}

QuantumCircuit::QuantumCircuit(size_t num_qubits, size_t num_classical_bits)
    : num_qubits_(num_qubits), num_classical_bits_(num_classical_bits) {}

void QuantumCircuit::addGate(std::unique_ptr<QuantumGate> gate) {
    // Validate qubit indices
    for (const auto& qubit : gate->getQubits()) {
        if (qubit >= num_qubits_) {
            throw std::out_of_range("Qubit index out of range");
        }
    }
    gates_.push_back(std::move(gate));
}

void QuantumCircuit::addMeasurement(size_t qubit, size_t classical_bit) {
    if (qubit >= num_qubits_) {
        throw std::out_of_range("Qubit index out of range");
    }
    if (classical_bit >= num_classical_bits_) {
        throw std::out_of_range("Classical bit index out of range");
    }
    measurements_.push_back({qubit, classical_bit});
}

std::string QuantumCircuit::toString() const {
    std::ostringstream ss;
    ss << "Quantum Circuit with " << num_qubits_ << " qubits and "
       << num_classical_bits_ << " classical bits\n";
    
    // Create circuit diagram
    std::vector<std::string> qubit_lines(num_qubits_, "");
    size_t max_width = 0;

    // Process gates
    for (const auto& gate : gates_) {
        auto qubits = gate->getQubits();
        std::string gate_symbol;
        
        switch (gate->getType()) {
            case GateType::H:  gate_symbol = "H"; break;
            case GateType::X:  gate_symbol = "X"; break;
            case GateType::Y:  gate_symbol = "Y"; break;
            case GateType::Z:  gate_symbol = "Z"; break;
            case GateType::RX: gate_symbol = "RX"; break;
            case GateType::RY: gate_symbol = "RY"; break;
            case GateType::RZ: gate_symbol = "RZ"; break;
            case GateType::P:  gate_symbol = "P"; break;
            case GateType::CNOT: {
                qubit_lines[qubits[0]] += "•";
                qubit_lines[qubits[1]] += "⊕";
                continue;
            }
            case GateType::CZ: {
                qubit_lines[qubits[0]] += "•";
                qubit_lines[qubits[1]] += "Z";
                continue;
            }
            case GateType::CUSTOM: gate_symbol = "U"; break;
            default: gate_symbol = "?"; break;
        }

        for (size_t qubit : qubits) {
            qubit_lines[qubit] += gate_symbol;
        }

        // Update maximum width
        max_width = std::max(max_width, qubit_lines[0].length());
    }

    // Process measurements
    for (const auto& meas : measurements_) {
        qubit_lines[meas.first] += "M";
    }

    // Print circuit
    for (size_t i = 0; i < num_qubits_; ++i) {
        ss << "q" << i << ": " << qubit_lines[i] << "\n";
    }

    return ss.str();
}

size_t QuantumCircuit::getNumQubits() const {
    return num_qubits_;
}

size_t QuantumCircuit::getNumClassicalBits() const {
    return num_classical_bits_;
}

const std::vector<std::unique_ptr<QuantumGate>>& QuantumCircuit::getGates() const {
    return gates_;
}

const std::vector<std::pair<size_t, size_t>>& QuantumCircuit::getMeasurements() const {
    return measurements_;
}

void QuantumCircuit::optimize() {
    CircuitOptimizer::optimize(*this);
}

} // namespace qasm 