SRC_FILES := $(wildcard src/*.cc)
HEADER_FILES := $(wildcard include/*.hh)
EXEC := build/game

$(EXEC): $(SRC_FILES) $(HEADER_FILES)
	mkdir -p build
	bear -- g++ -g -std=c++23 $(SRC_FILES) -I./include $(shell pkg-config --libs --cflags raylib) -lGL -lm -lpthread -ldl -lrt -lX11 -o $@

.PHONY: run clean
run:
	LD_LIBRARY_PATH=$(shell pkg-config --libs raylib | sed 's/-L//g' | sed 's/-l[^ ]*//g') ./$(EXEC)

clean:
	rm -rvf ./build/**
