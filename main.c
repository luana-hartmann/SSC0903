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

int main() {
    char line[1001]; // buffer that stores one line (1000 characters max)
    int frequency[96]; // frequency for ASCII characters from 32 to 127

    // reads each line
    while (fgets(line, sizeof(line), stdin) != NULL) {

        // reset frequency to 0 for each line
        for (int i = 0; i < 96; i++) {
            frequency[i] = 0;
        }

        // count frequency of each ASCII character
        for (int i = 0; line[i] != '\0'; i++) {
            unsigned char c = line[i];
            if (c >= 32 && c <= 127) { // only consider ASCII 32 to 127
                frequency[c - 32]++;
            }
        }

        // print the frequencies found in this line (not sorted yet)
        for (int i = 0; i < 96; i++) {
            if (frequency[i] > 0) {
                printf("%d %d\n", i + 32, frequency[i]);
            }
        }

        printf("\n");
    }

    return 0;
}
