/*
Trabalho 1 da disciplina SSC0903 - Computação de Alto Desempenho
Integrantes:
Guilherme Lorete Schmidt - NUSP: 13676857
João Victor Breches Alves - NUSP: 13677142
Luana Hartmann Franco da Cruz - NUSP: 13676350
Lucas Corlete Alves de Melo - NUSP: 13676461
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

// Important constants
#define MIN_CHAR 32
#define MAX_CHAR 127 // One above the max admissable char
#define INPUT_SIZE 1001
#define CHUNK_SIZE 100

// Struct with char value and int frequency
typedef struct {
    int f;
    char c;
} char_freq;

/**
 * @brief Quicksort comparing function
 * @param a first char / frequency struct
 * @param b second char / frequency struct
 * @return negative if 'a' goes first, positive if 'b' goes first
 */
int compare_char_freq(const void* a, const void* b) {
    const char_freq* x = (const char_freq*) a;
    const char_freq* y = (const char_freq*) b;

    if (x->f == y->f) // If frequencies are equal
        return (x->c - y->c); // Sorts by lower char value
    return (x->f - y->f); // Sorts by lower frequency
}

/**
 * @brief Function to process string into a sorted char frequency vector
 * @param frequency frequency vector to be filled with the string information
 * @param string string to be processed
 */
void process_string(char_freq* frequency, char* string) {
    int string_size = strlen(string);

    // Increases frequency corresponding to the character in the string
    for(int i = 0; i < string_size; i++) {
        unsigned char ch = string[i];
        if (ch < MIN_CHAR || ch >= MAX_CHAR) // checks for invalid chars
            continue;
        ++frequency[string[i] - MIN_CHAR].f;
    }
    free(string);

    // Sets the char values in the frequency struct
    for(int j = MIN_CHAR; j < MAX_CHAR; j++) {
        frequency[j - MIN_CHAR].c = j;
    }

    // Sorts frequency vector using quicksort
    qsort(frequency, MAX_CHAR - MIN_CHAR, sizeof(char_freq), compare_char_freq);
}

/**
 * @brief Removes newline and carrier return chars from the given string
 * @param string input string
 */
void sanitize_string(char* string) {
    int string_size = strlen(string);
    string[string_size - 1] = '\0';
    string_size = strlen(string);
    if(string[string_size - 1] == '\r')
        string[string_size - 1] = '\0';
}

int main() {
    // Allocates memory for the input
    char* input = malloc(INPUT_SIZE * sizeof(char));

    // Step size for the vector with the frequencies to print later
    int step = 10000;
    int num_steps = 1;
    char_freq** frequencies = malloc(num_steps * step * sizeof(char_freq*));

    int count = 0;

    // Calculates time
    double overhead = omp_get_wtime();
    overhead = omp_get_wtime() - overhead;
    double time = omp_get_wtime();
    
    // Executes while EOF is not reached
    while(fgets(input, INPUT_SIZE, stdin) != NULL) {
        // realloc frequencies vector
        if(count >= num_steps * step) {
            ++num_steps;
            frequencies = realloc(frequencies, num_steps * step * sizeof(char_freq*));
        }
        
        // Removes '\n' and '\r' from the string
        sanitize_string(input);
        
        // Duplicates string to use in the task
        char* string = strdup(input);
        
        // Takes imutable frequency pointer to use in the task
        char_freq* frequency = frequencies[count] = calloc(MAX_CHAR - MIN_CHAR, sizeof(char_freq));

        // Processes each input line
        process_string(frequency, string);

        // Increases the count of inputs
        ++count;
    }
    time = omp_get_wtime() - time - overhead;

    // Print expected output (comment for big inputs)
    for(int i = 0; i < count; i++) {
        for(int j = 0; j < MAX_CHAR - MIN_CHAR; j++) {
            char_freq current = frequencies[i][j];
            if(current.f == 0)
                continue;
            printf("%d %d\n", current.c, current.f);
        }
        free(frequencies[i]);
        printf("\n");
    }
    free(frequencies);

    // printf("%lf\n", time); // Uncomment if you want to see the execution time

    // Frees input at the end of the program
    free(input);
    return 0;
}