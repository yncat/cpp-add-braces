cmake_minimum_required(VERSION 3.13)
project(AddBracesTool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find LLVM and Clang
find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Add executable
add_executable(add-braces
    add_braces.cpp
)

# Link against Clang libraries
target_link_libraries(add-braces
    clangTooling
    clangBasic
    clangASTMatchers
    clangRewrite
    clangFrontend
    clangSerialization
)
