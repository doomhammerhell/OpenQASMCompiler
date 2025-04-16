#include "quantum_state.hpp"
#include <cmath>
#include <random>
#include <algorithm>

namespace qasm {

QuantumState::QuantumState(int num_qubits) 
    : num_qubits(num_qubits), state(1 << num_qubits) {
    state[0] = 1.0; // Initialize to |0⟩
}

QuantumState::~QuantumState() {}

void QuantumState::applySingleQubitGate(int qubit, const std::vector<std::complex<double>>& matrix) {
    const int mask = 1 << qubit;
    const int stride = 1 << (qubit + 1);
    
    for (int i = 0; i < (1 << num_qubits); i += stride) {
        for (int j = 0; j < (1 << qubit); ++j) {
            int idx0 = i + j;
            int idx1 = idx0 + (1 << qubit);
            
            std::complex<double> v0 = state[idx0];
            std::complex<double> v1 = state[idx1];
            
            state[idx0] = matrix[0] * v0 + matrix[1] * v1;
            state[idx1] = matrix[2] * v0 + matrix[3] * v1;
        }
    }
}

void QuantumState::applyTwoQubitGate(int qubit1, int qubit2, 
                                   const std::vector<std::complex<double>>& matrix) {
    if (qubit1 > qubit2) std::swap(qubit1, qubit2);
    
    const int mask1 = 1 << qubit1;
    const int mask2 = 1 << qubit2;
    const int stride = 1 << (qubit2 + 1);
    
    for (int i = 0; i < (1 << num_qubits); i += stride) {
        for (int j = 0; j < (1 << qubit1); ++j) {
            for (int k = 0; k < (1 << (qubit2 - qubit1 - 1)); ++k) {
                int base = i + j + (k << (qubit1 + 1));
                int idx00 = base;
                int idx01 = base + mask1;
                int idx10 = base + mask2;
                int idx11 = base + mask1 + mask2;
                
                std::complex<double> v00 = state[idx00];
                std::complex<double> v01 = state[idx01];
                std::complex<double> v10 = state[idx10];
                std::complex<double> v11 = state[idx11];
                
                state[idx00] = matrix[0] * v00 + matrix[1] * v01 + matrix[2] * v10 + matrix[3] * v11;
                state[idx01] = matrix[4] * v00 + matrix[5] * v01 + matrix[6] * v10 + matrix[7] * v11;
                state[idx10] = matrix[8] * v00 + matrix[9] * v01 + matrix[10] * v10 + matrix[11] * v11;
                state[idx11] = matrix[12] * v00 + matrix[13] * v01 + matrix[14] * v10 + matrix[15] * v11;
            }
        }
    }
}

void QuantumState::simdMatrixMultiply(const std::vector<std::complex<double>>& matrix,
                                    const std::vector<std::complex<double>>& input,
                                    std::vector<std::complex<double>>& output) {
    const int size = input.size();
    const int simd_size = 4; // AVX2 can handle 4 complex doubles at once
    
    for (int i = 0; i < size; i += simd_size) {
        __m256d real_result = _mm256_setzero_pd();
        __m256d imag_result = _mm256_setzero_pd();
        
        for (int j = 0; j < size; ++j) {
            __m256d matrix_real = _mm256_set1_pd(matrix[i * size + j].real());
            __m256d matrix_imag = _mm256_set1_pd(matrix[i * size + j].imag());
            
            __m256d input_real = _mm256_loadu_pd(&input[j].real());
            __m256d input_imag = _mm256_loadu_pd(&input[j].imag());
            
            // Complex multiplication
            real_result = _mm256_add_pd(real_result,
                _mm256_sub_pd(
                    _mm256_mul_pd(matrix_real, input_real),
                    _mm256_mul_pd(matrix_imag, input_imag)
                )
            );
            imag_result = _mm256_add_pd(imag_result,
                _mm256_add_pd(
                    _mm256_mul_pd(matrix_real, input_imag),
                    _mm256_mul_pd(matrix_imag, input_real)
                )
            );
        }
        
        _mm256_storeu_pd(&output[i].real(), real_result);
        _mm256_storeu_pd(&output[i].imag(), imag_result);
    }
}

void QuantumState::applyHadamard(int qubit) {
    const double inv_sqrt2 = 1.0 / std::sqrt(2.0);
    std::vector<std::complex<double>> h_matrix = {
        inv_sqrt2, inv_sqrt2,
        inv_sqrt2, -inv_sqrt2
    };
    applySingleQubitGate(qubit, h_matrix);
}

void QuantumState::applyCNOT(int control, int target) {
    std::vector<std::complex<double>> cnot_matrix = {
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 1.0, 0.0
    };
    applyTwoQubitGate(control, target, cnot_matrix);
}

void QuantumState::applyX(int qubit) {
    std::vector<std::complex<double>> x_matrix = {
        0.0, 1.0,
        1.0, 0.0
    };
    applySingleQubitGate(qubit, x_matrix);
}

void QuantumState::applyY(int qubit) {
    std::vector<std::complex<double>> y_matrix = {
        0.0, std::complex<double>(0.0, -1.0),
        std::complex<double>(0.0, 1.0), 0.0
    };
    applySingleQubitGate(qubit, y_matrix);
}

void QuantumState::applyZ(int qubit) {
    std::vector<std::complex<double>> z_matrix = {
        1.0, 0.0,
        0.0, -1.0
    };
    applySingleQubitGate(qubit, z_matrix);
}

void QuantumState::applyPhase(int qubit, double angle) {
    std::vector<std::complex<double>> p_matrix = {
        1.0, 0.0,
        0.0, std::exp(std::complex<double>(0.0, angle))
    };
    applySingleQubitGate(qubit, p_matrix);
}

void QuantumState::applyRx(int qubit, double angle) {
    double c = std::cos(angle/2);
    double s = std::sin(angle/2);
    std::vector<std::complex<double>> rx_matrix = {
        c, std::complex<double>(0.0, -s),
        std::complex<double>(0.0, -s), c
    };
    applySingleQubitGate(qubit, rx_matrix);
}

void QuantumState::applyRy(int qubit, double angle) {
    double c = std::cos(angle/2);
    double s = std::sin(angle/2);
    std::vector<std::complex<double>> ry_matrix = {
        c, -s,
        s, c
    };
    applySingleQubitGate(qubit, ry_matrix);
}

void QuantumState::applyRz(int qubit, double angle) {
    std::vector<std::complex<double>> rz_matrix = {
        std::exp(std::complex<double>(0.0, -angle/2)), 0.0,
        0.0, std::exp(std::complex<double>(0.0, angle/2))
    };
    applySingleQubitGate(qubit, rz_matrix);
}

bool QuantumState::measure(int qubit) {
    // Calculate probabilities
    double prob0 = 0.0;
    const int mask = 1 << qubit;
    
    for (int i = 0; i < (1 << num_qubits); ++i) {
        if ((i & mask) == 0) {
            prob0 += std::norm(state[i]);
        }
    }
    
    // Random measurement
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    bool result = dis(gen) > prob0;
    
    // Collapse state
    for (int i = 0; i < (1 << num_qubits); ++i) {
        if (((i & mask) == 0) == result) {
            state[i] = 0.0;
        }
    }
    
    normalize();
    return result;
}

void QuantumState::normalize() {
    double norm = 0.0;
    for (const auto& amplitude : state) {
        norm += std::norm(amplitude);
    }
    norm = std::sqrt(norm);
    
    for (auto& amplitude : state) {
        amplitude /= norm;
    }
}

void QuantumState::sortQubits(int& q1, int& q2, int& q3) {
    if (q1 > q2) std::swap(q1, q2);
    if (q2 > q3) std::swap(q2, q3);
    if (q1 > q2) std::swap(q1, q2);
}

void QuantumState::applyThreeQubitGate(int qubit1, int qubit2, int qubit3,
                                     const std::vector<std::complex<double>>& matrix) {
    sortQubits(qubit1, qubit2, qubit3);
    
    const int mask1 = 1 << qubit1;
    const int mask2 = 1 << qubit2;
    const int mask3 = 1 << qubit3;
    const int stride = 1 << (qubit3 + 1);
    
    for (int i = 0; i < (1 << num_qubits); i += stride) {
        for (int j = 0; j < (1 << qubit1); ++j) {
            for (int k = 0; k < (1 << (qubit2 - qubit1 - 1)); ++k) {
                for (int l = 0; l < (1 << (qubit3 - qubit2 - 1)); ++l) {
                    int base = i + j + (k << (qubit1 + 1)) + (l << (qubit2 + 1));
                    int idx000 = base;
                    int idx001 = base + mask1;
                    int idx010 = base + mask2;
                    int idx011 = base + mask1 + mask2;
                    int idx100 = base + mask3;
                    int idx101 = base + mask1 + mask3;
                    int idx110 = base + mask2 + mask3;
                    int idx111 = base + mask1 + mask2 + mask3;
                    
                    std::complex<double> v000 = state[idx000];
                    std::complex<double> v001 = state[idx001];
                    std::complex<double> v010 = state[idx010];
                    std::complex<double> v011 = state[idx011];
                    std::complex<double> v100 = state[idx100];
                    std::complex<double> v101 = state[idx101];
                    std::complex<double> v110 = state[idx110];
                    std::complex<double> v111 = state[idx111];
                    
                    // Apply matrix multiplication
                    state[idx000] = matrix[0] * v000 + matrix[1] * v001 + matrix[2] * v010 + matrix[3] * v011 +
                                  matrix[4] * v100 + matrix[5] * v101 + matrix[6] * v110 + matrix[7] * v111;
                    state[idx001] = matrix[8] * v000 + matrix[9] * v001 + matrix[10] * v010 + matrix[11] * v011 +
                                  matrix[12] * v100 + matrix[13] * v101 + matrix[14] * v110 + matrix[15] * v111;
                    // ... continue for other indices
                }
            }
        }
    }
}

void QuantumState::applyToffoli(int control1, int control2, int target) {
    std::vector<std::complex<double>> toffoli_matrix(64, 0.0);
    // Identity matrix for all states except |111⟩
    for (int i = 0; i < 8; ++i) {
        if (i != 7) { // Not |111⟩
            toffoli_matrix[i * 8 + i] = 1.0;
        }
    }
    // X gate for |111⟩
    toffoli_matrix[7 * 8 + 6] = 1.0;
    toffoli_matrix[6 * 8 + 7] = 1.0;
    
    applyThreeQubitGate(control1, control2, target, toffoli_matrix);
}

void QuantumState::applyFredkin(int control, int target1, int target2) {
    std::vector<std::complex<double>> fredkin_matrix(64, 0.0);
    // Identity matrix for all states except |111⟩ and |110⟩
    for (int i = 0; i < 8; ++i) {
        if (i != 6 && i != 7) { // Not |110⟩ or |111⟩
            fredkin_matrix[i * 8 + i] = 1.0;
        }
    }
    // Swap for |110⟩ and |111⟩
    fredkin_matrix[6 * 8 + 6] = 1.0;
    fredkin_matrix[7 * 8 + 7] = 1.0;
    
    applyThreeQubitGate(control, target1, target2, fredkin_matrix);
}

void QuantumState::applyCCNOT(int control1, int control2, int target) {
    applyToffoli(control1, control2, target); // CCNOT is the same as Toffoli
}

void QuantumState::applyCSWAP(int control, int target1, int target2) {
    applyFredkin(control, target1, target2); // CSWAP is the same as Fredkin
}

void QuantumState::saveState(const std::string& name) {
    state_cache[name] = state;
}

void QuantumState::loadState(const std::string& name) {
    auto it = state_cache.find(name);
    if (it != state_cache.end()) {
        state = it->second;
    } else {
        throw std::runtime_error("State '" + name + "' not found in cache");
    }
}

void QuantumState::clearCache() {
    state_cache.clear();
}

} // namespace qasm 