#include "OpenQASMCompiler/circuit_optimizer.hpp"
#include <algorithm>
#include <cmath>

namespace qasm {

// Initialize static members
const std::unordered_map<GateType, GateType> CircuitOptimizer::cancellation_pairs = {
    {GateType::X, GateType::X},
    {GateType::Y, GateType::Y},
    {GateType::Z, GateType::Z},
    {GateType::H, GateType::H},
    {GateType::S, GateType::S_DAG},
    {GateType::S_DAG, GateType::S},
    {GateType::T, GateType::T_DAG},
    {GateType::T_DAG, GateType::T}
};

const std::unordered_map<GateType, std::unordered_map<GateType, GateType>> CircuitOptimizer::merging_rules = {
    {GateType::RX, {
        {GateType::RX, GateType::RX}
    }},
    {GateType::RY, {
        {GateType::RY, GateType::RY}
    }},
    {GateType::RZ, {
        {GateType::RZ, GateType::RZ}
    }},
    {GateType::P, {
        {GateType::P, GateType::P}
    }}
};

const std::unordered_map<GateType, std::vector<GateType>> CircuitOptimizer::commutation_rules = {
    {GateType::X, {GateType::Z}},
    {GateType::Z, {GateType::X}},
    {GateType::H, {GateType::X, GateType::Z}},
    {GateType::S, {GateType::X}},
    {GateType::T, {GateType::X}}
};

void CircuitOptimizer::optimize(QuantumCircuit& circuit) {
    // Apply optimization passes in sequence
    applyCancellationRules(circuit);
    applyMergingRules(circuit);
    applyCommutationRules(circuit);
    optimizeDepth(circuit);
    optimizeQubitMapping(circuit);
}

void CircuitOptimizer::optimizeDepth(QuantumCircuit& circuit) {
    reorderGates(circuit);
}

void CircuitOptimizer::optimizeGateCount(QuantumCircuit& circuit) {
    applyCancellationRules(circuit);
    applyMergingRules(circuit);
}

void CircuitOptimizer::optimizeQubitMapping(QuantumCircuit& circuit) {
    remapQubits(circuit);
}

bool CircuitOptimizer::canCancelGates(const QuantumGate& gate1, const QuantumGate& gate2) {
    if (gate1.getType() != gate2.getType()) {
        return false;
    }

    // Check if the gates operate on the same qubits
    auto qubits1 = gate1.getQubits();
    auto qubits2 = gate2.getQubits();
    if (qubits1 != qubits2) {
        return false;
    }

    // Check if the gates are in the cancellation pairs
    auto it = cancellation_pairs.find(gate1.getType());
    return it != cancellation_pairs.end() && it->second == gate2.getType();
}

bool CircuitOptimizer::canMergeGates(const QuantumGate& gate1, const QuantumGate& gate2) {
    if (gate1.getType() != gate2.getType()) {
        return false;
    }

    // Check if the gates operate on the same qubits
    auto qubits1 = gate1.getQubits();
    auto qubits2 = gate2.getQubits();
    if (qubits1 != qubits2) {
        return false;
    }

    // Check if the gates are in the merging rules
    auto it = merging_rules.find(gate1.getType());
    if (it == merging_rules.end()) {
        return false;
    }

    return it->second.find(gate2.getType()) != it->second.end();
}

std::unique_ptr<QuantumGate> CircuitOptimizer::mergeGates(const QuantumGate& gate1, const QuantumGate& gate2) {
    if (!canMergeGates(gate1, gate2)) {
        return nullptr;
    }

    auto params1 = gate1.getParameters();
    auto params2 = gate2.getParameters();
    std::vector<double> merged_params;

    // Merge parameters based on gate type
    switch (gate1.getType()) {
        case GateType::RX:
        case GateType::RY:
        case GateType::RZ:
        case GateType::P:
            merged_params.push_back(params1[0] + params2[0]);
            break;
        default:
            return nullptr;
    }

    // Create new gate with merged parameters
    switch (gate1.getType()) {
        case GateType::RX:
            return std::make_unique<RXGate>(gate1.getQubits()[0], merged_params[0]);
        case GateType::RY:
            return std::make_unique<RYGate>(gate1.getQubits()[0], merged_params[0]);
        case GateType::RZ:
            return std::make_unique<RZGate>(gate1.getQubits()[0], merged_params[0]);
        case GateType::P:
            return std::make_unique<PhaseGate>(gate1.getQubits()[0], merged_params[0]);
        default:
            return nullptr;
    }
}

bool CircuitOptimizer::canCommuteGates(const QuantumGate& gate1, const QuantumGate& gate2) {
    // Check if gates operate on different qubits
    auto qubits1 = gate1.getQubits();
    auto qubits2 = gate2.getQubits();
    for (auto q1 : qubits1) {
        for (auto q2 : qubits2) {
            if (q1 == q2) {
                return false;
            }
        }
    }

    // Check if gates are in commutation rules
    auto it = commutation_rules.find(gate1.getType());
    if (it == commutation_rules.end()) {
        return false;
    }

    return std::find(it->second.begin(), it->second.end(), gate2.getType()) != it->second.end();
}

void CircuitOptimizer::applyCancellationRules(QuantumCircuit& circuit) {
    auto& gates = circuit.getGates();
    for (size_t i = 0; i < gates.size(); ++i) {
        for (size_t j = i + 1; j < gates.size(); ++j) {
            if (canCancelGates(*gates[i], *gates[j])) {
                // Remove both gates
                gates.erase(gates.begin() + j);
                gates.erase(gates.begin() + i);
                --i; // Adjust index after removal
                break;
            }
        }
    }
}

void CircuitOptimizer::applyMergingRules(QuantumCircuit& circuit) {
    auto& gates = circuit.getGates();
    for (size_t i = 0; i < gates.size(); ++i) {
        for (size_t j = i + 1; j < gates.size(); ++j) {
            if (canMergeGates(*gates[i], *gates[j])) {
                auto merged_gate = mergeGates(*gates[i], *gates[j]);
                if (merged_gate) {
                    // Replace the two gates with the merged gate
                    gates[i] = std::move(merged_gate);
                    gates.erase(gates.begin() + j);
                    --i; // Adjust index after removal
                    break;
                }
            }
        }
    }
}

void CircuitOptimizer::applyCommutationRules(QuantumCircuit& circuit) {
    auto& gates = circuit.getGates();
    for (size_t i = 0; i < gates.size() - 1; ++i) {
        if (canCommuteGates(*gates[i], *gates[i + 1])) {
            // Swap the gates
            std::swap(gates[i], gates[i + 1]);
        }
    }
}

void CircuitOptimizer::reorderGates(QuantumCircuit& circuit) {
    auto& gates = circuit.getGates();
    
    // Group gates by qubit
    std::vector<std::vector<size_t>> qubit_gates(circuit.getNumQubits());
    for (size_t i = 0; i < gates.size(); ++i) {
        for (auto qubit : gates[i]->getQubits()) {
            qubit_gates[qubit].push_back(i);
        }
    }

    // Reorder gates to minimize depth
    std::vector<bool> used(gates.size(), false);
    std::vector<std::unique_ptr<QuantumGate>> new_gates;
    
    while (new_gates.size() < gates.size()) {
        // Find gates that can be executed in parallel
        std::vector<size_t> current_layer;
        for (size_t i = 0; i < gates.size(); ++i) {
            if (!used[i]) {
                bool can_add = true;
                for (auto qubit : gates[i]->getQubits()) {
                    for (auto gate_idx : qubit_gates[qubit]) {
                        if (gate_idx < i && !used[gate_idx]) {
                            can_add = false;
                            break;
                        }
                    }
                }
                if (can_add) {
                    current_layer.push_back(i);
                    used[i] = true;
                }
            }
        }
        
        // Add gates in the current layer
        for (auto idx : current_layer) {
            new_gates.push_back(std::move(gates[idx]));
        }
    }
    
    gates = std::move(new_gates);
}

void CircuitOptimizer::remapQubits(QuantumCircuit& circuit) {
    // Count gate operations per qubit
    std::vector<size_t> qubit_usage(circuit.getNumQubits(), 0);
    for (const auto& gate : circuit.getGates()) {
        for (auto qubit : gate->getQubits()) {
            ++qubit_usage[qubit];
        }
    }

    // Create a mapping from most used to least used qubits
    std::vector<size_t> qubit_order(circuit.getNumQubits());
    std::iota(qubit_order.begin(), qubit_order.end(), 0);
    std::sort(qubit_order.begin(), qubit_order.end(),
              [&qubit_usage](size_t a, size_t b) {
                  return qubit_usage[a] > qubit_usage[b];
              });

    // Create the remapping
    std::vector<size_t> remapping(circuit.getNumQubits());
    for (size_t i = 0; i < circuit.getNumQubits(); ++i) {
        remapping[qubit_order[i]] = i;
    }

    // Apply the remapping to all gates
    for (auto& gate : circuit.getGates()) {
        auto qubits = gate->getQubits();
        for (auto& qubit : qubits) {
            qubit = remapping[qubit];
        }
    }
}

} // namespace qasm 