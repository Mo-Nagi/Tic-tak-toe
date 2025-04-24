cmake_minimum_required(VERSION 3.14)

# Set vcpkg path (choose only ONE)
set(VCPKG_ROOT "C:/vcpkg")
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

project(TicTacToe)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find GTest (CONFIG mode required for vcpkg)
find_package(GTest CONFIG REQUIRED)

# Library
add_library(TicTacToeLib
    game_board.cpp
    game_logic.cpp
    tic_tac_toe.cpp
    tic_tac_toe_ai.cpp
)
target_include_directories(TicTacToeLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Main game
add_executable(TicTacToe main.cpp)
target_link_libraries(TicTacToe PRIVATE TicTacToeLib)

# Tests
add_executable(TicTacToeTests test_tic_tac_toe.cpp)
target_link_libraries(TicTacToeTests PRIVATE 
    TicTacToeLib 
    GTest::gtest 
    GTest::gtest_main
)

enable_testing()
add_test(NAME TicTacToeTests COMMAND TicTacToeTests)