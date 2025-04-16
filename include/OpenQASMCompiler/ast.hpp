#ifndef OPENQASM_AST_HPP
#define OPENQASM_AST_HPP

#include "quantum_circuit.hpp"
#include <memory>
#include <vector>
#include <string>
#include <ostream>
#include <complex>

namespace qasm {

// Forward declarations
class ASTVisitor;

// Base class for all AST nodes
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) const = 0;
    virtual void print(std::ostream& os, int indent = 0) const = 0;
};

// AST Visitor interface
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(const class ProgramNode& node) = 0;
    virtual void visit(const class GateNode& node) = 0;
    virtual void visit(const class MeasureNode& node) = 0;
    virtual void visit(const class CustomGateNode& node) = 0;
    virtual void visit(const class GateDeclarationNode& node) = 0;
    virtual void visit(const class QubitDeclarationNode& node) = 0;
    virtual void visit(const class ClassicalBitDeclarationNode& node) = 0;
};

// Program node - root of the AST
class ProgramNode : public ASTNode {
public:
    void addStatement(std::unique_ptr<ASTNode> statement) {
        statements.push_back(std::move(statement));
    }

    const std::vector<std::unique_ptr<ASTNode>>& getStatements() const {
        return statements;
    }

    void accept(ASTVisitor& visitor) const override {
        visitor.visit(*this);
    }

    void print(std::ostream& os, int indent = 0) const override {
        os << std::string(indent, ' ') << "Program:\n";
        for (const auto& stmt : statements) {
            stmt->print(os, indent + 2);
        }
    }

private:
    std::vector<std::unique_ptr<ASTNode>> statements;
};

// Base class for gate nodes
class GateNode : public ASTNode {
public:
    GateNode(GateType type, const std::vector<size_t>& qubits)
        : type_(type), qubits_(qubits) {}

    GateType getType() const { return type_; }
    const std::vector<size_t>& getQubits() const { return qubits_; }

    void accept(ASTVisitor& visitor) const override {
        visitor.visit(*this);
    }

    void print(std::ostream& os, int indent = 0) const override {
        os << std::string(indent, ' ') << "Gate: " << static_cast<int>(type_)
           << " on qubits: ";
        for (size_t q : qubits_) {
            os << q << " ";
        }
        os << "\n";
    }

private:
    GateType type_;
    std::vector<size_t> qubits_;
};

// Parameterized gate node
class ParameterizedGateNode : public GateNode {
public:
    ParameterizedGateNode(GateType type, const std::vector<size_t>& qubits, double parameter)
        : GateNode(type, qubits), parameter_(parameter) {}

    double getParameter() const { return parameter_; }

    void print(std::ostream& os, int indent = 0) const override {
        os << std::string(indent, ' ') << "Parameterized Gate: " << static_cast<int>(getType())
           << " on qubits: ";
        for (size_t q : getQubits()) {
            os << q << " ";
        }
        os << "with parameter: " << parameter_ << "\n";
    }

private:
    double parameter_;
};

// Measurement node
class MeasureNode : public ASTNode {
public:
    MeasureNode(size_t qubit, size_t classical_bit)
        : qubit_(qubit), classical_bit_(classical_bit) {}

    size_t getQubit() const { return qubit_; }
    size_t getClassicalBit() const { return classical_bit_; }

    void accept(ASTVisitor& visitor) const override {
        visitor.visit(*this);
    }

    void print(std::ostream& os, int indent = 0) const override {
        os << std::string(indent, ' ') << "Measure qubit " << qubit_
           << " to classical bit " << classical_bit_ << "\n";
    }

private:
    size_t qubit_;
    size_t classical_bit_;
};

// Custom gate node
class CustomGateNode : public ASTNode {
public:
    CustomGateNode(const std::string& name,
                  const std::vector<std::vector<std::complex<double>>>& matrix,
                  const std::vector<size_t>& qubits)
        : name_(name), matrix_(matrix), qubits_(qubits) {}

    const std::string& getName() const { return name_; }
    const std::vector<std::vector<std::complex<double>>>& getMatrix() const { return matrix_; }
    const std::vector<size_t>& getQubits() const { return qubits_; }

    void accept(ASTVisitor& visitor) const override {
        visitor.visit(*this);
    }

    void print(std::ostream& os, int indent = 0) const override {
        os << std::string(indent, ' ') << "Custom Gate: " << name_ << " on qubits: ";
        for (size_t q : qubits_) {
            os << q << " ";
        }
        os << "\n";
    }

private:
    std::string name_;
    std::vector<std::vector<std::complex<double>>> matrix_;
    std::vector<size_t> qubits_;
};

// Gate declaration node
class GateDeclarationNode : public ASTNode {
public:
    GateDeclarationNode(const std::string& name,
                       const std::vector<std::string>& parameters,
                       const std::vector<std::string>& qubits,
                       std::unique_ptr<ProgramNode> body)
        : name_(name), parameters_(parameters), qubits_(qubits), body_(std::move(body)) {}

    const std::string& getName() const { return name_; }
    const std::vector<std::string>& getParameters() const { return parameters_; }
    const std::vector<std::string>& getQubits() const { return qubits_; }
    const ProgramNode& getBody() const { return *body_; }

    void accept(ASTVisitor& visitor) const override {
        visitor.visit(*this);
    }

    void print(std::ostream& os, int indent = 0) const override {
        os << std::string(indent, ' ') << "Gate Declaration: " << name_ << "\n";
        os << std::string(indent + 2, ' ') << "Parameters: ";
        for (const auto& param : parameters_) {
            os << param << " ";
        }
        os << "\n";
        os << std::string(indent + 2, ' ') << "Qubits: ";
        for (const auto& qubit : qubits_) {
            os << qubit << " ";
        }
        os << "\n";
        body_->print(os, indent + 2);
    }

private:
    std::string name_;
    std::vector<std::string> parameters_;
    std::vector<std::string> qubits_;
    std::unique_ptr<ProgramNode> body_;
};

// Qubit declaration node
class QubitDeclarationNode : public ASTNode {
public:
    explicit QubitDeclarationNode(size_t num_qubits)
        : num_qubits_(num_qubits) {}

    size_t getNumQubits() const { return num_qubits_; }

    void accept(ASTVisitor& visitor) const override {
        visitor.visit(*this);
    }

    void print(std::ostream& os, int indent = 0) const override {
        os << std::string(indent, ' ') << "Qubit Declaration: " << num_qubits_ << " qubits\n";
    }

private:
    size_t num_qubits_;
};

// Classical bit declaration node
class ClassicalBitDeclarationNode : public ASTNode {
public:
    explicit ClassicalBitDeclarationNode(size_t num_bits)
        : num_bits_(num_bits) {}

    size_t getNumBits() const { return num_bits_; }

    void accept(ASTVisitor& visitor) const override {
        visitor.visit(*this);
    }

    void print(std::ostream& os, int indent = 0) const override {
        os << std::string(indent, ' ') << "Classical Bit Declaration: " << num_bits_ << " bits\n";
    }

private:
    size_t num_bits_;
};

} // namespace qasm

#endif // OPENQASM_AST_HPP 