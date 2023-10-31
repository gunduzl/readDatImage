#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


void processInputData(int ***myARR, int *N_dimension) {
    char input[] = "data.dat";
    FILE *input_file;

    if ((input_file = fopen(input, "r")) == NULL) {
        perror("Failed to open input input_file");
        exit(1);
    }

    fscanf(input_file, "%d", N_dimension);

    *myARR = (int **)malloc(*N_dimension * sizeof(int *));
    for (int i = 0; i < *N_dimension; i++) {
        (*myARR)[i] = (int *)malloc(*N_dimension * sizeof(int));
    }

    for (int i = 0; i < *N_dimension; i++) {
        for (int j = 0; j < *N_dimension; j++) {
            if (fscanf(input_file, "%d", &(*myARR)[i][j]) != 1) {
                perror("Failed to read input data");
                exit(1);
            }
        }
    }

    fclose(input_file);
}

void processChild(int IDofChild, int **myARR, int N_dimension) {
    int *modified = (int *)malloc(N_dimension * sizeof(int));
    for (int i = 0; i < N_dimension; i++) {
        if (myARR[IDofChild][i] <= 127) {
            modified[i] = 0;
        } else {
            modified[i] = 255;
        }
    }

    char input[20];
    snprintf(input, sizeof(input), "%d.txt", IDofChild);

    FILE *input_file = fopen(input, "w");

    for (int j = 0; j < N_dimension; j++) {
        fprintf(input_file, "%d\n", modified[j]);
    }

    fclose(input_file);

    exit(0);
}

int main() {
    int **myARR;
    int N_dimension;
    processInputData(&myARR, &N_dimension);

    pid_t child_pid, wpid;
    int status = 0;

    for (int IDofChild = 0; IDofChild < N_dimension; IDofChild++) {
        child_pid = fork();
        if (child_pid == -1) {
            perror("Failed to fork");
            exit(1);
        } else if (child_pid == 0) {
            processChild(IDofChild, myARR, N_dimension);
        } else {
            switch (child_pid) {
                case -1:
                    perror("Failed to fork");
                    exit(1);
                case 0:
                    processChild(IDofChild, myARR, N_dimension);
                    break;
                default:
                    wait(&status);
                    break;
            }
        }
    }

    return 0;
}
