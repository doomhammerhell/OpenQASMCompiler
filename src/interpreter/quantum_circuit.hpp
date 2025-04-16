#ifndef QUANTUM_CIRCUIT_HPP
#define QUANTUM_CIRCUIT_HPP

#include <vector>
#include <string>
#include <memory>
#include "quantum_state.hpp"

namespace qasm {

class QuantumCircuit {
public:
    enum class GateType {
        H,      // Hadamard
        X,      // Pauli-X
        Y,      // Pauli-Y
        Z,      // Pauli-Z
        CNOT,   // Controlled-NOT
        SWAP,   // SWAP
        CZ,     // Controlled-Z
        TOFFOLI,// Toffoli
        FREDKIN,// Fredkin
        RX,     // Rotation X
        RY,     // Rotation Y
        RZ,     // Rotation Z
        MEASURE // Measurement
    };

    struct Gate {
        GateType type;
        std::vector<int> qubits;
        double angle; // For rotation gates
    };

    QuantumCircuit(int num_qubits);
    ~QuantumCircuit();

    // Add gates
    void addGate(GateType type, const std::vector<int>& qubits, double angle = 0.0);
    
    // Execute circuit
    void execute(QuantumState& state);
    
    // Visualization
    std::string toQASM() const;
    std::string toLatex() const;
    std::string toASCII() const;
    
    // Access
    int getNumQubits() const { return num_qubits; }
    const std::vector<Gate>& getGates() const { return gates; }

private:
    int num_qubits;
    std::vector<Gate> gates;
    
    // Helper functions
    std::string gateTypeToString(GateType type) const;
    std::string gateToQASM(const Gate& gate) const;
    std::string gateToLatex(const Gate& gate) const;
    std::string gateToASCII(const Gate& gate) const;
};

} // namespace qasm

#endif // QUANTUM_CIRCUIT_HPP 