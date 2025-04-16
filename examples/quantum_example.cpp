#include "OpenQASMCompiler/quantum_simulator.hpp"
#include "OpenQASMCompiler/quantum_gates.hpp"
#include <iostream>
#include <iomanip>

using namespace qasm;

void printState(const std::vector<std::complex<double>>& state) {
    std::cout << "Quantum State:" << std::endl;
    for (size_t i = 0; i < state.size(); ++i) {
        std::cout << "|" << std::bitset<3>(i) << "⟩: "
                  << std::fixed << std::setprecision(4)
                  << state[i].real() << " + " << state[i].imag() << "i"
                  << " (Probability: " << std::norm(state[i]) << ")"
                  << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    // Create a 3-qubit quantum simulator
    QuantumSimulator simulator(3);
    
    std::cout << "Initial state:" << std::endl;
    printState(simulator.getState());
    
    // Create a Bell state between qubits 0 and 1
    HGate h(0);
    CNOTGate cnot(0, 1);
    
    simulator.applyGate(h);
    simulator.applyGate(cnot);
    
    std::cout << "After creating Bell state:" << std::endl;
    printState(simulator.getState());
    
    // Apply a Toffoli gate
    CCXGate toffoli(0, 1, 2);
    simulator.applyGate(toffoli);
    
    std::cout << "After applying Toffoli gate:" << std::endl;
    printState(simulator.getState());
    
    // Measure qubit 0
    bool result = simulator.measure(0);
    std::cout << "Measurement result for qubit 0: " << result << std::endl;
    printState(simulator.getState());
    
    // Create a GHZ state
    simulator.reset();
    simulator.applyGate(h);
    simulator.applyGate(cnot);
    CNOTGate cnot2(1, 2);
    simulator.applyGate(cnot2);
    
    std::cout << "GHZ state:" << std::endl;
    printState(simulator.getState());
    
    // Test parameterized gates
    simulator.reset();
    RXGate rx(0, M_PI/4);
    RYGate ry(1, M_PI/2);
    RZGate rz(2, M_PI);
    
    simulator.applyGate(rx);
    simulator.applyGate(ry);
    simulator.applyGate(rz);
    
    std::cout << "After applying parameterized gates:" << std::endl;
    printState(simulator.getState());
    
    // Test expectation values
    std::cout << "Expectation values:" << std::endl;
    std::cout << "⟨X⟩ on qubit 0: " << simulator.getExpectationValue('X', 0) << std::endl;
    std::cout << "⟨Y⟩ on qubit 1: " << simulator.getExpectationValue('Y', 1) << std::endl;
    std::cout << "⟨Z⟩ on qubit 2: " << simulator.getExpectationValue('Z', 2) << std::endl;
    
    return 0;
} 