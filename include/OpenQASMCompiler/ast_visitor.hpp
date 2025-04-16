#ifndef OPENQASM_AST_VISITOR_HPP
#define OPENQASM_AST_VISITOR_HPP

#include "ast.hpp"
#include "quantum_circuit.hpp"
#include <memory>

namespace qasm {

class ASTToCircuitVisitor : public ASTVisitor {
public:
    explicit ASTToCircuitVisitor(size_t num_qubits = 0, size_t num_classical_bits = 0)
        : circuit_(std::make_unique<QuantumCircuit>(num_qubits, num_classical_bits)) {}

    std::unique_ptr<QuantumCircuit> getCircuit() {
        return std::move(circuit_);
    }

    void visit(const ProgramNode& node) override {
        for (const auto& stmt : node.getStatements()) {
            stmt->accept(*this);
        }
    }

    void visit(const GateNode& node) override {
        // Create appropriate gate based on type
        std::unique_ptr<QuantumGate> gate;
        switch (node.getType()) {
            case GateType::H:
                gate = std::make_unique<HadamardGate>(node.getQubits()[0]);
                break;
            case GateType::X:
                gate = std::make_unique<PauliXGate>(node.getQubits()[0]);
                break;
            case GateType::Y:
                gate = std::make_unique<PauliYGate>(node.getQubits()[0]);
                break;
            case GateType::Z:
                gate = std::make_unique<PauliZGate>(node.getQubits()[0]);
                break;
            case GateType::CNOT:
                gate = std::make_unique<CNOTGate>(node.getQubits()[0], node.getQubits()[1]);
                break;
            case GateType::CZ:
                gate = std::make_unique<CZGate>(node.getQubits()[0], node.getQubits()[1]);
                break;
            default:
                throw std::runtime_error("Unsupported gate type");
        }
        circuit_->addGate(std::move(gate));
    }

    void visit(const MeasureNode& node) override {
        circuit_->addMeasurement(node.getQubit(), node.getClassicalBit());
    }

    void visit(const CustomGateNode& node) override {
        auto gate = std::make_unique<CustomGate>(
            node.getName(),
            node.getMatrix(),
            node.getQubits()
        );
        circuit_->addGate(std::move(gate));
    }

    void visit(const GateDeclarationNode& node) override {
        // Store gate definition for later use
        // This will be implemented when we add support for gate definitions
    }

    void visit(const QubitDeclarationNode& node) override {
        // Update circuit with new qubit count if needed
        if (node.getNumQubits() > circuit_->getNumQubits()) {
            circuit_ = std::make_unique<QuantumCircuit>(
                node.getNumQubits(),
                circuit_->getNumClassicalBits()
            );
        }
    }

    void visit(const ClassicalBitDeclarationNode& node) override {
        // Update circuit with new classical bit count if needed
        if (node.getNumBits() > circuit_->getNumClassicalBits()) {
            circuit_ = std::make_unique<QuantumCircuit>(
                circuit_->getNumQubits(),
                node.getNumBits()
            );
        }
    }

private:
    std::unique_ptr<QuantumCircuit> circuit_;
};

} // namespace qasm

#endif // OPENQASM_AST_VISITOR_HPP 