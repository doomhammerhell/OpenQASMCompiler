#include "quantum_debugger.hpp"
#include <cmath>
#include <sstream>
#include <iomanip>

namespace qasm {

QuantumDebugger::QuantumDebugger(QuantumCircuit& circuit, QuantumState& state)
    : circuit(circuit), state(state), current_gate_index(0), is_running(false) {}

QuantumDebugger::~QuantumDebugger() {}

void QuantumDebugger::addGateBreakpoint(int gate_index) {
    Breakpoint bp;
    bp.type = BreakpointType::GATE;
    bp.condition = [gate_index, this](const QuantumState&) {
        return this->current_gate_index == gate_index;
    };
    bp.description = "Break at gate " + std::to_string(gate_index);
    breakpoints.push_back(bp);
}

void QuantumDebugger::addStateBreakpoint(
    const std::function<bool(const QuantumState&)>& condition,
    const std::string& description
) {
    Breakpoint bp;
    bp.type = BreakpointType::STATE;
    bp.condition = condition;
    bp.description = description;
    breakpoints.push_back(bp);
}

void QuantumDebugger::addProbabilityBreakpoint(int qubit, double threshold) {
    Breakpoint bp;
    bp.type = BreakpointType::PROBABILITY;
    bp.condition = [qubit, threshold, this](const QuantumState& state) {
        return getProbability(qubit, true) > threshold;
    };
    bp.description = "Probability of qubit " + std::to_string(qubit) + 
                    " > " + std::to_string(threshold);
    breakpoints.push_back(bp);
}

void QuantumDebugger::addCustomBreakpoint(
    const std::function<bool(const QuantumState&)>& condition,
    const std::string& description
) {
    Breakpoint bp;
    bp.type = BreakpointType::CUSTOM;
    bp.condition = condition;
    bp.description = description;
    breakpoints.push_back(bp);
}

void QuantumDebugger::removeBreakpoint(int index) {
    if (index >= 0 && index < breakpoints.size()) {
        breakpoints.erase(breakpoints.begin() + index);
    }
}

void QuantumDebugger::clearBreakpoints() {
    breakpoints.clear();
}

void QuantumDebugger::step() {
    if (current_gate_index < circuit.getGates().size()) {
        const auto& gate = circuit.getGates()[current_gate_index];
        circuit.execute(state);
        current_gate_index++;
        updateStateInfo();
    }
}

void QuantumDebugger::continueExecution() {
    is_running = true;
    while (is_running && current_gate_index < circuit.getGates().size()) {
        step();
        if (checkBreakpoints()) {
            is_running = false;
        }
    }
}

void QuantumDebugger::reset() {
    current_gate_index = 0;
    state = QuantumState(state.getNumQubits());
    updateStateInfo();
}

double QuantumDebugger::getProbability(int qubit, bool value) const {
    double prob = 0.0;
    const int mask = 1 << qubit;
    for (size_t i = 0; i < state.getState().size(); ++i) {
        if (((i & mask) == 0) == !value) {
            prob += std::norm(state.getState()[i]);
        }
    }
    return prob;
}

double QuantumDebugger::getEntanglement(int qubit1, int qubit2) const {
    // Calculate entanglement using concurrence
    double concurrence = 0.0;
    const int mask1 = 1 << qubit1;
    const int mask2 = 1 << qubit2;
    
    for (size_t i = 0; i < state.getState().size(); ++i) {
        for (size_t j = 0; j < state.getState().size(); ++j) {
            if ((i ^ mask1 ^ mask2) == j) {
                concurrence += std::abs(state.getState()[i] * state.getState()[j]);
            }
        }
    }
    
    return concurrence;
}

std::vector<double> QuantumDebugger::getStateProbabilities() const {
    std::vector<double> probs(state.getState().size());
    for (size_t i = 0; i < state.getState().size(); ++i) {
        probs[i] = std::norm(state.getState()[i]);
    }
    return probs;
}

std::string QuantumDebugger::getStateInfo() const {
    return formatStateInfo();
}

bool QuantumDebugger::checkBreakpoints() const {
    for (const auto& bp : breakpoints) {
        if (bp.condition(state)) {
            return true;
        }
    }
    return false;
}

void QuantumDebugger::updateStateInfo() {
    // Update any cached state information
}

std::string QuantumDebugger::formatStateInfo() const {
    std::stringstream ss;
    ss << "Current Gate: " << current_gate_index << "\n";
    ss << "State Probabilities:\n";
    
    const auto& state_vector = state.getState();
    for (size_t i = 0; i < state_vector.size(); ++i) {
        ss << "|" << std::bitset<32>(i).to_string().substr(32 - state.getNumQubits())
           << "⟩: " << std::fixed << std::setprecision(4)
           << std::norm(state_vector[i]) << "\n";
    }
    
    return ss.str();
}

} // namespace qasm 