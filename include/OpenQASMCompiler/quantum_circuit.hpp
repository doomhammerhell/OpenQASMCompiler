#ifndef OPENQASM_QUANTUM_CIRCUIT_HPP
#define OPENQASM_QUANTUM_CIRCUIT_HPP

#include <vector>
#include <string>
#include <memory>
#include <map>

namespace qasm {

/**
 * @brief Enumeration of supported quantum gate types.
 * 
 * This enum defines all the quantum gates supported by the compiler,
 * including standard gates, parameterized gates, and multi-qubit gates.
 */
enum class GateType {
    // Single-qubit gates
    X,      ///< Pauli-X gate (quantum NOT)
    Y,      ///< Pauli-Y gate
    Z,      ///< Pauli-Z gate
    H,      ///< Hadamard gate
    S,      ///< S gate (√Z)
    S_DAG,  ///< S† gate (inverse of S)
    T,      ///< T gate (π/4 phase)
    T_DAG,  ///< T† gate (inverse of T)
    RX,     ///< Rotation around X axis
    RY,     ///< Rotation around Y axis
    RZ,     ///< Rotation around Z axis
    P,      ///< Phase gate
    U1,     ///< U1 gate (1-parameter unitary)
    U2,     ///< U2 gate (2-parameter unitary)
    U3,     ///< U3 gate (3-parameter unitary)
    
    // Two-qubit gates
    CNOT,   ///< Controlled-NOT gate
    CZ,     ///< Controlled-Z gate
    SWAP,   ///< SWAP gate
    CP,     ///< Controlled phase
    CRX,    ///< Controlled RX
    CRY,    ///< Controlled RY
    CRZ,    ///< Controlled RZ
    CU1,    ///< Controlled U1
    CU2,    ///< Controlled U2
    CU3,    ///< Controlled U3
    ISWAP,  ///< iSWAP gate
    SQISWAP,///< √iSWAP gate
    
    // Three-qubit gates
    CCX,    ///< Toffoli gate (CCNOT)
    CCZ,    ///< Controlled-controlled-Z
    CSWAP,  ///< Controlled SWAP (Fredkin)
    
    // Error correction gates
    SYNC,   ///< Synchronization gate for error correction
    MEASURE,///< Measurement gate
    RESET,  ///< Reset gate
    
    // Custom gates
    CUSTOM  ///< User-defined gate
};

/**
 * @brief Base class for quantum gates.
 * 
 * This abstract class defines the interface for all quantum gates.
 * Each gate must specify its type and the qubits it operates on.
 */
class QuantumGate {
public:
    virtual ~QuantumGate() = default;
    
    /**
     * @brief Gets the type of the gate.
     * 
     * @return GateType The type of the gate.
     */
    virtual GateType getType() const = 0;
    
    /**
     * @brief Gets the qubits affected by the gate.
     * 
     * @return std::vector<size_t> The indices of the affected qubits.
     */
    virtual std::vector<size_t> getQubits() const = 0;
    
    /**
     * @brief Gets the parameters of the gate.
     * 
     * @return std::vector<double> The parameters of the gate.
     */
    virtual std::vector<double> getParameters() const = 0;
};

/**
 * @brief Represents a quantum circuit.
 * 
 * This class manages a collection of quantum gates and measurements,
 * providing methods to add gates, perform measurements, and analyze
 * the circuit properties.
 */
class QuantumCircuit {
public:
    /**
     * @brief Constructs a quantum circuit with the specified number of qubits.
     * 
     * @param num_qubits The number of qubits in the circuit.
     */
    explicit QuantumCircuit(size_t num_qubits);
    
    /**
     * @brief Constructs a quantum circuit with the specified number of qubits and classical bits.
     * 
     * @param num_qubits The number of qubits in the circuit.
     * @param num_classical_bits The number of classical bits in the circuit.
     */
    QuantumCircuit(size_t num_qubits, size_t num_classical_bits);
    ~QuantumCircuit() = default;

    // Gate operations
    /**
     * @brief Adds a gate to the circuit.
     * 
     * @param type The type of the gate.
     * @param qubits The qubits affected by the gate.
     */
    void addGate(GateType type, const std::vector<size_t>& qubits);
    
    /**
     * @brief Adds a parameterized gate to the circuit.
     * 
     * @param type The type of the gate.
     * @param qubits The qubits affected by the gate.
     * @param parameter The parameter value.
     */
    void addGate(GateType type, const std::vector<size_t>& qubits, double parameter);
    
    /**
     * @brief Adds a custom gate to the circuit.
     * 
     * @param name The name of the gate.
     * @param matrix The unitary matrix of the gate.
     * @param qubits The qubits affected by the gate.
     */
    void addCustomGate(const std::string& name,
                      const std::vector<std::vector<std::complex<double>>>& matrix,
                      const std::vector<size_t>& qubits);

    // Measurement
    /**
     * @brief Adds a measurement operation to the circuit.
     * 
     * @param qubits The qubits to measure.
     */
    void addMeasurement(const std::vector<size_t>& qubits);
    
    /**
     * @brief Adds a classically controlled gate to the circuit.
     * 
     * @param type The type of the gate.
     * @param qubits The qubits affected by the gate.
     * @param classical_bits The classical bits controlling the gate.
     */
    void addClassicalControl(GateType type, const std::vector<size_t>& qubits,
                           const std::vector<size_t>& classical_bits);

    // Circuit manipulation
    /**
     * @brief Optimizes the circuit using various strategies.
     */
    void optimize();
    
    /**
     * @brief Decomposes complex gates into simpler ones.
     */
    void decompose();
    
    /**
     * @brief Validates the circuit for correctness.
     * 
     * @throw std::runtime_error if the circuit is invalid.
     */
    void validate() const;
    
    // Parameter management
    /**
     * @brief Updates a gate parameter.
     * 
     * @param param_idx The index of the parameter to update.
     * @param value The new value of the parameter.
     */
    void updateParameter(size_t param_idx, double value);
    
    /**
     * @brief Updates multiple gate parameters.
     * 
     * @param values The new parameter values.
     */
    void updateParameters(const std::vector<double>& values);
    
    // Circuit properties
    /**
     * @brief Gets the number of qubits in the circuit.
     * 
     * @return size_t The number of qubits.
     */
    size_t getNumQubits() const { return num_qubits; }
    
    /**
     * @brief Gets the depth of the circuit.
     * 
     * @return size_t The depth of the circuit.
     */
    size_t getDepth() const;
    
    /**
     * @brief Gets the number of gates in the circuit.
     * 
     * @return size_t The number of gates.
     */
    size_t getGateCount() const;
    
    /**
     * @brief Gets the list of supported gates.
     * 
     * @return std::vector<GateType> The supported gate types.
     */
    std::vector<GateType> getSupportedGates() const;
    
    // Circuit visualization
    /**
     * @brief Converts the circuit to OpenQASM format.
     * 
     * @return std::string The circuit in OpenQASM format.
     */
    std::string toQASM() const;
    
    /**
     * @brief Exports the circuit to LaTeX format.
     * 
     * @param filename The name of the output file.
     */
    void toLatex(const std::string& filename) const;

    /**
     * @brief Gets the list of gates in the circuit.
     * 
     * @return std::vector<std::unique_ptr<QuantumGate>>& The list of gates.
     */
    std::vector<std::unique_ptr<QuantumGate>>& getGates() { return gates; }

    /**
     * @brief Gets the list of gates in the circuit (const version).
     * 
     * @return const std::vector<std::unique_ptr<QuantumGate>>& The list of gates.
     */
    const std::vector<std::unique_ptr<QuantumGate>>& getGates() const { return gates; }

private:
    size_t num_qubits;
    std::vector<std::unique_ptr<QuantumGate>> gates;
    std::map<std::string, std::vector<std::vector<std::complex<double>>>> custom_gates;
    std::vector<std::pair<std::vector<size_t>, std::vector<size_t>>> measurements;
};

} // namespace qasm

#endif // OPENQASM_QUANTUM_CIRCUIT_HPP 