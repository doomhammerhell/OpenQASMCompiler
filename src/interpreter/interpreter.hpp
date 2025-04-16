#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <complex>
#include <vector>
#include "../ast/ast.hpp"

namespace qasm {

class Interpreter {
public:
    Interpreter();
    ~Interpreter();

    // Execute the AST
    void execute(const std::shared_ptr<Program>& program);

private:
    // State management
    std::unordered_map<std::string, std::vector<bool>> qregisters;
    std::unordered_map<std::string, std::vector<bool>> cregisters;
    std::unordered_map<std::string, std::vector<std::complex<double>>> quantum_states;
    std::unordered_map<std::string, GateDecl> gates;

    // Node visitors
    void visitProgram(const std::shared_ptr<Program>& node);
    void visitQRegDecl(const std::shared_ptr<QRegDecl>& node);
    void visitCRegDecl(const std::shared_ptr<CRegDecl>& node);
    void visitGateDecl(const std::shared_ptr<GateDecl>& node);
    void visitGateCall(const std::shared_ptr<GateCall>& node);
    void visitMeasurement(const std::shared_ptr<Measurement>& node);

    // Quantum operations
    void applyHadamard(const std::shared_ptr<Qubit>& qubit);
    void applyCNOT(const std::shared_ptr<Qubit>& control, const std::shared_ptr<Qubit>& target);
    void applyX(const std::shared_ptr<Qubit>& qubit);
    void applyY(const std::shared_ptr<Qubit>& qubit);
    void applyZ(const std::shared_ptr<Qubit>& qubit);
    void applyPhase(const std::shared_ptr<Qubit>& qubit, double angle);
    void applyRx(const std::shared_ptr<Qubit>& qubit, double angle);
    void applyRy(const std::shared_ptr<Qubit>& qubit, double angle);
    void applyRz(const std::shared_ptr<Qubit>& qubit, double angle);

    // Helper functions
    void initializeQuantumState(const std::string& reg, int size);
    void updateQuantumState(const std::string& reg, int index, const std::vector<std::complex<double>>& new_state);
    std::vector<std::complex<double>> tensorProduct(const std::vector<std::complex<double>>& a, 
                                                  const std::vector<std::complex<double>>& b);
    std::vector<std::complex<double>> matrixMultiply(const std::vector<std::complex<double>>& matrix,
                                                    const std::vector<std::complex<double>>& state);
};

} // namespace qasm

#endif // INTERPRETER_HPP 