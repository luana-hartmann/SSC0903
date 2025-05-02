# Character Frequency Counter using OpenMP

This is a C program that reads lines of text from standard input (`stdin`) and counts the frequency of all visible ASCII characters (from 32 to 127). The purpose is to practice parallel programming using **OpenMP**.

## What the Program Does

For each line of input:
- Counts how many times each character appears.
- Prints the ASCII value and frequency of each character (one per line).
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

This project includes a simple `Makefile`. Just run:


````
make entrada.txt
````
