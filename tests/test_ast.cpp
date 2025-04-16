#include <gtest/gtest.h>
#include "../src/ast/ast.hpp"

using namespace qasm;

TEST(ASTTest, QRegDecl) {
    QRegDecl qreg("q", 2);
    EXPECT_EQ(qreg.name, "q");
    EXPECT_EQ(qreg.size, 2);
    EXPECT_EQ(qreg.toString(), "QReg q[2]");
}

TEST(ASTTest, CRegDecl) {
    CRegDecl creg("c", 2);
    EXPECT_EQ(creg.name, "c");
    EXPECT_EQ(creg.size, 2);
    EXPECT_EQ(creg.toString(), "CReg c[2]");
}

TEST(ASTTest, Program) {
    auto program = std::make_shared<Program>();
    auto qreg = std::make_shared<QRegDecl>("q", 2);
    auto creg = std::make_shared<CRegDecl>("c", 2);
    
    program->statements.push_back(qreg);
    program->statements.push_back(creg);
    
    EXPECT_EQ(program->statements.size(), 2);
    EXPECT_EQ(program->toString(), "Program:\n  QReg q[2]\n  CReg c[2]\n");
}

TEST(ASTTest, GateDecl) {
    std::vector<std::string> params = {"theta"};
    std::vector<std::string> qubits = {"q"};
    auto gate = std::make_shared<GateDecl>("my_gate", params, qubits);
    
    EXPECT_EQ(gate->name, "my_gate");
    EXPECT_EQ(gate->params.size(), 1);
    EXPECT_EQ(gate->qubits.size(), 1);
    EXPECT_EQ(gate->toString(), "gate my_gate(theta) q {\n}");
}

TEST(ASTTest, GateCall) {
    auto qubit = std::make_shared<Qubit>("q", 0);
    std::vector<std::shared_ptr<Node>> args;
    std::vector<std::shared_ptr<Qubit>> qubits = {qubit};
    auto gate_call = std::make_shared<GateCall>("h", args, qubits);
    
    EXPECT_EQ(gate_call->name, "h");
    EXPECT_EQ(gate_call->args.size(), 0);
    EXPECT_EQ(gate_call->qubits.size(), 1);
    EXPECT_EQ(gate_call->toString(), "h q[0]");
}

TEST(ASTTest, Measurement) {
    auto qubit = std::make_shared<Qubit>("q", 0);
    auto cbit = std::make_shared<ClassicalBit>("c", 0);
    auto meas = std::make_shared<Measurement>(qubit, cbit);
    
    EXPECT_EQ(meas->toString(), "measure q[0] -> c[0]");
}

TEST(ASTTest, Qubit) {
    auto qubit = std::make_shared<Qubit>("q", 1);
    EXPECT_EQ(qubit->reg, "q");
    EXPECT_EQ(qubit->index, 1);
    EXPECT_EQ(qubit->toString(), "q[1]");
}

TEST(ASTTest, ClassicalBit) {
    auto cbit = std::make_shared<ClassicalBit>("c", 1);
    EXPECT_EQ(cbit->reg, "c");
    EXPECT_EQ(cbit->index, 1);
    EXPECT_EQ(cbit->toString(), "c[1]");
} 