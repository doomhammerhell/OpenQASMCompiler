#include "OpenQASMCompiler/ast_dot_generator.hpp"
#include <sstream>
#include <iomanip>
#include <fstream>
#include <cstdlib>

namespace qasm {

std::string ASTDotGenerator::generateDot(ASTNode* root, const StyleConfig& config) {
    if (!root) return "";

    std::stringstream ss;
    ss << "digraph AST {\n";
    ss << "  node [fontname=\"" << config.fontName << "\", fontsize=" << config.fontSize << "];\n";
    ss << "  rankdir=" << config.rankDir << ";\n\n";

    size_t nodeId = 0;
    if (config.useClusters) {
        ss << generateCluster(root, nodeId, config);
    } else {
        ss << generateNode(root, nodeId, config);
    }

    ss << "}\n";
    return ss.str();
}

bool ASTDotGenerator::saveDotToFile(const std::string& dotString, const std::filesystem::path& outputPath) {
    try {
        std::ofstream file(outputPath);
        if (!file.is_open()) {
            return false;
        }
        file << dotString;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool ASTDotGenerator::renderDotFile(const std::filesystem::path& dotPath, 
                                  const std::filesystem::path& outputPath,
                                  const std::string& format) {
    std::string command = "dot -T" + format + " " + dotPath.string() + " -o " + outputPath.string();
    return std::system(command.c_str()) == 0;
}

bool ASTDotGenerator::generateAndRender(ASTNode* root,
                                      const std::filesystem::path& outputPath,
                                      const std::string& format,
                                      const StyleConfig& config) {
    std::string dotString = generateDot(root, config);
    std::filesystem::path dotPath = outputPath;
    dotPath.replace_extension(".dot");
    
    if (!saveDotToFile(dotString, dotPath)) {
        return false;
    }
    
    return renderDotFile(dotPath, outputPath, format);
}

std::string ASTDotGenerator::generateNode(ASTNode* node, size_t& nodeId, const StyleConfig& config) {
    if (!node) return "";

    std::stringstream ss;
    size_t currentId = nodeId++;
    
    // Generate node definition
    ss << "  " << currentId << " [label=\"" << generateLabel(node, config) << "\"";
    
    if (config.useShapes) {
        ss << ", shape=" << generateShape(node, config);
    }
    
    if (config.useColors) {
        ss << ", color=" << generateColor(node, config);
    }
    
    ss << getAdditionalAttributes(node);
    ss << "];\n";

    // Generate edges to children
    for (const auto& child : node->getChildren()) {
        size_t childId = nodeId;
        ss << generateNode(child.get(), nodeId, config);
        ss << generateEdge(currentId, childId, config);
    }

    return ss.str();
}

std::string ASTDotGenerator::generateEdge(size_t parentId, size_t childId, const StyleConfig& config) {
    std::stringstream ss;
    ss << "  " << parentId << " -> " << childId;
    if (config.useColors) {
        ss << " [color=gray]";
    }
    ss << ";\n";
    return ss.str();
}

std::string ASTDotGenerator::generateLabel(ASTNode* node, const StyleConfig& config) {
    std::stringstream ss;
    
    if (config.showNodeIds) {
        ss << "[" << node->getId() << "] ";
    }
    
    if (config.showLineNumbers && node->getLineNumber() > 0) {
        ss << "L" << node->getLineNumber() << ": ";
    }
    
    switch (node->getType()) {
        case ASTNodeType::Module:
            ss << "Module";
            break;
        case ASTNodeType::GateDeclaration:
            ss << "Gate: " << node->getValue();
            break;
        case ASTNodeType::GateCall:
            ss << "Gate Call: " << node->getValue();
            break;
        case ASTNodeType::QubitDeclaration:
            ss << "Qubit: " << node->getValue();
            break;
        case ASTNodeType::ClassicalDeclaration:
            ss << "Classical: " << node->getValue();
            break;
        case ASTNodeType::Barrier:
            ss << "Barrier";
            break;
        case ASTNodeType::Reset:
            ss << "Reset";
            break;
        case ASTNodeType::Measure:
            ss << "Measure";
            break;
        case ASTNodeType::IfStatement:
            ss << "If";
            break;
        case ASTNodeType::ForLoop:
            ss << "For";
            break;
        case ASTNodeType::WhileLoop:
            ss << "While";
            break;
        case ASTNodeType::Expression:
            ss << "Expr: " << node->getValue();
            break;
        case ASTNodeType::Identifier:
            ss << "Id: " << node->getValue();
            break;
        case ASTNodeType::Number:
            ss << "Num: " << node->getValue();
            break;
        case ASTNodeType::String:
            ss << "Str: " << node->getValue();
            break;
        case ASTNodeType::Operator:
            ss << "Op: " << node->getValue();
            break;
        default:
            ss << "Unknown";
    }

    return ss.str();
}

std::string ASTDotGenerator::generateShape(ASTNode* node, const StyleConfig& config) {
    if (!config.useShapes) return "box";
    
    switch (node->getType()) {
        case ASTNodeType::Module:
            return "box3d";
        case ASTNodeType::GateDeclaration:
        case ASTNodeType::GateCall:
            return "diamond";
        case ASTNodeType::QubitDeclaration:
        case ASTNodeType::ClassicalDeclaration:
            return "ellipse";
        case ASTNodeType::Barrier:
        case ASTNodeType::Reset:
        case ASTNodeType::Measure:
            return "hexagon";
        case ASTNodeType::IfStatement:
        case ASTNodeType::ForLoop:
        case ASTNodeType::WhileLoop:
            return "parallelogram";
        case ASTNodeType::Expression:
            return "box";
        case ASTNodeType::Identifier:
        case ASTNodeType::Number:
        case ASTNodeType::String:
            return "oval";
        case ASTNodeType::Operator:
            return "circle";
        default:
            return "box";
    }
}

std::string ASTDotGenerator::generateColor(ASTNode* node, const StyleConfig& config) {
    if (!config.useColors) return "black";
    
    switch (node->getType()) {
        case ASTNodeType::Module:
            return "blue";
        case ASTNodeType::GateDeclaration:
        case ASTNodeType::GateCall:
            return "red";
        case ASTNodeType::QubitDeclaration:
            return "green";
        case ASTNodeType::ClassicalDeclaration:
            return "purple";
        case ASTNodeType::Barrier:
        case ASTNodeType::Reset:
        case ASTNodeType::Measure:
            return "orange";
        case ASTNodeType::IfStatement:
        case ASTNodeType::ForLoop:
        case ASTNodeType::WhileLoop:
            return "cyan";
        case ASTNodeType::Expression:
            return "gray";
        case ASTNodeType::Identifier:
            return "black";
        case ASTNodeType::Number:
            return "brown";
        case ASTNodeType::String:
            return "pink";
        case ASTNodeType::Operator:
            return "yellow";
        default:
            return "black";
    }
}

std::string ASTDotGenerator::generateCluster(ASTNode* node, size_t& nodeId, const StyleConfig& config) {
    if (!node) return "";

    std::stringstream ss;
    size_t currentId = nodeId++;
    
    // Start cluster
    ss << "  subgraph cluster_" << currentId << " {\n";
    ss << "    label=\"" << generateLabel(node, config) << "\";\n";
    ss << "    style=filled;\n";
    ss << "    color=lightgray;\n";
    
    // Add node
    ss << "    " << currentId << " [label=\"" << generateLabel(node, config) << "\"";
    if (config.useShapes) {
        ss << ", shape=" << generateShape(node, config);
    }
    if (config.useColors) {
        ss << ", color=" << generateColor(node, config);
    }
    ss << getAdditionalAttributes(node);
    ss << "];\n";
    
    // Add children
    for (const auto& child : node->getChildren()) {
        size_t childId = nodeId;
        ss << generateCluster(child.get(), nodeId, config);
        ss << "    " << currentId << " -> " << childId << ";\n";
    }
    
    // End cluster
    ss << "  }\n";
    
    return ss.str();
}

std::string ASTDotGenerator::getAdditionalAttributes(ASTNode* node) {
    std::stringstream ss;
    
    // Add tooltip with additional information
    ss << ", tooltip=\"Type: " << static_cast<int>(node->getType());
    if (!node->getValue().empty()) {
        ss << "\\nValue: " << node->getValue();
    }
    if (node->getLineNumber() > 0) {
        ss << "\\nLine: " << node->getLineNumber();
    }
    ss << "\"";
    
    return ss.str();
}

} // namespace qasm 