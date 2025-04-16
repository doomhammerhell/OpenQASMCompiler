#ifndef OPENQASM_CIRCUIT_OPTIMIZER_HPP
#define OPENQASM_CIRCUIT_OPTIMIZER_HPP

#include "quantum_circuit.hpp"
#include "quantum_gates.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace qasm {

/**
 * @brief Class for optimizing quantum circuits.
 * 
 * This class provides various optimization strategies for quantum circuits,
 * including gate cancellation, merging, and circuit depth optimization.
 */
class CircuitOptimizer {
public:
    /**
     * @brief Optimizes a quantum circuit using various strategies.
     * 
     * @param circuit The circuit to optimize.
     */
    static void optimize(QuantumCircuit& circuit);

    /**
     * @brief Optimizes the circuit depth by reordering gates.
     * 
     * @param circuit The circuit to optimize.
     */
    static void optimizeDepth(QuantumCircuit& circuit);

    /**
     * @brief Optimizes the gate count by removing redundant gates.
     * 
     * @param circuit The circuit to optimize.
     */
    static void optimizeGateCount(QuantumCircuit& circuit);

    /**
     * @brief Optimizes the qubit mapping for better hardware utilization.
     * 
     * @param circuit The circuit to optimize.
     */
    static void optimizeQubitMapping(QuantumCircuit& circuit);

private:
    /**
     * @brief Checks if two gates can be cancelled out.
     * 
     * @param gate1 The first gate.
     * @param gate2 The second gate.
     * @return true if the gates cancel each other out.
     */
    static bool canCancelGates(const QuantumGate& gate1, const QuantumGate& gate2);

    /**
     * @brief Checks if two gates can be merged.
     * 
     * @param gate1 The first gate.
     * @param gate2 The second gate.
     * @return true if the gates can be merged.
     */
    static bool canMergeGates(const QuantumGate& gate1, const QuantumGate& gate2);

    /**
     * @brief Merges two gates into a single gate.
     * 
     * @param gate1 The first gate.
     * @param gate2 The second gate.
     * @return A new gate that represents the merged operation.
     */
    static std::unique_ptr<QuantumGate> mergeGates(const QuantumGate& gate1, const QuantumGate& gate2);

    /**
     * @brief Checks if two gates can be commuted.
     * 
     * @param gate1 The first gate.
     * @param gate2 The second gate.
     * @return true if the gates can be commuted.
     */
    static bool canCommuteGates(const QuantumGate& gate1, const QuantumGate& gate2);

    /**
     * @brief Applies gate cancellation rules to the circuit.
     * 
     * @param circuit The circuit to optimize.
     */
    static void applyCancellationRules(QuantumCircuit& circuit);

    /**
     * @brief Applies gate merging rules to the circuit.
     * 
     * @param circuit The circuit to optimize.
     */
    static void applyMergingRules(QuantumCircuit& circuit);

    /**
     * @brief Applies gate commutation rules to the circuit.
     * 
     * @param circuit The circuit to optimize.
     */
    static void applyCommutationRules(QuantumCircuit& circuit);

    /**
     * @brief Reorders gates to minimize circuit depth.
     * 
     * @param circuit The circuit to optimize.
     */
    static void reorderGates(QuantumCircuit& circuit);

    /**
     * @brief Maps qubits to optimize hardware utilization.
     * 
     * @param circuit The circuit to optimize.
     */
    static void remapQubits(QuantumCircuit& circuit);

    // Gate cancellation rules
    static const std::unordered_map<GateType, GateType> cancellation_pairs;
    
    // Gate merging rules
    static const std::unordered_map<GateType, std::unordered_map<GateType, GateType>> merging_rules;
    
    // Gate commutation rules
    static const std::unordered_map<GateType, std::vector<GateType>> commutation_rules;
};

} // namespace qasm

#endif // OPENQASM_CIRCUIT_OPTIMIZER_HPP 