# Chess (C++ · Bitboards · SFML)

A lightweight chess engine and GUI built with C++, bitboards, and SFML 3.

## Features
- Bitboard move generation for speed.
- CLI mode (`e2e4`, `g7g8q`) and SFML GUI mode as well.
- Uses bitboard manipulation and zobrist hashing for speed and multiple different cases.
- Will make the gui better, give more details and moves and then will work on the engine starting with a minimax search. Then i'll move to alpha-beta pruning. I'll try out other algorithms depending on how it goes.
- I'll also consider using magic bitboards.

## Build (Windows)
Will give full details later

## Run 
```powershell
.\build\Release\chess_main.exe   # CLI
.\build\Release\chess_gui.exe    # GUI (needs assets/DejaVuSans.ttf)
```

## Controls (GUI)
- Left click  - select/move
- Right click - cancel
- U           - undo
- R           - reset
