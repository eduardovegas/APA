#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <stdlib.h>

void read_data(int argc, char** argv, int* n, int* m, int* p, int** b, int*** t, int*** c, int** cur_capacities);
void print_data(int n, int m, int p, int* b, int **t, int **c);
void free(int m, int** b, int*** t, int*** c);

#endif