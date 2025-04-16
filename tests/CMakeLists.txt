cmake_minimum_required(VERSION 3.10)
project(OpenQASMCompilerTests)

# Find required packages
find_package(GTest REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Threads REQUIRED)

# Add the main library as a dependency
add_subdirectory(..)

# Add test executables
add_executable(parser_test parser_test.cpp)
add_executable(ast_test ast_test.cpp)
add_executable(ast_dot_generator_test ast_dot_generator_test.cpp)
add_executable(quantum_circuit_test quantum_circuit_test.cpp)
add_executable(quantum_simulator_test quantum_simulator_test.cpp)
add_executable(circuit_optimizer_test circuit_optimizer_test.cpp)

# Link test executables against the main library and required dependencies
target_link_libraries(parser_test PRIVATE
    OpenQASMCompiler
    GTest::GTest
    GTest::Main
    Threads::Threads
)

target_link_libraries(ast_test PRIVATE
    OpenQASMCompiler
    GTest::GTest
    GTest::Main
    Threads::Threads
)

target_link_libraries(ast_dot_generator_test PRIVATE
    OpenQASMCompiler
    GTest::GTest
    GTest::Main
    Threads::Threads
)

target_link_libraries(quantum_circuit_test PRIVATE
    OpenQASMCompiler
    GTest::GTest
    GTest::Main
    Threads::Threads
)

target_link_libraries(quantum_simulator_test PRIVATE
    OpenQASMCompiler
    GTest::GTest
    GTest::Main
    Threads::Threads
)

target_link_libraries(circuit_optimizer_test PRIVATE
    OpenQASMCompiler
    GTest::GTest
    GTest::Main
    Threads::Threads
)

# Set C++ standard
set_target_properties(parser_test
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(ast_test
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(ast_dot_generator_test
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(quantum_circuit_test
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(quantum_simulator_test
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(circuit_optimizer_test
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Include directories
target_include_directories(parser_test
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${EIGEN3_INCLUDE_DIR}
)

target_include_directories(ast_test
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${EIGEN3_INCLUDE_DIR}
)

target_include_directories(ast_dot_generator_test
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${EIGEN3_INCLUDE_DIR}
)

target_include_directories(quantum_circuit_test
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${EIGEN3_INCLUDE_DIR}
)

target_include_directories(quantum_simulator_test
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${EIGEN3_INCLUDE_DIR}
)

target_include_directories(circuit_optimizer_test
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${EIGEN3_INCLUDE_DIR}
)

# Add test targets to CTest
include(GoogleTest)
gtest_discover_tests(parser_test)
gtest_discover_tests(ast_test)
gtest_discover_tests(ast_dot_generator_test)
gtest_discover_tests(quantum_circuit_test)
gtest_discover_tests(quantum_simulator_test)
gtest_discover_tests(circuit_optimizer_test)

# Set test properties
set_tests_properties(
    parser_test
    ast_test
    ast_dot_generator_test
    quantum_circuit_test
    quantum_simulator_test
    circuit_optimizer_test
    PROPERTIES
    ENVIRONMENT "TEST_DATA_DIR=${CMAKE_CURRENT_SOURCE_DIR}/test_data"
)

# Enable testing
enable_testing()

# ==-------- Support ----------==
efd_test (CommandLineTests
    EfdSupport)

efd_test (GraphTests
    EfdSupport)

efd_test (WeightedGraphTests
    EfdSupport)

efd_test (SIFinderTests
    EfdSupport)

efd_test (BFSPathFinderTests
    EfdSupport)

efd_test (BFSCachedDistanceTests
    EfdSupport)

efd_test (ApproxTSFinderTests
    EfdSupport)

efd_test (SimplifiedApproxTSFinderTests
    EfdSupport)

efd_test (EnumStringTests
    EfdSupport)

efd_test (PossibleValuesListTraitTests
    EfdSupport)

efd_test (BitOptionsTests
    EfdSupport)

efd_test (GraphDotifyTests
    EfdSupport)

# ==-------- Analysis ----------==
efd_test (ASTNodeTests
    EfdAnalysis EfdSupport)

efd_test (DriverTests
    EfdAnalysis EfdSupport)

efd_test (DriverFileTests
    EfdAnalysis EfdSupport)

# ==-------- Arch ----------==
efd_test (ArchGraphTests
    EfdArch EfdAnalysis EfdSupport)

efd_test (IBMQX2Tests
    EfdArch EfdAnalysis EfdSupport)

# ==-------- Transforms ----------==
efd_test (QModuleTests
    EfdTransform EfdAnalysis EfdSupport)

efd_test (QModuleCloneTests
    EfdTransform EfdAnalysis EfdSupport)

efd_test (XbitToNumberWrapperPassTests
    EfdTransform EfdAnalysis EfdSupport)

efd_test (DependencyBuilderPassTests
    EfdTransform EfdAnalysis EfdSupport)

efd_test (FlattenPassTests
    EfdTransform EfdAnalysis EfdSupport)

efd_test (TransformUtilsTests
    EfdTransform EfdAnalysis EfdSupport)

efd_test (InlineAllPassTests
    EfdTransform EfdAnalysis EfdSupport)

efd_test (RenameQbitsPassTests
    EfdTransform EfdAnalysis EfdSupport)

efd_test (CircuitGraphBuilderPassTests
    EfdTransform EfdAnalysis EfdSupport)

efd_test (CNOTLBOWrapperPassTests
    EfdTransform EfdAnalysis EfdSupport)

efd_test (ReverseEdgesPassTests
    EfdAllocator EfdTransform EfdArch EfdAnalysis EfdSupport)

efd_test (SemanticVerifierPassTests
    EfdAllocator EfdTransform EfdArch EfdAnalysis EfdSupport)

efd_test (ArchVerifierPassTests
    EfdAllocator EfdTransform EfdArch EfdAnalysis EfdSupport)

efd_test (LayersBuilderPassTests
    EfdAllocator EfdTransform EfdArch EfdAnalysis EfdSupport)

efd_test (DependencyGraphBuilderPassTests
    EfdAllocator EfdTransform EfdArch EfdAnalysis EfdSupport)

efd_test (CircuitGraphTests
    EfdAllocator EfdTransform EfdArch EfdAnalysis EfdSupport)

efd_test (QModuleQualityEvalPassTests
    EfdAllocator EfdTransform EfdArch EfdAnalysis EfdSupport)

efd_test (QubitRemapPassTests
    EfdAllocator EfdTransform EfdArch EfdAnalysis EfdSupport)

# ==-------- Allocator ----------==
efd_test (DynprogQAllocatorTests
    EfdAllocator EfdTransform EfdArch EfdAnalysis EfdSupport)

efd_test (IBMQAllocatorTests
    EfdAllocator EfdTransform EfdArch EfdAnalysis EfdSupport)

efd_test (SabreQAllocatorTests
    EfdAllocator EfdSimpleImpl EfdTransform EfdArch EfdAnalysis EfdSupport)

efd_test (BoundedMappingTreeQAllocatorTests
    EfdAllocator EfdBMTImpl EfdTransform EfdArch EfdAnalysis EfdSupport)

efd_test (OptBMTQAllocatorTests
    EfdAllocator EfdBMTImpl EfdTransform EfdArch EfdAnalysis EfdSupport)

efd_test (GreedyCktQAllocatorTests
    EfdAllocator EfdSimpleImpl EfdTransform EfdArch EfdAnalysis EfdSupport)

efd_test (WeightedSIMappingFinderTests
    EfdAllocator EfdSimpleImpl EfdTransform EfdArch EfdAnalysis EfdSupport)

efd_test (QbitterQbitAllocatorTests
    EfdAllocator EfdSimpleImpl EfdTransform EfdArch EfdAnalysis EfdSupport)

efd_test (JKUQAllocatorTests
    EfdAllocator EfdTransform EfdArch EfdAnalysis EfdSupport)

efd_test (ChallengeWinnerQAllocatorTests
    EfdAllocator EfdTransform EfdArch EfdAnalysis EfdSupport)

efd_test (LayeredBMTQAllocatorTests
    EfdAllocator EfdTransform EfdArch EfdAnalysis EfdSupport)
