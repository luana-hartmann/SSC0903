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

input_gen:
	$(CC) inputs_generator.c -o input_gen

PROGS = main1 main2 main3
TEST_DIR = inputs
OUT_DIR = outputs
RUNS = 30

TEST_FILES := $(basename $(notdir $(wildcard $(TEST_DIR)/*.txt)))

OUTPUTS := $(foreach prog,$(PROGS),$(foreach test,$(TEST_FILES),$(OUT_DIR)/$(prog)_$(test).out))

teste: $(OUTPUTS)

$(OUT_DIR)/%.out:
	@mkdir -p $(OUT_DIR)
	@prog_test=$(basename $(notdir $(@:.out=))); \
	prog=$${prog_test%%_*}; \
	test=$${prog_test#*_}; \
	infile=$(TEST_DIR)/$$test.txt; \
	echo "Executando $$prog com $$infile..."; \
	rm -f $@; \
	for i in $(shell seq 1 $(RUNS)); do \
		./$$prog < $$infile >> $@; \
	done

clean:
	rm -f $(OUT_DIR)/*.out

