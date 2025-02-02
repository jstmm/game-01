# Game 01 - Platform game (WIP)

## Dependencies
```
brew install cmake ninja raylib
sudo apt install mesa-utils libglu1-mesa-dev freeglut3-dev mesa-common-dev libglew-dev libglfw3-dev libglm-dev
```

## Instructions
Use these two scripts:
``` bash
$ ./BUILD.sh
$ ./RUN.sh
```
or use the scripts in the .vscode folder.

To run on WSL, start **VcXsrv X Server** using *config.xlaunch* before starting
the program from a WSL console.


### Features

- User score
- Edit and save level

## TODO

### Improvements

- [x] Keep showing the same image when character is jumping - do not reset to facing character
- [ ] Add inventory
- [ ] Set a window/taskbar icon

### Bugs

- [ ] Release build failing
