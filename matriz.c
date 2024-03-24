#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define TAMANHO 4

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Defina o tamanho da matriz (número de linhas e colunas)
    int n = 4; // Por exemplo, uma matriz 4x4

    // Aloque memória para as matrizes A, B e C
    double *matrizA = (double *)malloc(n * n * sizeof(double));
    double *matrizB = (double *)malloc(n * n * sizeof(double));
    double *matrizC = (double *)malloc(n * n * sizeof(double));
    double *A_local = (double *)malloc(n * n / size * sizeof(double));
    double *C_local = (double *)malloc(n * n / size * sizeof(double));

    // Inicialize as matrizes A e B (ou leia de arquivos, etc.)
    if (rank == 0)
    {
        // Inicialize as matrizes A e B na raiz
        // Exemplo: preenchendo-as com valores aleatórios
        for (int i = 0; i < n * n; i++)
        {
            matrizA[i] = rand() % 10; // Valores aleatórios entre 0 e 9
            matrizB[i] = rand() % 10;
        }
    }

    // Divida a matriz A em partes e distribua para os processos
    MPI_Scatter(matrizA, n * n / size, MPI_DOUBLE, A_local, n * n / size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Broadcast a matriz B para todos os processos
    MPI_Bcast(matrizB, n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Faça a multiplicação local (A_local * B) para cada processo
    for (int i = 0; i < n / size; i++)
    {
        for (int j = 0; j < n; j++)
        {
            double sum = 0.0;
            for (int k = 0; k < n; k++)
            {
                sum += A_local[i * n + k] * matrizB[k * n + j];
            }
            C_local[i * n + j] = sum;
        }
    }

    // Colete as partes resultantes da matriz C de cada processo
    MPI_Gather(C_local, n * n / size, MPI_DOUBLE, matrizC, n * n / size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Processo raiz imprime a matriz C completa
    if (rank == 0)
    {
        printf("Matriz C:\n");
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                printf("%.2f ", matrizC[i * n + j]);
            }
            printf("\n");
        }
    }

    // Libere a memória e finalize o MPI
    free(matrizA);
    free(matrizB);
    free(matrizC);
    free(A_local);
    free(C_local);
    MPI_Finalize();

    return 0;
}
