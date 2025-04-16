#ifndef AST_HPP
#define AST_HPP

#include <memory>
#include <string>
#include <vector>

namespace qasm {

// Forward declarations
class Node;
class Program;
class QRegDecl;
class CRegDecl;
class GateDecl;
class GateCall;
class Measurement;
class Qubit;
class ClassicalBit;

// Base class for all AST nodes
class Node {
public:
    virtual ~Node() = default;
    virtual std::string toString() const = 0;
};

// Root node of the AST
class Program : public Node {
public:
    std::vector<std::shared_ptr<Node>> statements;
    std::string toString() const override;
};

// Quantum register declaration
class QRegDecl : public Node {
public:
    std::string name;
    int size;
    QRegDecl(const std::string& name, int size);
    std::string toString() const override;
};

// Classical register declaration
class CRegDecl : public Node {
public:
    std::string name;
    int size;
    CRegDecl(const std::string& name, int size);
    std::string toString() const override;
};

// Gate declaration
class GateDecl : public Node {
public:
    std::string name;
    std::vector<std::string> params;
    std::vector<std::string> qubits;
    std::vector<std::shared_ptr<Node>> body;
    GateDecl(const std::string& name, 
             const std::vector<std::string>& params,
             const std::vector<std::string>& qubits);
    std::string toString() const override;
};

// Gate call
class GateCall : public Node {
public:
    std::string name;
    std::vector<std::shared_ptr<Node>> args;
    std::vector<std::shared_ptr<Qubit>> qubits;
    GateCall(const std::string& name,
             const std::vector<std::shared_ptr<Node>>& args,
             const std::vector<std::shared_ptr<Qubit>>& qubits);
    std::string toString() const override;
};

// Measurement operation
class Measurement : public Node {
public:
    std::shared_ptr<Qubit> qubit;
    std::shared_ptr<ClassicalBit> cbit;
    Measurement(std::shared_ptr<Qubit> qubit, std::shared_ptr<ClassicalBit> cbit);
    std::string toString() const override;
};

// Qubit reference
class Qubit : public Node {
public:
    std::string reg;
    int index;
    Qubit(const std::string& reg, int index);
    std::string toString() const override;
};

// Classical bit reference
class ClassicalBit : public Node {
public:
    std::string reg;
    int index;
    ClassicalBit(const std::string& reg, int index);
    std::string toString() const override;
};

} // namespace qasm

#endif // AST_HPP 