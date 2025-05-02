# Compiler and flags
CC = gcc
CFLAGS = -fopenmp -Wall

# Declare phony targets to force execution
.PHONY: all run

# Default target: compile
all:
	$(CC) $(CFLAGS) -o diga_freq main.c

# Run with default input
run: diga_freq
	./diga_freq < test_input.txt

# Generic run with input file
%: diga_freq
	./diga_freq < $@
