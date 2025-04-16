#include "OpenQASMCompiler/ast.hpp"
#include <gtest/gtest.h>
#include <memory>

namespace qasm {
namespace test {

class ASTTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a simple AST for testing
        root = std::make_unique<ASTNode>(ASTNodeType::Module);
        
        // Add include statement
        auto include = std::make_unique<ASTNode>(ASTNodeType::Include);
        include->setValue("qelib1.inc");
        root->addChild(std::move(include));
        
        // Add qubit register
        auto qreg = std::make_unique<ASTNode>(ASTNodeType::QubitDeclaration);
        qreg->setValue("q[2]");
        root->addChild(std::move(qreg));
        
        // Add classical register
        auto creg = std::make_unique<ASTNode>(ASTNodeType::ClassicalDeclaration);
        creg->setValue("c[2]");
        root->addChild(std::move(creg));
        
        // Add H gate
        auto hGate = std::make_unique<ASTNode>(ASTNodeType::GateCall);
        hGate->setValue("h");
        auto hTarget = std::make_unique<ASTNode>(ASTNodeType::Identifier);
        hTarget->setValue("q[0]");
        hGate->addChild(std::move(hTarget));
        root->addChild(std::move(hGate));
        
        // Add CNOT gate
        auto cnotGate = std::make_unique<ASTNode>(ASTNodeType::GateCall);
        cnotGate->setValue("cx");
        auto cnotControl = std::make_unique<ASTNode>(ASTNodeType::Identifier);
        cnotControl->setValue("q[0]");
        auto cnotTarget = std::make_unique<ASTNode>(ASTNodeType::Identifier);
        cnotTarget->setValue("q[1]");
        cnotGate->addChild(std::move(cnotControl));
        cnotGate->addChild(std::move(cnotTarget));
        root->addChild(std::move(cnotGate));
    }
    
    std::unique_ptr<ASTNode> root;
};

TEST_F(ASTTest, NodeCreation) {
    auto node = std::make_unique<ASTNode>(ASTNodeType::Module);
    EXPECT_EQ(node->getType(), ASTNodeType::Module);
    EXPECT_TRUE(node->getValue().empty());
    EXPECT_TRUE(node->getChildren().empty());
}

TEST_F(ASTTest, NodeValue) {
    auto node = std::make_unique<ASTNode>(ASTNodeType::Identifier);
    node->setValue("test");
    EXPECT_EQ(node->getValue(), "test");
}

TEST_F(ASTTest, NodeChildren) {
    auto parent = std::make_unique<ASTNode>(ASTNodeType::Module);
    auto child1 = std::make_unique<ASTNode>(ASTNodeType::Identifier);
    auto child2 = std::make_unique<ASTNode>(ASTNodeType::Number);
    
    parent->addChild(std::move(child1));
    parent->addChild(std::move(child2));
    
    EXPECT_EQ(parent->getChildren().size(), 2);
    EXPECT_EQ(parent->getChildren()[0]->getType(), ASTNodeType::Identifier);
    EXPECT_EQ(parent->getChildren()[1]->getType(), ASTNodeType::Number);
}

TEST_F(ASTTest, ASTStructure) {
    // Verify root node
    EXPECT_EQ(root->getType(), ASTNodeType::Module);
    EXPECT_EQ(root->getChildren().size(), 5);
    
    // Verify include statement
    auto include = root->getChildren()[0].get();
    EXPECT_EQ(include->getType(), ASTNodeType::Include);
    EXPECT_EQ(include->getValue(), "qelib1.inc");
    
    // Verify qubit register
    auto qreg = root->getChildren()[1].get();
    EXPECT_EQ(qreg->getType(), ASTNodeType::QubitDeclaration);
    EXPECT_EQ(qreg->getValue(), "q[2]");
    
    // Verify classical register
    auto creg = root->getChildren()[2].get();
    EXPECT_EQ(creg->getType(), ASTNodeType::ClassicalDeclaration);
    EXPECT_EQ(creg->getValue(), "c[2]");
    
    // Verify H gate
    auto hGate = root->getChildren()[3].get();
    EXPECT_EQ(hGate->getType(), ASTNodeType::GateCall);
    EXPECT_EQ(hGate->getValue(), "h");
    EXPECT_EQ(hGate->getChildren().size(), 1);
    EXPECT_EQ(hGate->getChildren()[0]->getType(), ASTNodeType::Identifier);
    EXPECT_EQ(hGate->getChildren()[0]->getValue(), "q[0]");
    
    // Verify CNOT gate
    auto cnotGate = root->getChildren()[4].get();
    EXPECT_EQ(cnotGate->getType(), ASTNodeType::GateCall);
    EXPECT_EQ(cnotGate->getValue(), "cx");
    EXPECT_EQ(cnotGate->getChildren().size(), 2);
    EXPECT_EQ(cnotGate->getChildren()[0]->getType(), ASTNodeType::Identifier);
    EXPECT_EQ(cnotGate->getChildren()[0]->getValue(), "q[0]");
    EXPECT_EQ(cnotGate->getChildren()[1]->getType(), ASTNodeType::Identifier);
    EXPECT_EQ(cnotGate->getChildren()[1]->getValue(), "q[1]");
}

TEST_F(ASTTest, NodeLineNumbers) {
    auto node = std::make_unique<ASTNode>(ASTNodeType::Module);
    node->setLineNumber(42);
    EXPECT_EQ(node->getLineNumber(), 42);
}

TEST_F(ASTTest, NodeCopy) {
    auto original = std::make_unique<ASTNode>(ASTNodeType::Module);
    original->setValue("test");
    original->setLineNumber(42);
    
    auto child = std::make_unique<ASTNode>(ASTNodeType::Identifier);
    child->setValue("child");
    original->addChild(std::move(child));
    
    auto copy = std::make_unique<ASTNode>(*original);
    
    EXPECT_EQ(copy->getType(), original->getType());
    EXPECT_EQ(copy->getValue(), original->getValue());
    EXPECT_EQ(copy->getLineNumber(), original->getLineNumber());
    EXPECT_EQ(copy->getChildren().size(), original->getChildren().size());
    EXPECT_EQ(copy->getChildren()[0]->getType(), original->getChildren()[0]->getType());
    EXPECT_EQ(copy->getChildren()[0]->getValue(), original->getChildren()[0]->getValue());
}

TEST_F(ASTTest, NodeMove) {
    auto original = std::make_unique<ASTNode>(ASTNodeType::Module);
    original->setValue("test");
    original->setLineNumber(42);
    
    auto child = std::make_unique<ASTNode>(ASTNodeType::Identifier);
    child->setValue("child");
    original->addChild(std::move(child));
    
    auto moved = std::make_unique<ASTNode>(std::move(*original));
    
    EXPECT_EQ(moved->getType(), ASTNodeType::Module);
    EXPECT_EQ(moved->getValue(), "test");
    EXPECT_EQ(moved->getLineNumber(), 42);
    EXPECT_EQ(moved->getChildren().size(), 1);
    EXPECT_EQ(moved->getChildren()[0]->getType(), ASTNodeType::Identifier);
    EXPECT_EQ(moved->getChildren()[0]->getValue(), "child");
}

} // namespace test
} // namespace qasm 