#ifndef QUANTUM_DEBUGGER_HPP
#define QUANTUM_DEBUGGER_HPP

#include <vector>
#include <string>
#include <functional>
#include "quantum_circuit.hpp"
#include "quantum_state.hpp"

namespace qasm {

class QuantumDebugger {
public:
    enum class BreakpointType {
        GATE,        // Break at specific gate
        STATE,       // Break based on state condition
        PROBABILITY, // Break based on probability threshold
        CUSTOM       // Custom break condition
    };

    struct Breakpoint {
        BreakpointType type;
        std::function<bool(const QuantumState&)> condition;
        std::string description;
    };

    QuantumDebugger(QuantumCircuit& circuit, QuantumState& state);
    ~QuantumDebugger();

    // Breakpoint management
    void addGateBreakpoint(int gate_index);
    void addStateBreakpoint(
        const std::function<bool(const QuantumState&)>& condition,
        const std::string& description
    );
    void addProbabilityBreakpoint(int qubit, double threshold);
    void addCustomBreakpoint(
        const std::function<bool(const QuantumState&)>& condition,
        const std::string& description
    );
    void removeBreakpoint(int index);
    void clearBreakpoints();

    // Execution control
    void step();
    void continueExecution();
    void reset();

    // State analysis
    double getProbability(int qubit, bool value) const;
    double getEntanglement(int qubit1, int qubit2) const;
    std::vector<double> getStateProbabilities() const;
    std::string getStateInfo() const;

private:
    QuantumCircuit& circuit;
    QuantumState& state;
    std::vector<Breakpoint> breakpoints;
    int current_gate_index;
    bool is_running;

    // Helper functions
    bool checkBreakpoints() const;
    void updateStateInfo();
    std::string formatStateInfo() const;
};

} // namespace qasm

#endif // QUANTUM_DEBUGGER_HPP 