#include "data.h"

void read_data(int argc, char** argv, int* n, int* m, int* p, int** b, int ***t, int ***c, int** cur_capacities)
{
    if (argc != 3)
    {
        printf("Incorrect number of params\n");
        exit(1);
    }

    char* instance = argv[1];

    FILE* f = fopen(instance, "r");

    if(!f)
    {
        printf("Couldn't read %s\n", instance);
        exit(1);
    }

    if(fscanf(f, "%d", n) != 1)
    {
        printf("Couldn't read 'n'\n");
        exit(1);
    }

    if(fscanf(f, "%d", m) != 1)
    {
        printf("Couldn't read 'm'\n");
        exit(1);
    }

    if(fscanf(f, "%d", p) != 1)
    {
        printf("Couldn't read 'p'\n");
        exit(1);
    }

    *b = (int*) malloc((*m) * sizeof(int));
    *cur_capacities = (int*) malloc((*m) * sizeof(int));
    for(int i = 0; i < *m; i++)
    {
        if(fscanf(f, "%d", (*b)+i) != 1)
        {
            printf("Couldn't read 'b'\n");
            exit(1);
        }
        cur_capacities[i] = b[i];
    }

    *t = (int**) malloc((*m) * sizeof(int*));
    for(int i = 0; i < *m; i++)
    {
        (*t)[i] = (int*) malloc((*n) * sizeof(int));
        for(int j = 0; j < *n; j++)
        {
            if(fscanf(f, "%d", ((*t)[i])+j) != 1)
            {
                printf("Couldn't read 't'\n");
                exit(1);
            }
        }
    }

    *c = (int**) malloc((*m) * sizeof(int*));
    for(int i = 0; i < *m; i++)
    {
        (*c)[i] = (int*) malloc((*n) * sizeof(int));
        for(int j = 0; j < *n; j++)
        {
            if(fscanf(f, "%d", ((*c)[i])+j) != 1)
            {
                printf("Couldn't read 'c'\n");
                exit(1);
            }
        }
    }

    fclose(f);

    return;
}

void print_data(int n, int m, int p, int* b, int **t, int **c)
{
    printf("n: %d\n", n);
    printf("m: %d\n", m);
    printf("p: %d\n", p);
    puts("");

    printf("b: ");
    for(int i = 0; i < m; i++)
    {
        printf("%d ", b[i]);
    }
    puts("\n");

    printf("t:\n");
    for(int i = 0; i < m; i++)
    {
        for(int j = 0; j < n; j++)
        {
            printf("%d ", t[i][j]);
        }
        puts("");
    }
    puts("");
    
    printf("c:\n");
    for(int i = 0; i < m; i++)
    {
        for(int j = 0; j < n; j++)
        {
            printf("%d ", c[i][j]);
        }
        puts("");
    }
    puts("");
    return;
}

void free(int m, int** b, int ***t, int ***c)
{
    free(*b);

    for(int i = 0; i < m; i++)
    {
        free((*t)[i]);
        free((*c)[i]);
    }
    free(*t);
    free(*c);

    *b = NULL;
    *t = NULL;
    *c = NULL;

    return;
}