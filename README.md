# Character Frequency Counter using OpenMP

This is a C program that reads lines of text from standard input (`stdin`) and counts the frequency of all visible ASCII characters (from 32 to 127). The purpose is to practice parallel programming using **OpenMP**.

## What the Program Does

For each line of input:
- Counts how many times each character appears.
- Prints the ASCII value and frequency of each character (one per line), sorted.
- Adds a blank line between the outputs of different input lines.

## Example Input (`test_input.txt`):

````
AACBAB

312323
````

### Expected Output:

````
67 1
66 2
65 3

49 1
50 2
51 3
````

## How to Compile

This project includes a simple `Makefile`.

````
make all
````

This will generate three different versions of the solution.
- `main1` corresponds to the sequential algorithm in `main_opt1`;
- `main2` presents an algorithm that processes each line in parallel, present in `main_opt2`;
- `main3` brings a fully parallelized algorithm, which code is in `main_opt3`.

You can find more information about the algorithms and their performances in the document `Relatório.pdf`.

## How to run

Pick your algorithm and run the command:

```
./main2 < entradas/0.txt
```

The input `entradas/0.txt` can be substituted by your own input file.