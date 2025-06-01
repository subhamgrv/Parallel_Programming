/* Author : Subham Gaurav (fd0003361)
Description: This program calculates the sum of the first 100,000,000 natural numbers
in a single-threaded loop */

#include<stdio.h>
#include<omp.h>
#define N 100000000
int main() {
    long long total_sum = 0;

    // Omp_get_wtime() is used to measure the time taken for the computation as it is running in single thread
    double starting_time = omp_get_wtime();

  for(int i = 1; i <= N; i++) {
        total_sum += i;
    }
    
    double end_time = omp_get_wtime();
    printf("Time taken: %f seconds\n", end_time - starting_time);
    printf("Total sum: %lld\n", total_sum);

    return 0;
}