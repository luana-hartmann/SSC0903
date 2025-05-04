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

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<omp.h>

// Important constants
#define MIN_CHAR 32
#define MAX_CHAR 126
#define INPUT_SIZE 1001
#define NUM_THREADS 4
#define CHUNK_SIZE 100

typedef struct {
    int f;
    char c;
} char_freq;


void merge(char_freq* array, int start, int end, int mid) {
    int position_first_half = start;
    int position_copy = 0;
    int position_second_half = mid + 1;
    char_freq* copy = malloc((end - start + 1) * sizeof(char_freq));

    while(position_first_half <= mid && position_second_half <= end) {
        if(array[position_first_half].f == array[position_second_half].f) {
            if(array[position_first_half].c < array[position_second_half].c)
                copy[position_copy++] = array[position_first_half++];
            else
                copy[position_copy++] = array[position_second_half++];
        }
        else if(array[position_first_half].f < array[position_second_half].f)
            copy[position_copy++] = array[position_first_half++];
        else
            copy[position_copy++] = array[position_second_half++];
    }

    // Copies the residue in the first half
    #pragma omp simd
    for(int i = 0; i <= mid - position_first_half; i++)
        copy[position_copy + i] = array[position_first_half + i];

    // Copies the residue in the second half
    #pragma omp simd
    for(int i = 0; i <= end - position_second_half; i++)
        copy[position_copy + i] = array[position_second_half + i];

    #pragma omp simd
    for(int i = start; i <= end; i++)
        array[i] = copy[i - start];

    free(copy);
}

void merge_sort(char_freq* array, int start, int end, int remaining_threads) {
    int mid = (end + start) / 2;

    if(start == end)
        return;

    if(remaining_threads == 1) {
        #pragma omp task shared(array)
        merge_sort(array, start, mid, remaining_threads/2);

        #pragma omp task shared(array)
        merge_sort(array, mid + 1, end, remaining_threads/2);

        #pragma omp taskwait
    } else {
        merge_sort(array, start, mid, remaining_threads/2);
        merge_sort(array, mid + 1, end, remaining_threads/2);
    }

    merge(array, start, end, mid);
}

char_freq* process_string(char* string) {
    // Allocates a vector for each thread to receive the char frequency and sets its frequencies to 0
    int** count = malloc(NUM_THREADS * sizeof(int*));
    for(int i = 0; i < NUM_THREADS; i++)
        count[i] = calloc(MAX_CHAR - MIN_CHAR, sizeof(int));
    int string_size = strlen(string);

    #pragma omp parallel for schedule(dynamic, CHUNK_SIZE)
    for(int i = 0; i < string_size; i++) {
        // Increases frequency corresponding to the character in the string (and the thread executing)
        ++count[omp_get_thread_num()][string[i] - MIN_CHAR];
    }
    //printf("Saiu\n\n");
    // Frees string received as input
    free(string);

    // Reduction of the count vector using simd extension (quicker than using multiple threads)
    char_freq* frequency = calloc(MAX_CHAR - MIN_CHAR, sizeof(char_freq));
    #pragma omp simd
    for(int j = MIN_CHAR; j < MAX_CHAR; j++) {
        frequency[j - MIN_CHAR].c = j;
    }
    for(int i = 0; i < NUM_THREADS; i++) {
        #pragma omp simd
        for(int j = 0; j < MAX_CHAR - MIN_CHAR; j++) {
            frequency[j].f += count[i][j];
        }
        free(count[i]);
    }
    free(count);

    // Sort frequency vector
    merge_sort(frequency, 0, MAX_CHAR - MIN_CHAR - 1, NUM_THREADS);

    return frequency;
}

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

    // Makes parallel  block
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        // Executes while EOF is not reached
        #pragma omp single
        {
            int step = 100;
            char_freq** frequencies = malloc(step * sizeof(char_freq*));

            // Calculates time
            double overhead = omp_get_wtime();
            overhead = omp_get_wtime() - overhead;
            double time = omp_get_wtime();
            
            int count = 0;
            // Generates a task for each string
            while(fgets(input, INPUT_SIZE, stdin) != NULL) {
                if(count >= step) {
                    step += 100;
                    frequencies = realloc(frequencies, step * sizeof(char_freq*));
                }
                
                // Removes '\n' and '\r' from the string
                sanitize_string(input);

                // Duplicates string to use in the task
                char* string = strdup(input);
                #pragma omp task firstprivate(count) shared(frequencies, string)
                {
                    // Creates a task to process each input line in parallel
                    frequencies[count] = process_string(string);
                }
                // Increases the count of inputs
                ++count;
            }
            #pragma omp taskwait
            time = omp_get_wtime() - time - overhead;
            printf("Time: %lf\n\n", time);

            // Print expected output
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
        }
    }

    // Frees input at the end of the program
    free(input);
    return 0;
}