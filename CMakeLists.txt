cmake_minimum_required(VERSION 2.8.11)
project (chesssouls)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_PDB_OUTPUT_DIRECTORY     "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(beginner)
add_subdirectory(chesssouls)
add_subdirectory(greedy_player)
add_subdirectory(libchesssouls)
add_subdirectory(libchesssouls.tests)
add_subdirectory(random_player)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
