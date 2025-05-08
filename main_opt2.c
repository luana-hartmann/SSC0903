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
#define MAX_CHAR 126
#define INPUT_SIZE 1001
#define NUM_THREADS 3
#define CHUNK_SIZE 100

// Struct with char value and int frequency
typedef struct {
    int f;
    char c;
} char_freq;

/**
 * @brief Function to merge two halves of an array range in ascending order
 * @param array array to be sorted
 * @param start start of the range to be sorted
 * @param end end of the range to be sorted
 * @param mid mid point of the range to be sorted
 */
void merge(char_freq* array, int start, int end, int mid) {
    // Initial positions in the arrays
    int position_first_half = start;
    int position_copy = 0;
    int position_second_half = mid + 1;

    // Auxiliar array to store the sorted values in the given range
    char_freq* copy = malloc((end - start + 1) * sizeof(char_freq));

    // Sorts first by frequency then by char value
    while(position_first_half <= mid && position_second_half <= end) {
        if(array[position_first_half].f == array[position_second_half].f) { // If frequency values are equal
            if(array[position_first_half].c < array[position_second_half].c) // Sorts by char value
                copy[position_copy++] = array[position_first_half++];
            else
                copy[position_copy++] = array[position_second_half++];
        }
        else if(array[position_first_half].f < array[position_second_half].f) // Sorts by frequency
            copy[position_copy++] = array[position_first_half++];
        else
            copy[position_copy++] = array[position_second_half++];
    }

    // Copies the residue in the first half of the array
    #pragma omp simd
    for(int i = 0; i <= mid - position_first_half; i++)
        copy[position_copy + i] = array[position_first_half + i];

    // Copies the residue in the second half of the array
    #pragma omp simd
    for(int i = 0; i <= end - position_second_half; i++)
        copy[position_copy + i] = array[position_second_half + i];

    // Moves the copy to the original array
    #pragma omp simd
    for(int i = start; i <= end; i++)
        array[i] = copy[i - start];

    free(copy);
}

/**
 * @brief Merge sort function
 * @param array array to be sorted
 * @param start start of the range to be sorted
 * @param end end of the range to be sorted
 * @param remaining_threads number of threads in which to execute the merge sort
 */
void merge_sort(char_freq* array, int start, int end, int remaining_threads) {
    int mid = (end + start) / 2;

    if(start == end)
        return;

    // Parallel merge sort when the current level of subdivisions is closest to the chosen number of threads
    if(remaining_threads == 1) {
        #pragma omp task shared(array)
        merge_sort(array, start, mid, remaining_threads/2);

        #pragma omp task shared(array)
        merge_sort(array, mid + 1, end, remaining_threads/2);

        #pragma omp taskwait
    }
    // Sequential merge sort for all other levels of the merge tree
    else {
        merge_sort(array, start, mid, remaining_threads/2);
        merge_sort(array, mid + 1, end, remaining_threads/2);
    }

    // Merges the two sorted halves
    merge(array, start, end, mid);
}

/**
 * @brief Function to process string into a sorted char frequency vector
 * @param frequency frequency vector to be filled with the string information
 * @param string string to be processed
 */
void process_string(char_freq* frequency, char* string) {
    // Allocates a vector for each thread to receive the char frequency and sets its frequencies to 0 (more efficient than lock / unlock)
    int** count = malloc(NUM_THREADS * sizeof(int*));
    for(int i = 0; i < NUM_THREADS; i++)
        count[i] = calloc(MAX_CHAR - MIN_CHAR, sizeof(int));
    int string_size = strlen(string);

    // Divides the string in chunks to be processed
    for(int i = 0; i < string_size; i+=100) {
        // Generates new tasks to count the frequencies, using the already generated threads
        #pragma omp task firstprivate(i, string_size) shared(string, count)
        {
            int end = i + 100;
            if(end > string_size)
                end = string_size;
            // Increases frequency corresponding to the character in the string (and the thread executing)
            for(int j = i; j < end; j++)
                ++count[omp_get_thread_num()][string[j] - MIN_CHAR];
        }
    }
    #pragma omp taskwait
    
    // Frees string received as input
    free(string);

    // Reduction of the count vector using simd extension, putting the information in the frequency vector
    #pragma omp simd
    for(int j = MIN_CHAR; j < MAX_CHAR; j++) {
        frequency[j - MIN_CHAR].c = j;
    }
    for(int i = 0; i < NUM_THREADS; i++) {
        // Sets the char values in the frequency struct
        #pragma omp simd
        for(int j = 0; j < MAX_CHAR - MIN_CHAR; j++) {
            frequency[j].f += count[i][j];
        }
        free(count[i]);
    }
    free(count);

    // Sorts frequency vector
    merge_sort(frequency, 0, MAX_CHAR - MIN_CHAR - 1, NUM_THREADS);
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

    // Makes parallel  block
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        // Executes while EOF is not reached
        #pragma omp single
        {
            // Step size for the vector with the frequencies to print later
            int step = 10000;
            int num_steps = 1;
            char_freq** frequencies = malloc(num_steps * step * sizeof(char_freq*));

            // Calculates time
            double overhead = omp_get_wtime();
            overhead = omp_get_wtime() - overhead;
            double time = omp_get_wtime();
            
            int count = 0;
            // Generates a task for each string
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
                #pragma omp task shared(frequency, string)
                {
                    // Creates a task to process each input line in parallel
                    process_string(frequency, string);
                }
                // Increases the count of inputs
                ++count;
            }
            #pragma omp taskwait
            time = omp_get_wtime() - time - overhead;

            // Print expected output (comment for big inputs)
            /*for(int i = 0; i < count; i++) {
                for(int j = 0; j < MAX_CHAR - MIN_CHAR; j++) {
                    char_freq current = frequencies[i][j];
                    if(current.f == 0)
                        continue;
                    printf("%d %d\n", current.c, current.f);
                }
                free(frequencies[i]);
                printf("\n");
            }
            free(frequencies);*/

            printf("\nTime: %lf\n", time);
        }
    }

    // Frees input at the end of the program
    free(input);
    return 0;
}