cmake_minimum_required(VERSION 3.16)
project(AdvancedTicTacToe VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
option(ENABLE_TESTING "Enable testing" ON)
if(ENABLE_TESTING)
    enable_testing()
    
    # Fetch GoogleTest
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/../core/include)
include_directories(${CMAKE_SOURCE_DIR}/../GUI/include)

set(CORE_LIB_SOURCES
    ${CMAKE_SOURCE_DIR}/../core/src/GameBoard.cpp
    ${CMAKE_SOURCE_DIR}/../core/src/GameHistory.cpp
    ${CMAKE_SOURCE_DIR}/../core/src/GameStateStack.cpp
    ${CMAKE_SOURCE_DIR}/../core/src/AIPlayer.cpp
)
add_library(game_core STATIC ${CORE_LIB_SOURCES})
target_include_directories(game_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/core/include)
# Testing configuration

if(ENABLE_TESTING)
    add_executable(gameboard_test ${CMAKE_SOURCE_DIR}/../tests/unit_tests/GameBoard_test.cpp)
    target_link_libraries(gameboard_test game_core gtest gtest_main)
    add_test(NAME GameBoardTest COMMAND gameboard_test)

    add_executable(gamehistory_test ${CMAKE_SOURCE_DIR}/../tests/unit_tests/GameHistory_test.cpp)
    target_link_libraries(gamehistory_test game_core gtest gtest_main)
    add_test(NAME GameHistoryTest COMMAND gamehistory_test)

    add_executable(gamestack_test ${CMAKE_SOURCE_DIR}/../tests/unit_tests/GameStateStack_test.cpp)
    target_link_libraries(gamestack_test game_core gtest gtest_main)
    add_test(NAME GameStackTest COMMAND gamestack_test)

    add_executable(AIPlayer_test_test ${CMAKE_SOURCE_DIR}/../tests/unit_tests/AIPlayer_test.cpp)
    target_link_libraries(AIPlayer_test_test game_core gtest gtest_main)
    add_test(NAME AIPlayerTest COMMAND AIPlayer_test_test)

endif()
