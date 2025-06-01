/*
  Author: Subham Gaurav (fd0003361)
  Description: Parallel QuickSort implementation using OpenMP sections to sort subarrays concurrently.
  Implemented this code from https://www.geeksforgeeks.org/implementation-of-quick-sort-using-mpi-omp-and-posix-thread/ 
  and modified it to use the concept of pivot and sections.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define N 500000

int partition(int arr[], int start, int end) {
    int pivot = arr[end];
    int p = start;
    for (int j = start; j < end; j++) {
        if (arr[j] < pivot) {
            int temp = arr[p];
            arr[p] = arr[j];
            arr[j] = temp;
            p++;
        }
    }
    int temp = arr[p];
    arr[p] = arr[end];
    arr[end] = temp;
    return p;
}

void quickSort(int arr[], int start, int end) {
    if (start < end) {
        int p = partition(arr, start, end);
        quickSort(arr, start, p - 1);
        quickSort(arr, p + 1, end);
    }
}

void parallelQuickSortSections(int arr[], int start, int end) {
    if (start < end) {
        int p = partition(arr, start, end);
        
        #pragma omp parallel sections
        {
            #pragma omp section
            {
                quickSort(arr, start, p - 1);
            }

            #pragma omp section
            {
                quickSort(arr, p + 1, end);
            }
        }
    }
}

void printarr(int arr[]) {
    for (int i = 0; i < N; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    int arr[N];
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        arr[i] = rand() % 100000;
    }

    double start_time = omp_get_wtime();

    parallelQuickSortSections(arr, 0, N - 1);

    double end_time = omp_get_wtime();
    printf("Time taken for QuickSort using sections (N = %d): %f seconds\n", N, end_time - start_time);

    // printarr(arr); 

    return 0;
}



/*
Time taken for quicksort with Different ArrayLength on Cluster : Node 1 partition : all  with 64 threads

Array length N (Size) 	Sections Time (s) 
10 	                    0.006136 
100                 	0.006071 
1,000               	0.006055 
10,000              	0.008169 
100,000             	0.035131 
500,000 	            0.140462 
1,000,000           	0.243582 



*/

