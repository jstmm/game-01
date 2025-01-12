# Game 01 - Platform game (WIP)

## Dependencies
- GCC (supporting C++23)
- Raylib (works on 5.6)
- CMake (works on 3.28.3)

## Instructions
Use these two scripts:
``` bash
$ ./scripts/BUILD.sh
$ ./scripts/RUN.sh
```
or use the scripts in the .vscode folder.

To run on WSL, start **VcXsrv X Server** using *config.xlaunch* before starting
the program from a WSL console.

## TODO

### Features

- [x] Add collectible items with counter
- [ ] **Level editor**
  - [x] Add a way to switch to editor
  - [ ] Save / restore level
- [ ] Add inventory
- [ ] Set a window/taskbar icon

### Improvements

- [x] Keep showing the same image when character is jumping - do not reset to facing character

### Bugs
