#!/usr/bin/env bash

# if [[ "$1" == "release" ]]; then
#   BUILD_TYPE="release"
# else
#   BUILD_TYPE="debug"
# fi

# mkdir -p build
# rm -f build/**

# if [[ "$BUILD_TYPE" == "release" ]]; then
#   FLAGS="-std=c++23 -O3 -DNDEBUG"
# else
#   FLAGS="-g -std=c++23"
# fi

# g++ $FLAGS src/*.cpp -o build/game -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

run_command() {
    echo "Running: $*"
    "$@"
}

run_command cmake -S . -G "Unix Makefiles" -B build
# run_command cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

run_command cmake --build build
