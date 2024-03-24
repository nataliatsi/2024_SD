#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MAX_LINHAS 10
#define MAX_COLUNAS 10

void mult_matrizxmatriz(long int *matriz1, long int *matriz2, long int *resultado, int linhas1, int colunas1, int colunas2, int linhas_por_processo)
{
    for (int i = 0; i < linhas_por_processo; i++)
    {
        for (int j = 0; j < colunas2; j++)
        {
            resultado[i * colunas2 + j] = 0;
            for (int k = 0; k < colunas1; k++)
            {
                resultado[i * colunas2 + j] += matriz1[i * colunas1 + k] * matriz2[k * colunas2 + j];
            }
        }
    }
}

int main(int argc, char **argv)
{
    long int *matriz1 = NULL, *matriz2 = NULL, *resultado = NULL;
    int linhas1 = 0, colunas1 = 0, linhas2 = 0, colunas2 = 0;
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int linhas_por_processo;
    int linhas_extras;

    if (rank == 0)
    {
        matriz1 = (long int *)malloc(sizeof(long int) * MAX_LINHAS * MAX_COLUNAS);
        matriz2 = (long int *)malloc(sizeof(long int) * MAX_LINHAS * MAX_COLUNAS);
        resultado = (long int *)malloc(sizeof(long int) * MAX_LINHAS * MAX_COLUNAS);
    }

    while ((scanf("%d %d", &linhas1, &colunas1) == 2) && linhas1 > 0 && colunas1 > 0)
    {
        if (rank == 0)
        {
            for (int i = 0; i < linhas1; i++)
            {
                for (int j = 0; j < colunas1; j++)
                {
                    if (scanf("%ld", &matriz1[i * colunas1 + j]) != 1)
                        exit(1);
                }
            }

            linhas2 = colunas1;

            if (scanf("%d", &colunas2) != 1)
                exit(1);

            for (int i = 0; i < linhas2; i++)
            {
                for (int j = 0; j < colunas2; j++)
                {
                    if (scanf("%ld", &matriz2[i * colunas2 + j]) != 1)
                        exit(1);
                }
            }
        }

        MPI_Bcast(&linhas1, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&colunas1, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&colunas2, 1, MPI_INT, 0, MPI_COMM_WORLD);

        if (rank == 0)
        {
            linhas_por_processo = linhas1 / size;
            linhas_extras = linhas1 % size;
        }

        MPI_Bcast(&linhas_por_processo, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&linhas_extras, 1, MPI_INT, 0, MPI_COMM_WORLD);

        if (rank != 0)
        {
            matriz1 = (long int *)malloc(sizeof(long int) * (linhas_por_processo + (rank < linhas_extras ? 1 : 0)) * colunas1);
            matriz2 = (long int *)malloc(sizeof(long int) * linhas2 * colunas2);
            resultado = (long int *)malloc(sizeof(long int) * (linhas_por_processo + (rank < linhas_extras ? 1 : 0)) * colunas2);
        }

        MPI_Bcast(matriz1, linhas1 * colunas1, MPI_LONG, 0, MPI_COMM_WORLD);
        MPI_Bcast(matriz2, linhas2 * colunas2, MPI_LONG, 0, MPI_COMM_WORLD);

        mult_matrizxmatriz(matriz1 + rank * linhas_por_processo * colunas1,
                    matriz2, resultado, linhas_por_processo + (rank < linhas_extras ? 1 : 0), colunas1, colunas2, colunas1);

       
        for (int i = 0; i < linhas1; i++)
        {
            for (int j = 0; j < colunas2; j++)
            {
                printf("%2ld ", resultado[i * colunas2 + j]);
            }
            printf("\n");
        }
        printf("\n");

        free(matriz1);
        free(matriz2);
        free(resultado);
    }

    MPI_Finalize();
    return 0;
}
