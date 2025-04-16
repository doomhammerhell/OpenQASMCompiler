cmake_minimum_required(VERSION 3.10)
project(OpenQASMCompiler VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add build options
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
option(ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
option(ENABLE_DOXYGEN "Enable Doxygen documentation" OFF)

# Configure coverage if enabled
if(ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
endif()

# Configure clang-tidy if enabled
if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
    if(CLANG_TIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXE})
    endif()
endif()

# Configure Doxygen if enabled
if(ENABLE_DOXYGEN)
    find_package(Doxygen REQUIRED)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/docs)
        add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
    endif()
endif()

# Find required packages
find_package(Eigen3 REQUIRED)
find_package(CLI11 REQUIRED)

# Add source files
set(SOURCES
    src/parser.cpp
    src/ast.cpp
    src/ir.cpp
    src/optimizer.cpp
    src/cli.cpp
)

# Add header files
set(HEADERS
    include/OpenQASMCompiler/parser.hpp
    include/OpenQASMCompiler/ast.hpp
    include/OpenQASMCompiler/ir.hpp
    include/OpenQASMCompiler/optimizer.hpp
    include/OpenQASMCompiler/cli.hpp
)

# Create library
add_library(OpenQASMCompiler STATIC ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(OpenQASMCompiler
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${EIGEN3_INCLUDE_DIR}
)

# Link dependencies
target_link_libraries(OpenQASMCompiler
    PUBLIC
    Eigen3::Eigen
)

# Create compiler executable
add_executable(qasmc src/main.cpp)
target_link_libraries(qasmc
    PRIVATE
    OpenQASMCompiler
    CLI11::CLI11
)

# Install targets
install(TARGETS OpenQASMCompiler
    EXPORT OpenQASMCompilerTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(TARGETS qasmc
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include
)

# Export targets
install(EXPORT OpenQASMCompilerTargets
    FILE OpenQASMCompilerTargets.cmake
    NAMESPACE OpenQASMCompiler::
    DESTINATION lib/cmake/OpenQASMCompiler
)

# Add examples and tests subdirectories
add_subdirectory(examples)
add_subdirectory(tests)
