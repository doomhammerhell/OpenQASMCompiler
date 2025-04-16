#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include "../src/ast/ast.hpp"

extern FILE* yyin;
extern int yyparse();

class ParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary file for testing
        std::ofstream test_file("test.qasm");
        test_file << "OPENQASM 2.0;\n"
                  << "include \"qelib1.inc\";\n"
                  << "qreg q[2];\n"
                  << "creg c[2];\n"
                  << "h q[0];\n"
                  << "cx q[0],q[1];\n"
                  << "measure q -> c;\n";
        test_file.close();
    }

    void TearDown() override {
        // Remove temporary file
        std::remove("test.qasm");
    }
};

TEST_F(ParserTest, ParseValidProgram) {
    FILE* input = fopen("test.qasm", "r");
    ASSERT_NE(input, nullptr);
    
    yyin = input;
    EXPECT_EQ(yyparse(), 0);
    
    fclose(input);
}

TEST_F(ParserTest, ParseGateDeclaration) {
    std::ofstream test_file("test.qasm");
    test_file << "OPENQASM 2.0;\n"
              << "include \"qelib1.inc\";\n"
              << "gate my_gate(theta) q { rx(theta) q; }\n"
              << "qreg q[1];\n"
              << "my_gate(pi/2) q[0];\n";
    test_file.close();

    FILE* input = fopen("test.qasm", "r");
    ASSERT_NE(input, nullptr);
    
    yyin = input;
    EXPECT_EQ(yyparse(), 0);
    
    fclose(input);
}

TEST_F(ParserTest, ParseParameterizedGates) {
    std::ofstream test_file("test.qasm");
    test_file << "OPENQASM 2.0;\n"
              << "include \"qelib1.inc\";\n"
              << "qreg q[1];\n"
              << "rx(pi/2) q[0];\n"
              << "ry(pi/4) q[0];\n"
              << "rz(pi/8) q[0];\n";
    test_file.close();

    FILE* input = fopen("test.qasm", "r");
    ASSERT_NE(input, nullptr);
    
    yyin = input;
    EXPECT_EQ(yyparse(), 0);
    
    fclose(input);
}

TEST_F(ParserTest, ParseInvalidSyntax) {
    std::ofstream test_file("test.qasm");
    test_file << "OPENQASM 2.0;\n"
              << "include \"qelib1.inc\";\n"
              << "qreg q[2]\n"  // Missing semicolon
              << "h q[0];\n";
    test_file.close();

    FILE* input = fopen("test.qasm", "r");
    ASSERT_NE(input, nullptr);
    
    yyin = input;
    EXPECT_NE(yyparse(), 0);
    
    fclose(input);
}

TEST_F(ParserTest, ParseInvalidGate) {
    std::ofstream test_file("test.qasm");
    test_file << "OPENQASM 2.0;\n"
              << "include \"qelib1.inc\";\n"
              << "qreg q[1];\n"
              << "invalid_gate q[0];\n";  // Unknown gate
    test_file.close();

    FILE* input = fopen("test.qasm", "r");
    ASSERT_NE(input, nullptr);
    
    yyin = input;
    EXPECT_NE(yyparse(), 0);
    
    fclose(input);
}

// Add more parser tests here as needed 