#include "OpenQASMCompiler/ast_dot_generator.hpp"
#include "OpenQASMCompiler/ast.hpp"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

namespace qasm {
namespace test {

class ASTDotGeneratorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a simple AST for testing
        root = std::make_unique<ASTNode>(ASTNodeType::Module);
        auto gateDecl = std::make_unique<ASTNode>(ASTNodeType::GateDeclaration);
        gateDecl->setValue("h");
        root->addChild(std::move(gateDecl));
        
        auto qubitDecl = std::make_unique<ASTNode>(ASTNodeType::QubitDeclaration);
        qubitDecl->setValue("q");
        root->addChild(std::move(qubitDecl));
        
        auto gateCall = std::make_unique<ASTNode>(ASTNodeType::GateCall);
        gateCall->setValue("h");
        root->addChild(std::move(gateCall));
    }
    
    std::unique_ptr<ASTNode> root;
    ASTDotGenerator::StyleConfig defaultConfig;
};

TEST_F(ASTDotGeneratorTest, GenerateDotBasic) {
    std::string dotString = ASTDotGenerator::generateDot(root.get(), defaultConfig);
    
    // Check basic DOT structure
    EXPECT_TRUE(dotString.find("digraph AST") != std::string::npos);
    EXPECT_TRUE(dotString.find("Module") != std::string::npos);
    EXPECT_TRUE(dotString.find("Gate: h") != std::string::npos);
    EXPECT_TRUE(dotString.find("Qubit: q") != std::string::npos);
}

TEST_F(ASTDotGeneratorTest, GenerateDotWithCustomConfig) {
    ASTDotGenerator::StyleConfig config;
    config.useColors = false;
    config.useShapes = false;
    config.showNodeIds = true;
    config.showLineNumbers = true;
    config.fontName = "Courier";
    config.fontSize = 14;
    config.rankDir = "LR";
    config.useClusters = true;
    
    std::string dotString = ASTDotGenerator::generateDot(root.get(), config);
    
    // Check custom configuration
    EXPECT_TRUE(dotString.find("fontname=\"Courier\"") != std::string::npos);
    EXPECT_TRUE(dotString.find("fontsize=14") != std::string::npos);
    EXPECT_TRUE(dotString.find("rankdir=LR") != std::string::npos);
    EXPECT_TRUE(dotString.find("subgraph cluster_") != std::string::npos);
}

TEST_F(ASTDotGeneratorTest, SaveDotToFile) {
    std::string dotString = ASTDotGenerator::generateDot(root.get(), defaultConfig);
    std::filesystem::path tempPath = std::filesystem::temp_directory_path() / "test.dot";
    
    EXPECT_TRUE(ASTDotGenerator::saveDotToFile(dotString, tempPath));
    
    // Verify file contents
    std::ifstream file(tempPath);
    std::string fileContent((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
    EXPECT_EQ(dotString, fileContent);
    
    // Clean up
    std::filesystem::remove(tempPath);
}

TEST_F(ASTDotGeneratorTest, GenerateAndRender) {
    std::filesystem::path outputPath = std::filesystem::temp_directory_path() / "test.png";
    
    bool success = ASTDotGenerator::generateAndRender(root.get(), outputPath, "png", defaultConfig);
    
    // Check if file was created
    EXPECT_TRUE(success);
    EXPECT_TRUE(std::filesystem::exists(outputPath));
    
    // Clean up
    std::filesystem::remove(outputPath);
    std::filesystem::remove(outputPath.replace_extension(".dot"));
}

TEST_F(ASTDotGeneratorTest, NodeAttributes) {
    std::string dotString = ASTDotGenerator::generateDot(root.get(), defaultConfig);
    
    // Check for tooltips
    EXPECT_TRUE(dotString.find("tooltip=") != std::string::npos);
    
    // Check for shapes
    EXPECT_TRUE(dotString.find("shape=box3d") != std::string::npos);
    EXPECT_TRUE(dotString.find("shape=diamond") != std::string::npos);
    EXPECT_TRUE(dotString.find("shape=ellipse") != std::string::npos);
    
    // Check for colors
    EXPECT_TRUE(dotString.find("color=blue") != std::string::npos);
    EXPECT_TRUE(dotString.find("color=red") != std::string::npos);
    EXPECT_TRUE(dotString.find("color=green") != std::string::npos);
}

TEST_F(ASTDotGeneratorTest, EdgeAttributes) {
    std::string dotString = ASTDotGenerator::generateDot(root.get(), defaultConfig);
    
    // Check for edge attributes
    EXPECT_TRUE(dotString.find("->") != std::string::npos);
    EXPECT_TRUE(dotString.find("[color=gray]") != std::string::npos);
}

TEST_F(ASTDotGeneratorTest, ClusterGeneration) {
    ASTDotGenerator::StyleConfig config;
    config.useClusters = true;
    
    std::string dotString = ASTDotGenerator::generateDot(root.get(), config);
    
    // Check for cluster structure
    EXPECT_TRUE(dotString.find("subgraph cluster_") != std::string::npos);
    EXPECT_TRUE(dotString.find("style=filled") != std::string::npos);
    EXPECT_TRUE(dotString.find("color=lightgray") != std::string::npos);
}

} // namespace test
} // namespace qasm 