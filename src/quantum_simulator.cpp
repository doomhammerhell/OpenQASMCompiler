#include "OpenQASMCompiler/quantum_simulator.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <iomanip>

namespace qasm {

QuantumSimulator::QuantumSimulator(size_t num_qubits, 
                                 NoiseModel noise_model,
                                 double noise_parameter)
    : num_qubits_(num_qubits),
      state_vector_(1 << num_qubits, 0.0),
      rng_(std::random_device{}()),
      dist_(0.0, 1.0),
      noise_model_(noise_model),
      noise_parameter_(noise_parameter) {
    initializeState();
}

void QuantumSimulator::initializeState() {
    state_vector_[0] = 1.0;  // Initialize to |0⟩ state
    for (size_t i = 1; i < state_vector_.size(); ++i) {
        state_vector_[i] = 0.0;
    }
}

void QuantumSimulator::simulate(const QuantumCircuit& circuit, size_t shots) {
    for (const auto& gate : circuit.getGates()) {
        switch (gate->getType()) {
            case GateType::X:
            case GateType::Y:
            case GateType::Z:
            case GateType::H:
            case GateType::S:
            case GateType::SDag:
            case GateType::T:
            case GateType::TDag:
                applySingleQubitGate(static_cast<const SingleQubitGate&>(*gate));
                break;
            case GateType::CNOT:
            case GateType::SWAP:
            case GateType::CP:
            case GateType::CRX:
            case GateType::CRY:
            case GateType::CRZ:
            case GateType::CU1:
            case GateType::CU2:
            case GateType::CU3:
            case GateType::ISWAP:
            case GateType::SQISWAP:
                applyTwoQubitGate(static_cast<const TwoQubitGate&>(*gate));
                break;
            case GateType::CCX:
            case GateType::CCZ:
            case GateType::CSWAP:
                applyThreeQubitGate(static_cast<const ThreeQubitGate&>(*gate));
                break;
            case GateType::U1:
            case GateType::U2:
            case GateType::U3:
                applyParameterizedGate(static_cast<const ParameterizedGate&>(*gate));
                break;
            default:
                throw std::runtime_error("Unsupported gate type");
        }

        // Apply noise after each gate
        if (noise_model_ != NoiseModel::NONE) {
            for (size_t qubit : gate->getQubits()) {
                applyNoise(qubit);
            }
        }
    }
    normalizeState();
}

void QuantumSimulator::applySingleQubitGate(const SingleQubitGate& gate) {
    size_t target = gate.getQubits()[0];
    size_t mask = 1 << target;
    size_t size = state_vector_.size();
    
    std::vector<std::complex<double>> new_state = state_vector_;
    
    for (size_t i = 0; i < size; ++i) {
        if ((i & mask) == 0) {
            size_t j = i | mask;
            std::complex<double> a = state_vector_[i];
            std::complex<double> b = state_vector_[j];
            
            // Apply gate matrix
            switch (gate.getType()) {
                case GateType::X:
                    new_state[i] = b;
                    new_state[j] = a;
                    break;
                case GateType::Y:
                    new_state[i] = std::complex<double>(0, 1) * b;
                    new_state[j] = std::complex<double>(0, -1) * a;
                    break;
                case GateType::Z:
                    new_state[i] = a;
                    new_state[j] = -b;
                    break;
                case GateType::H:
                    new_state[i] = (a + b) / std::sqrt(2.0);
                    new_state[j] = (a - b) / std::sqrt(2.0);
                    break;
                case GateType::S:
                    new_state[i] = a;
                    new_state[j] = std::complex<double>(0, 1) * b;
                    break;
                case GateType::SDag:
                    new_state[i] = a;
                    new_state[j] = std::complex<double>(0, -1) * b;
                    break;
                case GateType::T:
                    new_state[i] = a;
                    new_state[j] = std::complex<double>(1.0 / std::sqrt(2.0), 1.0 / std::sqrt(2.0)) * b;
                    break;
                case GateType::TDag:
                    new_state[i] = a;
                    new_state[j] = std::complex<double>(1.0 / std::sqrt(2.0), -1.0 / std::sqrt(2.0)) * b;
                    break;
                default:
                    throw std::runtime_error("Unsupported single-qubit gate");
            }
        }
    }
    
    state_vector_ = std::move(new_state);
}

void QuantumSimulator::applyTwoQubitGate(const TwoQubitGate& gate) {
    const auto& qubits = gate.getQubits();
    size_t control = qubits[0];
    size_t target = qubits[1];
    size_t control_mask = 1 << control;
    size_t target_mask = 1 << target;
    size_t size = state_vector_.size();
    
    std::vector<std::complex<double>> new_state = state_vector_;
    
    for (size_t i = 0; i < size; ++i) {
        if ((i & control_mask) != 0) {  // Control qubit is |1⟩
            size_t j = i ^ target_mask;  // Flip target qubit
            std::complex<double> a = state_vector_[i];
            std::complex<double> b = state_vector_[j];
            
            // Apply gate matrix
            switch (gate.getType()) {
                case GateType::CNOT:
                    new_state[i] = b;
                    new_state[j] = a;
                    break;
                case GateType::SWAP:
                    new_state[i] = b;
                    new_state[j] = a;
                    break;
                case GateType::CP:
                    new_state[i] = a;
                    new_state[j] = std::complex<double>(0, 1) * b;
                    break;
                // Add other two-qubit gates here
                default:
                    throw std::runtime_error("Unsupported two-qubit gate");
            }
        }
    }
    
    state_vector_ = std::move(new_state);
}

void QuantumSimulator::applyThreeQubitGate(const ThreeQubitGate& gate) {
    const auto& qubits = gate.getQubits();
    size_t control1 = qubits[0];
    size_t control2 = qubits[1];
    size_t target = qubits[2];
    size_t control1_mask = 1 << control1;
    size_t control2_mask = 1 << control2;
    size_t target_mask = 1 << target;
    size_t size = state_vector_.size();
    
    std::vector<std::complex<double>> new_state = state_vector_;
    
    for (size_t i = 0; i < size; ++i) {
        if ((i & control1_mask) != 0 && (i & control2_mask) != 0) {  // Both control qubits are |1⟩
            size_t j = i ^ target_mask;  // Flip target qubit
            std::complex<double> a = state_vector_[i];
            std::complex<double> b = state_vector_[j];
            
            // Apply gate matrix
            switch (gate.getType()) {
                case GateType::CCX:
                    new_state[i] = b;
                    new_state[j] = a;
                    break;
                case GateType::CCZ:
                    new_state[i] = a;
                    new_state[j] = -b;
                    break;
                case GateType::CSWAP:
                    new_state[i] = b;
                    new_state[j] = a;
                    break;
                default:
                    throw std::runtime_error("Unsupported three-qubit gate");
            }
        }
    }
    
    state_vector_ = std::move(new_state);
}

void QuantumSimulator::applyParameterizedGate(const ParameterizedGate& gate) {
    const auto& qubits = gate.getQubits();
    const auto& params = gate.getParameters();
    size_t target = qubits[0];
    size_t mask = 1 << target;
    size_t size = state_vector_.size();
    
    std::vector<std::complex<double>> new_state = state_vector_;
    
    for (size_t i = 0; i < size; ++i) {
        if ((i & mask) == 0) {
            size_t j = i | mask;
            std::complex<double> a = state_vector_[i];
            std::complex<double> b = state_vector_[j];
            
            // Apply parameterized gate matrix
            switch (gate.getType()) {
                case GateType::U1:
                    new_state[i] = a;
                    new_state[j] = std::exp(std::complex<double>(0, params[0])) * b;
                    break;
                case GateType::U2:
                    new_state[i] = (a + std::exp(std::complex<double>(0, params[1])) * b) / std::sqrt(2.0);
                    new_state[j] = (std::exp(std::complex<double>(0, params[0])) * a - 
                                  std::exp(std::complex<double>(0, params[0] + params[1])) * b) / std::sqrt(2.0);
                    break;
                case GateType::U3:
                    new_state[i] = std::cos(params[0]/2) * a - 
                                 std::exp(std::complex<double>(0, params[2])) * std::sin(params[0]/2) * b;
                    new_state[j] = std::exp(std::complex<double>(0, params[1])) * std::sin(params[0]/2) * a + 
                                 std::exp(std::complex<double>(0, params[1] + params[2])) * std::cos(params[0]/2) * b;
                    break;
                default:
                    throw std::runtime_error("Unsupported parameterized gate");
            }
        }
    }
    
    state_vector_ = std::move(new_state);
}

bool QuantumSimulator::measure(size_t qubit_index) {
    if (qubit_index >= num_qubits_) {
        throw std::out_of_range("Qubit index out of range");
    }
    
    double probability = 0.0;
    size_t mask = 1 << qubit_index;
    
    // Calculate probability of measuring |1⟩
    for (size_t i = 0; i < state_vector_.size(); ++i) {
        if ((i & mask) != 0) {
            probability += std::norm(state_vector_[i]);
        }
    }
    
    // Perform measurement
    bool result = (dist_(rng_) < probability);
    
    // Collapse state
    for (size_t i = 0; i < state_vector_.size(); ++i) {
        if ((i & mask) != 0) {
            state_vector_[i] = result ? state_vector_[i] / std::sqrt(probability) : 0.0;
        } else {
            state_vector_[i] = !result ? state_vector_[i] / std::sqrt(1.0 - probability) : 0.0;
        }
    }
    
    return result;
}

std::vector<bool> QuantumSimulator::measureAll() {
    std::vector<bool> results(num_qubits_);
    for (size_t i = 0; i < num_qubits_; ++i) {
        results[i] = measure(i);
    }
    return results;
}

double QuantumSimulator::getProbability(const std::vector<bool>& state) const {
    if (state.size() != num_qubits_) {
        throw std::invalid_argument("State size does not match number of qubits");
    }
    
    size_t index = 0;
    for (size_t i = 0; i < state.size(); ++i) {
        if (state[i]) {
            index |= (1 << i);
        }
    }
    
    return std::norm(state_vector_[index]);
}

double QuantumSimulator::getExpectationValue(
    const std::vector<std::vector<std::complex<double>>>& observable) const {
    if (observable.size() != state_vector_.size() || 
        observable[0].size() != state_vector_.size()) {
        throw std::invalid_argument("Observable size does not match state vector size");
    }
    
    std::complex<double> expectation = 0.0;
    for (size_t i = 0; i < state_vector_.size(); ++i) {
        for (size_t j = 0; j < state_vector_.size(); ++j) {
            expectation += std::conj(state_vector_[i]) * observable[i][j] * state_vector_[j];
        }
    }
    
    return std::real(expectation);
}

void QuantumSimulator::reset() {
    initializeState();
}

void QuantumSimulator::normalizeState() {
    double norm = 0.0;
    for (const auto& amplitude : state_vector_) {
        norm += std::norm(amplitude);
    }
    norm = std::sqrt(norm);
    
    for (auto& amplitude : state_vector_) {
        amplitude /= norm;
    }
}

const std::vector<std::complex<double>>& QuantumSimulator::getStateVector() const {
    return state_vector_;
}

void QuantumSimulator::applyNoise(size_t qubit) {
    switch (noise_model_) {
        case NoiseModel::DEPOLARIZING:
            applyDepolarizingNoise(qubit);
            break;
        case NoiseModel::AMPLITUDE_DAMPING:
            applyAmplitudeDampingNoise(qubit);
            break;
        case NoiseModel::PHASE_DAMPING:
            applyPhaseDampingNoise(qubit);
            break;
        case NoiseModel::BIT_FLIP:
            applyBitFlipNoise(qubit);
            break;
        case NoiseModel::PHASE_FLIP:
            applyPhaseFlipNoise(qubit);
            break;
        case NoiseModel::BIT_PHASE_FLIP:
            applyBitPhaseFlipNoise(qubit);
            break;
        default:
            break;
    }
}

void QuantumSimulator::applyDepolarizingNoise(size_t qubit) {
    double p = noise_parameter_;
    if (dist_(rng_) < p) {
        double r = dist_(rng_);
        if (r < 1.0/3.0) {
            // Apply X
            applySingleQubitGate(XGate(qubit));
        } else if (r < 2.0/3.0) {
            // Apply Y
            applySingleQubitGate(YGate(qubit));
        } else {
            // Apply Z
            applySingleQubitGate(ZGate(qubit));
        }
    }
}

void QuantumSimulator::applyAmplitudeDampingNoise(size_t qubit) {
    double gamma = noise_parameter_;
    if (dist_(rng_) < gamma) {
        // Apply amplitude damping
        size_t mask = 1 << qubit;
        for (size_t i = 0; i < state_vector_.size(); ++i) {
            if (i & mask) {
                state_vector_[i] = 0.0;
            }
        }
    }
}

void QuantumSimulator::applyPhaseDampingNoise(size_t qubit) {
    double lambda = noise_parameter_;
    if (dist_(rng_) < lambda) {
        // Apply phase damping
        size_t mask = 1 << qubit;
        for (size_t i = 0; i < state_vector_.size(); ++i) {
            if (i & mask) {
                state_vector_[i] *= -1.0;
            }
        }
    }
}

void QuantumSimulator::applyBitFlipNoise(size_t qubit) {
    double p = noise_parameter_;
    if (dist_(rng_) < p) {
        applySingleQubitGate(XGate(qubit));
    }
}

void QuantumSimulator::applyPhaseFlipNoise(size_t qubit) {
    double p = noise_parameter_;
    if (dist_(rng_) < p) {
        applySingleQubitGate(ZGate(qubit));
    }
}

void QuantumSimulator::applyBitPhaseFlipNoise(size_t qubit) {
    double p = noise_parameter_;
    if (dist_(rng_) < p) {
        applySingleQubitGate(YGate(qubit));
    }
}

std::unordered_map<std::string, size_t> QuantumSimulator::getMeasurementStats(size_t shots) {
    std::unordered_map<std::string, size_t> stats;
    for (size_t i = 0; i < shots; ++i) {
        std::vector<bool> result = measureAll();
        std::string state_str = stateToString(result);
        stats[state_str]++;
    }
    return stats;
}

std::vector<std::vector<std::complex<double>>> QuantumSimulator::getDensityMatrix() const {
    size_t size = state_vector_.size();
    std::vector<std::vector<std::complex<double>>> rho(size, 
        std::vector<std::complex<double>>(size, 0.0));
    
    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < size; ++j) {
            rho[i][j] = state_vector_[i] * std::conj(state_vector_[j]);
        }
    }
    
    return rho;
}

void QuantumSimulator::setNoiseModel(NoiseModel model, double parameter) {
    noise_model_ = model;
    noise_parameter_ = parameter;
}

std::string QuantumSimulator::stateToString(const std::vector<bool>& state) const {
    std::stringstream ss;
    for (bool bit : state) {
        ss << (bit ? '1' : '0');
    }
    return ss.str();
}

} // namespace qasm 