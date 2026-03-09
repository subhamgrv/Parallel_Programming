// Circular shift logic by: 
// Matrix Multiplication and Fox’s Algorithm. (n.d.). Åbo Akademi University. Semantic Scholar
// From https://pdfs.semanticscholar.org/3d39/d9e575cdb1616ec082052e2be6bb6dd7d4f1.pdf (p. 10)
// Thomas, M. (2017). MPI Matrix Multiplication - Fox Algorithm. Slide 35.
// https://edoras.sdsu.edu/~mthomas/sp17.605/lectures/MPI-MatMatMult.pdf


// Matrix initialization logic  from course material:
// row_wise_matrix_mult.c  Parallel Programming - AI5085 (SoSe2025)


// Additional help in integration with MPI by OpenAI


//  TO run this code, use the command using Batch file using Processor square of the matrix size  or BY EDUMPI

////  Subham Gaurav fd0003361   1564925   FOX Algorithm



#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "mpi.h"

#define n 5  // Size of the matrix (n x n)

void fillArray(int** arr) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            arr[i][j] = rand() % 10;  // Random integers between 0 and 9
        }
    }
}

void printMatrix(int** arr) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    // Check if the number of processes is a perfect square and equal to matrix size squared  (For TASK A)
    if (n * n != size) {
        if (rank == 0)
            printf("Error: number of processes must equal n * n.\n");
        MPI_Finalize();
        return 1;
    }

    int row = rank / n;
    int col = rank % n;

    int a, b, c = 0;
    int** matrix_a = NULL;
    int** matrix_b = NULL;

    if (rank == 0) {
        srand(time(0));
        matrix_a = (int**)malloc(n * sizeof(int*));
        matrix_b = (int**)malloc(n * sizeof(int*));
        for (int i = 0; i < n; i++) {
            matrix_a[i] = (int*)malloc(n * sizeof(int));
            matrix_b[i] = (int*)malloc(n * sizeof(int));
        }

        fillArray(matrix_a);
        fillArray(matrix_b);

        // Used to verify the matrices and its multiplication
        // printMatrix(matrix_a);
        // printMatrix(matrix_b);

        for (int k = 0; k < size; k++) {
            int i = k / n;
            int j = k % n;
            if (k == 0) {
                a = matrix_a[i][j];
                b = matrix_b[i][j];
            } else {
                MPI_Send(&matrix_a[i][j], 1, MPI_INT, k, 0, MPI_COMM_WORLD);
                MPI_Send(&matrix_b[i][j], 1, MPI_INT, k, 1, MPI_COMM_WORLD);
            }
        }
    } else {
        MPI_Recv(&a, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&b, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    }

    MPI_Comm row_comm;
    MPI_Comm_split(MPI_COMM_WORLD, row, col, &row_comm);

    int a_bcast;
    for (int k = 0; k < n; k++) {
        int root = (row + k) % n;
        if (col == root) {
            a_bcast = a;
        }

        MPI_Bcast(&a_bcast, 1, MPI_INT, root, row_comm);
        c += a_bcast * b;


                // Shift B upward (circular)
        int send_to = ((row - 1 + n) % n) * n + col;
        int recv_from = ((row + 1) % n) * n + col;

        MPI_Sendrecv_replace(&b, 1, MPI_INT, send_to, 2, recv_from, 2, MPI_COMM_WORLD, &status);
    }

    // Gather results
    if (rank != 0) {
        MPI_Send(&c, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        int** matrix_c = (int**)malloc(n * sizeof(int*));
        for (int i = 0; i < n; i++) {
            matrix_c[i] = (int*)malloc(n * sizeof(int));
        }

        matrix_c[0][0] = c;
        for (int k = 1; k < size; k++) {
            int i = k / n;
            int j = k % n;
            MPI_Recv(&matrix_c[i][j], 1, MPI_INT, k, 3, MPI_COMM_WORLD, &status);
        }

        // Print the resulting matrix
        // printMatrix(matrix_c);
        printf("Solution lower right corner: %d\n", matrix_c[n - 1][n - 1]);

        for (int i = 0; i < n; i++) {
            free(matrix_a[i]);
            free(matrix_b[i]);
            free(matrix_c[i]);
        }

        free(matrix_a);
        free(matrix_b);
        free(matrix_c);
    }

    MPI_Comm_free(&row_comm);
    MPI_Finalize();
    return 0;
}
