# tests/CMakeLists.txt

function(add_chess_test name)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} PRIVATE chess)
  add_test(NAME ${name} COMMAND ${name})
endfunction()

add_chess_test(fen_roundtrip)
add_chess_test(make_undo_roundtrip)
add_chess_test(move_counts)
add_chess_test(attacks_knight_king)
add_chess_test(en_passant)
add_chess_test(status_draws)
add_chess_test(status_checkmate)
add_chess_test(perft_positions)
add_chess_test(perft_divide)


