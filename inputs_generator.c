#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LINE_LENGTH 1000
#define MIN_ASCII 32
#define MAX_ASCII 126


void generate_random_line(char* buffer, int length) {
    for (int i = 0; i < length; i++) {
        buffer[i] = (char)(MIN_ASCII + rand() % (MAX_ASCII - MIN_ASCII + 1));
    }
    buffer[length] = '\0';
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Erro\n");
        return 1;
    }

    const char* filename = argv[1];
    int num_lines = atoi(argv[2]);

    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Erro ao abrir arquivo");
        return 1;
    }

    srand(time(NULL));  // inicializa gerador de aleatório

    char buffer[MAX_LINE_LENGTH + 1];

    for (int i = 0; i < num_lines; i++) {
        int line_length = rand() % MAX_LINE_LENGTH + 1;  // entre 1 e 1000
        generate_random_line(buffer, line_length);
        fprintf(file, "%s\n", buffer);
    }

    fclose(file);
    printf("Arquivo \"%s\" com %d linhas gerado com sucesso.\n", filename, num_lines);

    return 0;
}
