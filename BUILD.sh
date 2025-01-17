#!/usr/bin/env bash

run_command() {
    echo "Running: $*"
    "$@"
}

if [ "$1" == "RELEASE" ]; then
    run_command cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
else
    run_command cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
fi

run_command cmake --build build

# run_command meson setup build
# run_command meson compile -C build
