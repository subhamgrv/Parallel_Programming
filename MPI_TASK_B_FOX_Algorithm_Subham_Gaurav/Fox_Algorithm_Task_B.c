/******************************************************************************
 * Parallel Matrix-Matrix Multiplication (Fox Algorithm)
 * Author      : Subham Gaurav  (fd0003361 / 1564925)
 *
 *
 * References
 * ----------------------------------------------------------------------------
 * - University of Nizhni Novgorod, Faculty of Computational Mathematics &
 *   Cybernetics. CS338 “Introduction to Parallel Programming”, Laboratory 1,
 *   Appendix 2 (pp. 32-36) – baseline functions such as
 *        ▸ CreateGridCommunicators          ▸ CheckerboardMatrixScatter
 *        ▸ ParallelResultCalculation        ▸ BblockCommunication …
 * http://www.hpcc.unn.ru/mskurs/LAB/ENG/DOC/Lab02.pdf
 *
 * - Peter Pacheco, *Parallel Programming with MPI* (Morgan‑Kaufmann, 1997),
 *   Section 5.3 “Implementation of MPI_Comm_split”, p. 125 – Row communicator .
 *   https://www.google.co.in/books/edition/Parallel_Programming_with_MPI/GufgfWSHt28C?hl=en&gbpv=1
 *
 * - Thomas, M.  “MPI Matrix Multiplication – Fox Algorithm”, slide 35,
 *   SDSU CS 605 lecture notes.  Broadcast-then-shift exposition.
 *   // https://edoras.sdsu.edu/~mthomas/sp17.605/lectures/MPI-MatMatMult.pdf
 *  
 * - row_wise_matrix_mult.c example from Parallel Programming AI5085 (SoSe 2025) –
 *   inspiration for simple random matrix initialisation.
 *
 * Mapping of this source file to Appendix-2 routines
 * ----------------------------------------------------------------------------
 *   ♦ MPI_Cart_create / MPI_Cart_coords  → CreateGridCommunicators
 *   ♦ MPI_Comm_split calls               → the two MPI_Cart_sub calls
 *   ♦ manual Send/Recv                   → CheckerboardMatrixScatter in
 *                                           DataDistribution
 *   ♦ Fox loop                           → ParallelResultCalculation
 *        - broadcast of A block          → ABlockCommunication
 *        - local C += A×B                → BlockMultiplication→SerialResultCalculation
 *        - cyclic Sendrecv_replace of B  → BblockCommunication
 *
 *
 * // Additional help in integration with MPI by OpenAI
 * 
 * NOTE: p must be a perfect square and MATRIX_SIZE divisible by √p.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

int N = 800;  // Matrix dimension (N x N)

void fillMatrix(int** matrix) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            matrix[i][j] = rand() % 10;
}

void printMatrix2D(int** matrix, int size) {
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            printf("%d ", matrix[row][col]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char* argv[]) {
    int my_rank, num_procs;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // printf("Process %d of %d started.\n", my_rank, num_procs);

    int grid = 0;
    while (grid * grid < num_procs) {
        grid++;
    }

    if ((grid * grid != num_procs) || (N % grid != 0)) {
        if (my_rank == 0) {
            printf("Error: Number of processes must be a perfect square and N divisible by sqrt(p)\n");
        }
        MPI_Finalize();
        return 1;
    }

    int block_size = N / grid;

    int** matA = NULL;
    int** matB = NULL;

    // Allocate local blocks
    int** localA = malloc(block_size * sizeof(int*));
    int** localB = malloc(block_size * sizeof(int*));
    int** localC = malloc(block_size * sizeof(int*));
    int** tempA = malloc(block_size * sizeof(int*));
    for (int i = 0; i < block_size; i++) {
        localA[i] = malloc(block_size * sizeof(int));
        localB[i] = malloc(block_size * sizeof(int));
        localC[i] = calloc(block_size, sizeof(int));
        tempA[i] = malloc(block_size * sizeof(int));
    }

    int dims[2] = { grid, grid };
    int wrap[2] = { 1, 1 };
    int coords[2];

    MPI_Comm cart_comm;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, wrap, 1, &cart_comm);
    MPI_Cart_coords(cart_comm, my_rank, 2, coords);
    int my_row = coords[0];
    int my_col = coords[1];

    MPI_Comm row_comm;
    MPI_Comm_split(cart_comm, my_row, my_col, &row_comm);

    if (my_rank == 0) {
        srand(time(NULL));
        matA = malloc(N * sizeof(int*));
        matB = malloc(N * sizeof(int*));
        for (int i = 0; i < N; i++) {
            matA[i] = malloc(N * sizeof(int));
            matB[i] = malloc(N * sizeof(int));
        }
        fillMatrix(matA);
        fillMatrix(matB);
        // printMatrix2D(matA, N);
        // printMatrix2D(matB, N);
    }

    // Scatter matA and matB to local blocks
    if (my_rank == 0) {
        for (int p = 0; p < num_procs; p++) {
            int coord[2];
            MPI_Cart_coords(cart_comm, p, 2, coord);
            int grid_row = coord[0];
            int grid_col = coord[1];
            for (int i = 0; i < block_size; i++) {
                for (int j = 0; j < block_size; j++) {
                    int valA = matA[grid_row * block_size + i][grid_col * block_size + j];
                    int valB = matB[grid_row * block_size + i][grid_col * block_size + j];
                    if (p == 0) {
                        localA[i][j] = valA;
                        localB[i][j] = valB;
                    } else {
                        MPI_Send(&valA, 1, MPI_INT, p, 0, MPI_COMM_WORLD);
                        MPI_Send(&valB, 1, MPI_INT, p, 1, MPI_COMM_WORLD);
                    }
                }
            }
        }
    } else {
        for (int i = 0; i < block_size; i++) {
            for (int j = 0; j < block_size; j++) {
                MPI_Recv(&localA[i][j], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
                MPI_Recv(&localB[i][j], 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
            }
        }
    }

    // FOX Algorithm core
    for (int step = 0; step < grid; step++) {
        int bcast_root = (my_row + step) % grid;

        if (my_col == bcast_root) {
            for (int i = 0; i < block_size; i++)
                for (int j = 0; j < block_size; j++)
                    tempA[i][j] = localA[i][j];
        }

        for (int i = 0; i < block_size; i++)
            MPI_Bcast(tempA[i], block_size, MPI_INT, bcast_root, row_comm);

        for (int i = 0; i < block_size; i++) {
            for (int j = 0; j < block_size; j++) {
                for (int k = 0; k < block_size; k++) {
                    localC[i][j] += tempA[i][k] * localB[k][j];
                }
            }
        }

        int src, dst;
        MPI_Cart_shift(cart_comm, 0, -1, &src, &dst);
        for (int i = 0; i < block_size; i++) {
            MPI_Sendrecv_replace(localB[i], block_size, MPI_INT, dst, 2, src, 2, cart_comm, &status);
        }
    }

    // Gather result
    if (my_rank == 0) {
        int** result = malloc(N * sizeof(int*));
        for (int i = 0; i < N; i++)
            result[i] = malloc(N * sizeof(int));

        // Copy localC from rank 0
        for (int i = 0; i < block_size; i++)
            for (int j = 0; j < block_size; j++)
                result[i][j] = localC[i][j];

        for (int p = 1; p < num_procs; p++) {
            int coord[2];
            MPI_Cart_coords(cart_comm, p, 2, coord);
            int row_offset = coord[0] * block_size;
            int col_offset = coord[1] * block_size;

            for (int i = 0; i < block_size; i++) {
                MPI_Recv(&result[row_offset + i][col_offset], block_size, MPI_INT, p, 3, MPI_COMM_WORLD, &status);
            }
        }

        // printMatrix2D(result, N);
        printf("Bottom-right element of result: %d\n", result[N - 1][N - 1]);

        for (int i = 0; i < N; i++) {
            free(matA[i]); free(matB[i]); free(result[i]);
        }
        free(matA); free(matB); free(result);
    } else {
        for (int i = 0; i < block_size; i++) {
            MPI_Send(localC[i], block_size, MPI_INT, 0, 3, MPI_COMM_WORLD);
        }
    }

    // Cleanup
    for (int i = 0; i < block_size; i++) {
        free(localA[i]);
        free(localB[i]);
        free(localC[i]);
        free(tempA[i]);
    }
    free(localA); free(localB); free(localC); free(tempA);

    MPI_Comm_free(&row_comm);
    MPI_Comm_free(&cart_comm);
    MPI_Finalize();
    return 0;
}
