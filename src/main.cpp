#include "data.h"

int n;
int m;
int p;
int* b;
int** t;
int** c;

int main(int argc, char** argv)
{
    read_data(argc, argv, &n, &m, &p, &b, &t, &c);
    print_data(n, m, p, b, t, c);
    free(m, &b, &t, &c);

    return 0;
}