# Compiler and flags
CC = gcc
CFLAGS = -O3 -fopt-info-vec-optimized -fopenmp

# Declare phony targets to force execution
.PHONY: all run

all:
	$(CC) $(CFLAGS) main.c -o main -g

# Run with default input
run:
	./main < test_input.txt

# Generic run with input file
%:
	./main < $@
