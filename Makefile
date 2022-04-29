# DPP Bot makefile

CPP=g++
CFLAGS=-std=gnu++2a -ldpp -I ../DPP/include -Wall -Wextra -pthread
DEBUG_OUTFILE=bin/debug
RELEASE_OUTFILE=bin/release
DEBUG_FLAGS=-g -O0
RELEASE_FLAGS=-O3 -s
MAIN=src/main.cpp

build_debug: $(MAIN)
	$(CPP) $(CFLAGS) -o $(DEBUG_OUTFILE) $(MAIN) $(DEBUG_FLAGS)

build_release: $(MAIN)
	$(CPP) $(CFLAGS) -o $(RELEASE_OUTFILE) $(MAIN) $(RELEASE_FLAGS)