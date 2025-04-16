cmake_minimum_required(VERSION 3.10)
project(OpenQASMCompilerExamples)

# Find required packages
find_package(Eigen3 REQUIRED)

# Add the main library as a dependency
add_subdirectory(..)

# Add the examples executable
add_executable(quantum_algorithms_example quantum_algorithms_example.cpp)

# Link against the main library and required dependencies
target_link_libraries(quantum_algorithms_example
    PRIVATE
    OpenQASMCompiler
    Eigen3::Eigen
)

# Set C++ standard
set_target_properties(quantum_algorithms_example
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Include directories
target_include_directories(quantum_algorithms_example
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${EIGEN3_INCLUDE_DIR}
)

# Install the example
install(TARGETS quantum_algorithms_example
    RUNTIME DESTINATION bin
) 