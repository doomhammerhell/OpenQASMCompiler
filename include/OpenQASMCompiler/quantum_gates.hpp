#ifndef OPENQASM_QUANTUM_GATES_HPP
#define OPENQASM_QUANTUM_GATES_HPP

#include "quantum_circuit.hpp"
#include <complex>
#include <vector>

namespace qasm {

/**
 * @brief Base class for single-qubit gates.
 * 
 * This class provides common functionality for all single-qubit gates.
 */
class SingleQubitGate : public QuantumGate {
public:
    explicit SingleQubitGate(size_t qubit) : qubit_(qubit) {}
    std::vector<size_t> getQubits() const override { return {qubit_}; }
protected:
    size_t qubit_;
};

/**
 * @brief Base class for two-qubit gates.
 * 
 * This class provides common functionality for all two-qubit gates.
 */
class TwoQubitGate : public QuantumGate {
public:
    TwoQubitGate(size_t control, size_t target) : control_(control), target_(target) {}
    std::vector<size_t> getQubits() const override { return {control_, target_}; }
protected:
    size_t control_;
    size_t target_;
};

/**
 * @brief Base class for three-qubit gates.
 * 
 * This class provides common functionality for all three-qubit gates.
 */
class ThreeQubitGate : public QuantumGate {
public:
    ThreeQubitGate(size_t control1, size_t control2, size_t target)
        : control1_(control1), control2_(control2), target_(target) {}
    std::vector<size_t> getQubits() const override { return {control1_, control2_, target_}; }
protected:
    size_t control1_;
    size_t control2_;
    size_t target_;
};

/**
 * @brief Base class for parameterized gates.
 * 
 * This class provides common functionality for gates with parameters.
 */
class ParameterizedGate : public QuantumGate {
public:
    explicit ParameterizedGate(const std::vector<double>& params) : parameters_(params) {}
    std::vector<double> getParameters() const override { return parameters_; }
protected:
    std::vector<double> parameters_;
};

// Single-qubit gates
class XGate : public SingleQubitGate {
public:
    explicit XGate(size_t qubit) : SingleQubitGate(qubit) {}
    GateType getType() const override { return GateType::X; }
    std::vector<double> getParameters() const override { return {}; }
};

class YGate : public SingleQubitGate {
public:
    explicit YGate(size_t qubit) : SingleQubitGate(qubit) {}
    GateType getType() const override { return GateType::Y; }
    std::vector<double> getParameters() const override { return {}; }
};

class ZGate : public SingleQubitGate {
public:
    explicit ZGate(size_t qubit) : SingleQubitGate(qubit) {}
    GateType getType() const override { return GateType::Z; }
    std::vector<double> getParameters() const override { return {}; }
};

class HGate : public SingleQubitGate {
public:
    explicit HGate(size_t qubit) : SingleQubitGate(qubit) {}
    GateType getType() const override { return GateType::H; }
    std::vector<double> getParameters() const override { return {}; }
};

class SGate : public SingleQubitGate {
public:
    explicit SGate(size_t qubit) : SingleQubitGate(qubit) {}
    GateType getType() const override { return GateType::S; }
    std::vector<double> getParameters() const override { return {}; }
};

class SDagGate : public SingleQubitGate {
public:
    explicit SDagGate(size_t qubit) : SingleQubitGate(qubit) {}
    GateType getType() const override { return GateType::S_DAG; }
    std::vector<double> getParameters() const override { return {}; }
};

class TGate : public SingleQubitGate {
public:
    explicit TGate(size_t qubit) : SingleQubitGate(qubit) {}
    GateType getType() const override { return GateType::T; }
    std::vector<double> getParameters() const override { return {}; }
};

class TDagGate : public SingleQubitGate {
public:
    explicit TDagGate(size_t qubit) : SingleQubitGate(qubit) {}
    GateType getType() const override { return GateType::T_DAG; }
    std::vector<double> getParameters() const override { return {}; }
};

class RXGate : public SingleQubitGate, public ParameterizedGate {
public:
    RXGate(size_t qubit, double angle) : SingleQubitGate(qubit), ParameterizedGate({angle}) {}
    GateType getType() const override { return GateType::RX; }
};

class RYGate : public SingleQubitGate, public ParameterizedGate {
public:
    RYGate(size_t qubit, double angle) : SingleQubitGate(qubit), ParameterizedGate({angle}) {}
    GateType getType() const override { return GateType::RY; }
};

class RZGate : public SingleQubitGate, public ParameterizedGate {
public:
    RZGate(size_t qubit, double angle) : SingleQubitGate(qubit), ParameterizedGate({angle}) {}
    GateType getType() const override { return GateType::RZ; }
};

class PhaseGate : public SingleQubitGate, public ParameterizedGate {
public:
    PhaseGate(size_t qubit, double angle) : SingleQubitGate(qubit), ParameterizedGate({angle}) {}
    GateType getType() const override { return GateType::P; }
};

class U1Gate : public SingleQubitGate, public ParameterizedGate {
public:
    U1Gate(size_t qubit, double lambda) : SingleQubitGate(qubit), ParameterizedGate({lambda}) {}
    GateType getType() const override { return GateType::U1; }
};

class U2Gate : public SingleQubitGate, public ParameterizedGate {
public:
    U2Gate(size_t qubit, double phi, double lambda) : SingleQubitGate(qubit), ParameterizedGate({phi, lambda}) {}
    GateType getType() const override { return GateType::U2; }
};

class U3Gate : public SingleQubitGate, public ParameterizedGate {
public:
    U3Gate(size_t qubit, double theta, double phi, double lambda) 
        : SingleQubitGate(qubit), ParameterizedGate({theta, phi, lambda}) {}
    GateType getType() const override { return GateType::U3; }
};

// Two-qubit gates
class CNOTGate : public TwoQubitGate {
public:
    CNOTGate(size_t control, size_t target) : TwoQubitGate(control, target) {}
    GateType getType() const override { return GateType::CNOT; }
    std::vector<double> getParameters() const override { return {}; }
};

class CZGate : public TwoQubitGate {
public:
    CZGate(size_t control, size_t target) : TwoQubitGate(control, target) {}
    GateType getType() const override { return GateType::CZ; }
    std::vector<double> getParameters() const override { return {}; }
};

class SWAPGate : public TwoQubitGate {
public:
    SWAPGate(size_t qubit1, size_t qubit2) : TwoQubitGate(qubit1, qubit2) {}
    GateType getType() const override { return GateType::SWAP; }
    std::vector<double> getParameters() const override { return {}; }
};

class CPGate : public TwoQubitGate, public ParameterizedGate {
public:
    CPGate(size_t control, size_t target, double angle) 
        : TwoQubitGate(control, target), ParameterizedGate({angle}) {}
    GateType getType() const override { return GateType::CP; }
};

class CRXGate : public TwoQubitGate, public ParameterizedGate {
public:
    CRXGate(size_t control, size_t target, double angle) 
        : TwoQubitGate(control, target), ParameterizedGate({angle}) {}
    GateType getType() const override { return GateType::CRX; }
};

class CRYGate : public TwoQubitGate, public ParameterizedGate {
public:
    CRYGate(size_t control, size_t target, double angle) 
        : TwoQubitGate(control, target), ParameterizedGate({angle}) {}
    GateType getType() const override { return GateType::CRY; }
};

class CRZGate : public TwoQubitGate, public ParameterizedGate {
public:
    CRZGate(size_t control, size_t target, double angle) 
        : TwoQubitGate(control, target), ParameterizedGate({angle}) {}
    GateType getType() const override { return GateType::CRZ; }
};

class CU1Gate : public TwoQubitGate, public ParameterizedGate {
public:
    CU1Gate(size_t control, size_t target, double lambda) 
        : TwoQubitGate(control, target), ParameterizedGate({lambda}) {}
    GateType getType() const override { return GateType::CU1; }
};

class CU2Gate : public TwoQubitGate, public ParameterizedGate {
public:
    CU2Gate(size_t control, size_t target, double phi, double lambda) 
        : TwoQubitGate(control, target), ParameterizedGate({phi, lambda}) {}
    GateType getType() const override { return GateType::CU2; }
};

class CU3Gate : public TwoQubitGate, public ParameterizedGate {
public:
    CU3Gate(size_t control, size_t target, double theta, double phi, double lambda) 
        : TwoQubitGate(control, target), ParameterizedGate({theta, phi, lambda}) {}
    GateType getType() const override { return GateType::CU3; }
};

class ISWAPGate : public TwoQubitGate {
public:
    ISWAPGate(size_t qubit1, size_t qubit2) : TwoQubitGate(qubit1, qubit2) {}
    GateType getType() const override { return GateType::ISWAP; }
    std::vector<double> getParameters() const override { return {}; }
};

class SQISWAPGate : public TwoQubitGate {
public:
    SQISWAPGate(size_t qubit1, size_t qubit2) : TwoQubitGate(qubit1, qubit2) {}
    GateType getType() const override { return GateType::SQISWAP; }
    std::vector<double> getParameters() const override { return {}; }
};

// Three-qubit gates
class CCXGate : public ThreeQubitGate {
public:
    CCXGate(size_t control1, size_t control2, size_t target) 
        : ThreeQubitGate(control1, control2, target) {}
    GateType getType() const override { return GateType::CCX; }
    std::vector<double> getParameters() const override { return {}; }
};

class CCZGate : public ThreeQubitGate {
public:
    CCZGate(size_t control1, size_t control2, size_t target) 
        : ThreeQubitGate(control1, control2, target) {}
    GateType getType() const override { return GateType::CCZ; }
    std::vector<double> getParameters() const override { return {}; }
};

class CSWAPGate : public ThreeQubitGate {
public:
    CSWAPGate(size_t control, size_t target1, size_t target2) 
        : ThreeQubitGate(control, target1, target2) {}
    GateType getType() const override { return GateType::CSWAP; }
    std::vector<double> getParameters() const override { return {}; }
};

// Error correction gates
class SyncGate : public QuantumGate {
public:
    explicit SyncGate(const std::vector<size_t>& qubits) : qubits_(qubits) {}
    GateType getType() const override { return GateType::SYNC; }
    std::vector<size_t> getQubits() const override { return qubits_; }
    std::vector<double> getParameters() const override { return {}; }
private:
    std::vector<size_t> qubits_;
};

class MeasureGate : public QuantumGate {
public:
    MeasureGate(size_t qubit, size_t classical_bit) 
        : qubit_(qubit), classical_bit_(classical_bit) {}
    GateType getType() const override { return GateType::MEASURE; }
    std::vector<size_t> getQubits() const override { return {qubit_}; }
    std::vector<double> getParameters() const override { return {}; }
    size_t getClassicalBit() const { return classical_bit_; }
private:
    size_t qubit_;
    size_t classical_bit_;
};

class ResetGate : public SingleQubitGate {
public:
    explicit ResetGate(size_t qubit) : SingleQubitGate(qubit) {}
    GateType getType() const override { return GateType::RESET; }
    std::vector<double> getParameters() const override { return {}; }
};

// Custom gate
class CustomGate : public QuantumGate {
public:
    CustomGate(const std::string& name, 
              const std::vector<std::vector<std::complex<double>>>& matrix,
              const std::vector<size_t>& qubits,
              const std::vector<double>& params = {})
        : name_(name), matrix_(matrix), qubits_(qubits), parameters_(params) {}
    
    GateType getType() const override { return GateType::CUSTOM; }
    std::vector<size_t> getQubits() const override { return qubits_; }
    std::vector<double> getParameters() const override { return parameters_; }
    std::string getName() const { return name_; }
    const std::vector<std::vector<std::complex<double>>>& getMatrix() const { return matrix_; }
private:
    std::string name_;
    std::vector<std::vector<std::complex<double>>> matrix_;
    std::vector<size_t> qubits_;
    std::vector<double> parameters_;
};

} // namespace qasm

#endif // OPENQASM_QUANTUM_GATES_HPP 