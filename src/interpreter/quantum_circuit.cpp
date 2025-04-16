#include "quantum_circuit.hpp"
#include <sstream>
#include <iomanip>

namespace qasm {

QuantumCircuit::QuantumCircuit(int num_qubits) : num_qubits(num_qubits) {}

QuantumCircuit::~QuantumCircuit() {}

void QuantumCircuit::addGate(GateType type, const std::vector<int>& qubits, double angle) {
    Gate gate;
    gate.type = type;
    gate.qubits = qubits;
    gate.angle = angle;
    gates.push_back(gate);
}

void QuantumCircuit::execute(QuantumState& state) {
    for (const auto& gate : gates) {
        switch (gate.type) {
            case GateType::H:
                state.applyHadamard(gate.qubits[0]);
                break;
            case GateType::X:
                state.applyX(gate.qubits[0]);
                break;
            case GateType::Y:
                state.applyY(gate.qubits[0]);
                break;
            case GateType::Z:
                state.applyZ(gate.qubits[0]);
                break;
            case GateType::CNOT:
                state.applyCNOT(gate.qubits[0], gate.qubits[1]);
                break;
            case GateType::SWAP:
                state.applySWAP(gate.qubits[0], gate.qubits[1]);
                break;
            case GateType::CZ:
                state.applyCZ(gate.qubits[0], gate.qubits[1]);
                break;
            case GateType::TOFFOLI:
                state.applyToffoli(gate.qubits[0], gate.qubits[1], gate.qubits[2]);
                break;
            case GateType::FREDKIN:
                state.applyFredkin(gate.qubits[0], gate.qubits[1], gate.qubits[2]);
                break;
            case GateType::RX:
                state.applyRx(gate.qubits[0], gate.angle);
                break;
            case GateType::RY:
                state.applyRy(gate.qubits[0], gate.angle);
                break;
            case GateType::RZ:
                state.applyRz(gate.qubits[0], gate.angle);
                break;
            case GateType::MEASURE:
                state.measure(gate.qubits[0]);
                break;
        }
    }
}

std::string QuantumCircuit::gateTypeToString(GateType type) const {
    switch (type) {
        case GateType::H: return "h";
        case GateType::X: return "x";
        case GateType::Y: return "y";
        case GateType::Z: return "z";
        case GateType::CNOT: return "cx";
        case GateType::SWAP: return "swap";
        case GateType::CZ: return "cz";
        case GateType::TOFFOLI: return "ccx";
        case GateType::FREDKIN: return "cswap";
        case GateType::RX: return "rx";
        case GateType::RY: return "ry";
        case GateType::RZ: return "rz";
        case GateType::MEASURE: return "measure";
        default: return "unknown";
    }
}

std::string QuantumCircuit::gateToQASM(const Gate& gate) const {
    std::stringstream ss;
    ss << gateTypeToString(gate.type);
    
    if (gate.type == GateType::RX || gate.type == GateType::RY || gate.type == GateType::RZ) {
        ss << "(" << gate.angle << ")";
    }
    
    for (int qubit : gate.qubits) {
        ss << " q[" << qubit << "]";
    }
    
    ss << ";\n";
    return ss.str();
}

std::string QuantumCircuit::gateToLatex(const Gate& gate) const {
    std::stringstream ss;
    
    switch (gate.type) {
        case GateType::H:
            ss << "\\gate{H}";
            break;
        case GateType::X:
            ss << "\\gate{X}";
            break;
        case GateType::Y:
            ss << "\\gate{Y}";
            break;
        case GateType::Z:
            ss << "\\gate{Z}";
            break;
        case GateType::CNOT:
            ss << "\\ctrl{" << gate.qubits[0] << "}\\targ{" << gate.qubits[1] << "}";
            break;
        case GateType::SWAP:
            ss << "\\swap{" << gate.qubits[0] << "}{" << gate.qubits[1] << "}";
            break;
        case GateType::CZ:
            ss << "\\ctrl{" << gate.qubits[0] << "}\\gate{Z}";
            break;
        case GateType::TOFFOLI:
            ss << "\\ctrl{" << gate.qubits[0] << "}\\ctrl{" << gate.qubits[1] << "}\\targ{" << gate.qubits[2] << "}";
            break;
        case GateType::FREDKIN:
            ss << "\\ctrl{" << gate.qubits[0] << "}\\swap{" << gate.qubits[1] << "}{" << gate.qubits[2] << "}";
            break;
        case GateType::RX:
            ss << "\\gate{R_x(" << gate.angle << ")}";
            break;
        case GateType::RY:
            ss << "\\gate{R_y(" << gate.angle << ")}";
            break;
        case GateType::RZ:
            ss << "\\gate{R_z(" << gate.angle << ")}";
            break;
        case GateType::MEASURE:
            ss << "\\meter";
            break;
    }
    
    return ss.str();
}

std::string QuantumCircuit::gateToASCII(const Gate& gate) const {
    std::stringstream ss;
    
    switch (gate.type) {
        case GateType::H:
            ss << "H";
            break;
        case GateType::X:
            ss << "X";
            break;
        case GateType::Y:
            ss << "Y";
            break;
        case GateType::Z:
            ss << "Z";
            break;
        case GateType::CNOT:
            ss << "CNOT(" << gate.qubits[0] << "," << gate.qubits[1] << ")";
            break;
        case GateType::SWAP:
            ss << "SWAP(" << gate.qubits[0] << "," << gate.qubits[1] << ")";
            break;
        case GateType::CZ:
            ss << "CZ(" << gate.qubits[0] << "," << gate.qubits[1] << ")";
            break;
        case GateType::TOFFOLI:
            ss << "TOFFOLI(" << gate.qubits[0] << "," << gate.qubits[1] << "," << gate.qubits[2] << ")";
            break;
        case GateType::FREDKIN:
            ss << "FREDKIN(" << gate.qubits[0] << "," << gate.qubits[1] << "," << gate.qubits[2] << ")";
            break;
        case GateType::RX:
            ss << "RX(" << gate.angle << ")";
            break;
        case GateType::RY:
            ss << "RY(" << gate.angle << ")";
            break;
        case GateType::RZ:
            ss << "RZ(" << gate.angle << ")";
            break;
        case GateType::MEASURE:
            ss << "MEASURE";
            break;
    }
    
    return ss.str();
}

std::string QuantumCircuit::toQASM() const {
    std::stringstream ss;
    ss << "OPENQASM 2.0;\n";
    ss << "include \"qelib1.inc\";\n";
    ss << "qreg q[" << num_qubits << "];\n";
    ss << "creg c[" << num_qubits << "];\n\n";
    
    for (const auto& gate : gates) {
        ss << gateToQASM(gate);
    }
    
    return ss.str();
}

std::string QuantumCircuit::toLatex() const {
    std::stringstream ss;
    ss << "\\begin{quantikz}\n";
    
    for (int i = 0; i < num_qubits; ++i) {
        ss << "\\lstick{$q_" << i << "$}";
        for (const auto& gate : gates) {
            if (std::find(gate.qubits.begin(), gate.qubits.end(), i) != gate.qubits.end()) {
                ss << " & " << gateToLatex(gate);
            } else {
                ss << " & ";
            }
        }
        ss << " \\\\\n";
    }
    
    ss << "\\end{quantikz}";
    return ss.str();
}

std::string QuantumCircuit::toASCII() const {
    std::stringstream ss;
    
    for (const auto& gate : gates) {
        ss << gateToASCII(gate) << "\n";
    }
    
    return ss.str();
}

} // namespace qasm 