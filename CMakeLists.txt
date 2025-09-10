cmake_minimum_required(VERSION 3.16)
project(CHESS CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---- Library sources ----
set(CHESS_LIB_SOURCES
    src/bitboard.cpp
    src/attacks.cpp
    src/position.cpp
    src/movegen.cpp
    src/make_undo.cpp
    src/status.cpp
    src/fen.cpp
    src/cli.cpp
    src/zobrist.cpp
    src/repetition.cpp 
)

# ---- Targets ----
add_library(chess STATIC ${CHESS_LIB_SOURCES})
target_include_directories(chess PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(chess_main src/main.cpp)
target_link_libraries(chess_main PRIVATE chess)


# ---- Tests ----
#enable_testing()
#add_subdirectory(tests)


# Set SFML_DIR to the directory containing SFMLConfig.cmake if not found automatically
#   set(SFML_DIR "C:/Path/To/SFML/lib/cmake/SFML")
# Alternatively, add the SFML installation prefix to CMAKE_PREFIX_PATH
# list(APPEND CMAKE_PREFIX_PATH "C:/Path/To/SFML")

find_package(SFML 3 CONFIG REQUIRED COMPONENTS Graphics Window System)

add_executable(chess_gui src/gui_main.cpp)
target_include_directories(chess_gui PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link the SFML 3 imported targets (note the SFML:: names)
target_link_libraries(chess_gui PRIVATE chess SFML::Graphics SFML::Window SFML::System)


