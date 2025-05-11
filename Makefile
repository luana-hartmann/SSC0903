# Compiler and flags
CC = gcc
CFLAGS = -O3 -fopt-info-vec-optimized -fopenmp

all1:
	$(CC) $(CFLAGS) main_opt1.c -o main1 -g

all2:
	$(CC) $(CFLAGS) main_opt2.c -o main2 -g

all3:
	$(CC) $(CFLAGS) main_opt3.c -o main3 -g

all: all1 all2 all3
