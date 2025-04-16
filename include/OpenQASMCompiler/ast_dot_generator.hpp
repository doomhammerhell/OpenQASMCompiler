#ifndef OPENQASM_AST_DOT_GENERATOR_HPP
#define OPENQASM_AST_DOT_GENERATOR_HPP

#include "OpenQASMCompiler/ast.hpp"
#include <string>
#include <sstream>
#include <unordered_map>
#include <filesystem>

namespace qasm {

/**
 * @brief Class for generating DOT format output from an AST
 * 
 * This class provides functionality to convert an AST into DOT format,
 * which can be used with Graphviz to generate visualizations of the AST.
 * It supports various styling options and can automatically render the
 * visualization using Graphviz.
 */
class ASTDotGenerator {
public:
    /**
     * @brief Style configuration for DOT generation
     */
    struct StyleConfig {
        bool useColors = true;              ///< Whether to use colors in the visualization
        bool useShapes = true;              ///< Whether to use different shapes for different node types
        bool showNodeIds = false;           ///< Whether to show node IDs in labels
        bool showLineNumbers = false;       ///< Whether to show line numbers in labels
        std::string fontName = "Arial";     ///< Font name for labels
        int fontSize = 12;                  ///< Font size for labels
        std::string rankDir = "TB";         ///< Direction of graph layout (TB, LR, BT, RL)
        bool useClusters = false;           ///< Whether to group related nodes in clusters
    };

    /**
     * @brief Generate DOT format string from an AST
     * @param root The root node of the AST
     * @param config Optional style configuration
     * @return DOT format string that can be used with Graphviz
     */
    static std::string generateDot(ASTNode* root, const StyleConfig& config = StyleConfig());

    /**
     * @brief Save DOT string to a file
     * @param dotString The DOT format string
     * @param outputPath Path to save the DOT file
     * @return true if successful, false otherwise
     */
    static bool saveDotToFile(const std::string& dotString, const std::filesystem::path& outputPath);

    /**
     * @brief Render DOT file to an image using Graphviz
     * @param dotPath Path to the DOT file
     * @param outputPath Path to save the rendered image
     * @param format Output format (e.g., "png", "svg", "pdf")
     * @return true if successful, false otherwise
     */
    static bool renderDotFile(const std::filesystem::path& dotPath, 
                            const std::filesystem::path& outputPath,
                            const std::string& format = "png");

    /**
     * @brief Generate and render AST visualization in one step
     * @param root The root node of the AST
     * @param outputPath Path to save the rendered image
     * @param format Output format (e.g., "png", "svg", "pdf")
     * @param config Optional style configuration
     * @return true if successful, false otherwise
     */
    static bool generateAndRender(ASTNode* root,
                                const std::filesystem::path& outputPath,
                                const std::string& format = "png",
                                const StyleConfig& config = StyleConfig());

private:
    /**
     * @brief Generate DOT node definition
     * @param node The AST node to generate DOT for
     * @param nodeId Unique identifier for the node
     * @param config Style configuration
     * @return DOT node definition string
     */
    static std::string generateNode(ASTNode* node, size_t& nodeId, const StyleConfig& config);

    /**
     * @brief Generate DOT edge definition
     * @param parentId ID of the parent node
     * @param childId ID of the child node
     * @param config Style configuration
     * @return DOT edge definition string
     */
    static std::string generateEdge(size_t parentId, size_t childId, const StyleConfig& config);

    /**
     * @brief Generate DOT label for a node
     * @param node The AST node
     * @param config Style configuration
     * @return Label string for the node
     */
    static std::string generateLabel(ASTNode* node, const StyleConfig& config);

    /**
     * @brief Generate DOT shape for a node
     * @param node The AST node
     * @param config Style configuration
     * @return Shape string for the node
     */
    static std::string generateShape(ASTNode* node, const StyleConfig& config);

    /**
     * @brief Generate DOT color for a node
     * @param node The AST node
     * @param config Style configuration
     * @return Color string for the node
     */
    static std::string generateColor(ASTNode* node, const StyleConfig& config);

    /**
     * @brief Generate DOT cluster for a node and its children
     * @param node The AST node
     * @param nodeId Unique identifier for the node
     * @param config Style configuration
     * @return DOT cluster definition string
     */
    static std::string generateCluster(ASTNode* node, size_t& nodeId, const StyleConfig& config);

    /**
     * @brief Get additional attributes for a node based on its type
     * @param node The AST node
     * @return Additional DOT attributes string
     */
    static std::string getAdditionalAttributes(ASTNode* node);
};

} // namespace qasm

#endif // OPENQASM_AST_DOT_GENERATOR_HPP 