# Chess (C++ · Bitboards · SFML)

A lightweight chess engine and GUI built with C++17, bitboards, and SFML 3.

## Features
- Bitboard move generation (castling, en passant, promotions, checks).
- CLI mode (`e2e4`, `g7g8q`) and SFML GUI with legal-move highlights.

## Build (Windows)
Requires **CMake 3.16+**, **Visual Studio 2022**, **SFML 3 (via vcpkg)**.

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 `
  -DCMAKE_TOOLCHAIN_FILE="$env:USERPROFILE\vcpkg\scripts\buildsystems\vcpkg.cmake"
cmake --build build --config Release
```

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
