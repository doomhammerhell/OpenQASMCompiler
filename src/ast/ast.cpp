#include "ast.hpp"
#include <sstream>

namespace qasm {

std::string Program::toString() const {
    std::string result = "Program:\n";
    for (const auto& stmt : statements) {
        result += "  " + stmt->toString() + "\n";
    }
    return result;
}

QRegDecl::QRegDecl(const std::string& name, int size)
    : name(name), size(size) {}

std::string QRegDecl::toString() const {
    return "QReg " + name + "[" + std::to_string(size) + "]";
}

CRegDecl::CRegDecl(const std::string& name, int size)
    : name(name), size(size) {}

std::string CRegDecl::toString() const {
    return "CReg " + name + "[" + std::to_string(size) + "]";
}

GateDecl::GateDecl(const std::string& name,
                   const std::vector<std::string>& params,
                   const std::vector<std::string>& qubits)
    : name(name), params(params), qubits(qubits) {}

std::string GateDecl::toString() const {
    std::stringstream ss;
    ss << "gate " << name;
    if (!params.empty()) {
        ss << "(";
        for (size_t i = 0; i < params.size(); ++i) {
            if (i > 0) ss << ",";
            ss << params[i];
        }
        ss << ")";
    }
    ss << " ";
    for (size_t i = 0; i < qubits.size(); ++i) {
        if (i > 0) ss << ",";
        ss << qubits[i];
    }
    ss << " {";
    for (const auto& stmt : body) {
        ss << "\n  " << stmt->toString();
    }
    ss << "\n}";
    return ss.str();
}

GateCall::GateCall(const std::string& name,
                   const std::vector<std::shared_ptr<Node>>& args,
                   const std::vector<std::shared_ptr<Qubit>>& qubits)
    : name(name), args(args), qubits(qubits) {}

std::string GateCall::toString() const {
    std::stringstream ss;
    ss << name;
    if (!args.empty()) {
        ss << "(";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) ss << ",";
            ss << args[i]->toString();
        }
        ss << ")";
    }
    ss << " ";
    for (size_t i = 0; i < qubits.size(); ++i) {
        if (i > 0) ss << ",";
        ss << qubits[i]->toString();
    }
    return ss.str();
}

Measurement::Measurement(std::shared_ptr<Qubit> qubit, std::shared_ptr<ClassicalBit> cbit)
    : qubit(qubit), cbit(cbit) {}

std::string Measurement::toString() const {
    return "measure " + qubit->toString() + " -> " + cbit->toString();
}

Qubit::Qubit(const std::string& reg, int index)
    : reg(reg), index(index) {}

std::string Qubit::toString() const {
    return reg + "[" + std::to_string(index) + "]";
}

ClassicalBit::ClassicalBit(const std::string& reg, int index)
    : reg(reg), index(index) {}

std::string ClassicalBit::toString() const {
    return reg + "[" + std::to_string(index) + "]";
}

} // namespace qasm 