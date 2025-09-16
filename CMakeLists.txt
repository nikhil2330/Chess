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

add_library(chess STATIC ${CHESS_LIB_SOURCES})
target_include_directories(chess PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(chess_main src/main.cpp)
target_link_libraries(chess_main PRIVATE chess)


#enable_testing()
#add_subdirectory(tests)


#   set(SFML_DIR "C:/Path/To/SFML/lib/cmake/SFML")
# list(APPEND CMAKE_PREFIX_PATH "C:/Path/To/SFML")

find_package(SFML 3 CONFIG REQUIRED COMPONENTS Graphics Window System)

add_executable(chess_gui src/gui_main.cpp)
target_include_directories(chess_gui PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(chess_gui PRIVATE chess SFML::Graphics SFML::Window SFML::System)


