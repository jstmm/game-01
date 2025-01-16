#!/usr/bin/env bash

run_command() {
    echo "Running: $*"
    "$@"
}

run_command cmake -S . -G "Unix Makefiles" -B build
# run_command cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

run_command cmake --build build
