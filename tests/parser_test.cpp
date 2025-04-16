#include "OpenQASMCompiler/parser.hpp"
#include <gtest/gtest.h>
#include <fstream>
#include <sstream>

namespace qasm {
namespace test {

class ParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test QASM files
        createTestFiles();
    }

    void TearDown() override {
        // Clean up test files
        removeTestFiles();
    }

    void createTestFiles() {
        // Create a simple QASM file
        std::ofstream simpleFile("test_data/simple.qasm");
        simpleFile << "OPENQASM 2.0;\n"
                  << "include \"qelib1.inc\";\n"
                  << "qreg q[2];\n"
                  << "creg c[2];\n"
                  << "h q[0];\n"
                  << "cx q[0], q[1];\n"
                  << "measure q[0] -> c[0];\n"
                  << "measure q[1] -> c[1];\n";
        simpleFile.close();

        // Create a QASM file with custom gates
        std::ofstream customGateFile("test_data/custom_gate.qasm");
        customGateFile << "OPENQASM 2.0;\n"
                      << "include \"qelib1.inc\";\n"
                      << "gate custom_gate a, b {\n"
                      << "  h a;\n"
                      << "  cx a, b;\n"
                      << "}\n"
                      << "qreg q[2];\n"
                      << "custom_gate q[0], q[1];\n";
        customGateFile.close();

        // Create a QASM file with control flow
        std::ofstream controlFlowFile("test_data/control_flow.qasm");
        controlFlowFile << "OPENQASM 2.0;\n"
                       << "include \"qelib1.inc\";\n"
                       << "qreg q[2];\n"
                       << "creg c[2];\n"
                       << "if (c == 0) h q[0];\n"
                       << "for (int i = 0; i < 2; i++) {\n"
                       << "  h q[i];\n"
                       << "}\n";
        controlFlowFile.close();
    }

    void removeTestFiles() {
        std::filesystem::remove("test_data/simple.qasm");
        std::filesystem::remove("test_data/custom_gate.qasm");
        std::filesystem::remove("test_data/control_flow.qasm");
    }

    Parser parser;
};

TEST_F(ParserTest, ParseSimpleCircuit) {
    std::ifstream file("test_data/simple.qasm");
    ASSERT_TRUE(file.is_open());

    auto ast = parser.parse(file);
    ASSERT_NE(ast, nullptr);

    // Verify AST structure
    EXPECT_EQ(ast->getType(), ASTNodeType::Module);
    EXPECT_EQ(ast->getChildren().size(), 6); // include, qreg, creg, h, cx, measure

    // Verify qubit register
    auto qreg = ast->getChildren()[1].get();
    EXPECT_EQ(qreg->getType(), ASTNodeType::QubitDeclaration);
    EXPECT_EQ(qreg->getValue(), "q[2]");

    // Verify classical register
    auto creg = ast->getChildren()[2].get();
    EXPECT_EQ(creg->getType(), ASTNodeType::ClassicalDeclaration);
    EXPECT_EQ(creg->getValue(), "c[2]");

    // Verify gates
    auto hGate = ast->getChildren()[3].get();
    EXPECT_EQ(hGate->getType(), ASTNodeType::GateCall);
    EXPECT_EQ(hGate->getValue(), "h");

    auto cxGate = ast->getChildren()[4].get();
    EXPECT_EQ(cxGate->getType(), ASTNodeType::GateCall);
    EXPECT_EQ(cxGate->getValue(), "cx");
}

TEST_F(ParserTest, ParseCustomGate) {
    std::ifstream file("test_data/custom_gate.qasm");
    ASSERT_TRUE(file.is_open());

    auto ast = parser.parse(file);
    ASSERT_NE(ast, nullptr);

    // Verify AST structure
    EXPECT_EQ(ast->getType(), ASTNodeType::Module);
    EXPECT_EQ(ast->getChildren().size(), 3); // include, gate definition, gate call

    // Verify gate definition
    auto gateDef = ast->getChildren()[1].get();
    EXPECT_EQ(gateDef->getType(), ASTNodeType::GateDeclaration);
    EXPECT_EQ(gateDef->getValue(), "custom_gate");
    EXPECT_EQ(gateDef->getChildren().size(), 2); // parameters and body

    // Verify gate call
    auto gateCall = ast->getChildren()[2].get();
    EXPECT_EQ(gateCall->getType(), ASTNodeType::GateCall);
    EXPECT_EQ(gateCall->getValue(), "custom_gate");
}

TEST_F(ParserTest, ParseControlFlow) {
    std::ifstream file("test_data/control_flow.qasm");
    ASSERT_TRUE(file.is_open());

    auto ast = parser.parse(file);
    ASSERT_NE(ast, nullptr);

    // Verify AST structure
    EXPECT_EQ(ast->getType(), ASTNodeType::Module);
    EXPECT_EQ(ast->getChildren().size(), 5); // include, qreg, creg, if, for

    // Verify if statement
    auto ifStmt = ast->getChildren()[3].get();
    EXPECT_EQ(ifStmt->getType(), ASTNodeType::IfStatement);
    EXPECT_EQ(ifStmt->getChildren().size(), 2); // condition and body

    // Verify for loop
    auto forLoop = ast->getChildren()[4].get();
    EXPECT_EQ(forLoop->getType(), ASTNodeType::ForLoop);
    EXPECT_EQ(forLoop->getChildren().size(), 3); // initialization, condition, body
}

TEST_F(ParserTest, ParseInvalidQASM) {
    std::stringstream invalidQASM("INVALID QASM CODE");
    EXPECT_THROW(parser.parse(invalidQASM), std::runtime_error);
}

TEST_F(ParserTest, ParseEmptyFile) {
    std::stringstream emptyFile;
    auto ast = parser.parse(emptyFile);
    EXPECT_EQ(ast, nullptr);
}

} // namespace test
} // namespace qasm 