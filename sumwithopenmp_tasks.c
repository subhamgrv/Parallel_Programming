/*
  Author: Subham Gaurav (fd0003361)
  Description: Using OpenMP with two tasks. Can Further optimize by increasing the number of tasks
 */

#include <stdio.h>
#include <omp.h>

#define N 100000000

int main() {
    long long total_sum = 0;
    double starting_time = omp_get_wtime();

    #pragma omp parallel
    {

        // Such that only one thread create the tasks
        #pragma omp single    
        {
            long long partialSum1 = 0;
            long long partialSum2 = 0;

            #pragma omp task shared(partialSum1)
            {
                for (int i = 1; i < N / 2; i++) {
                    partialSum1 += i;
                }
            }

            #pragma omp task shared(partialSum2)
            {
                for (int i = N / 2; i <= N; i++) {
                    partialSum2 += i;
                }
            }

            #pragma omp taskwait

            total_sum = partialSum1 + partialSum2;
        }
    }
        double end_time = omp_get_wtime();

printf("Time taken: %f seconds\n", end_time - starting_time);

    printf("Total sum in tasks : %lld\n", total_sum);
    return 0;
}


// Tried with sharing same partial sum variable like sections, But was overlapping the values cause of Race Condition

/*
[fd0003361@manager:~]$ cat my_job_Sections_50.out
Time taken: 1.836111 seconds
Total sum in tasks : 5000000050000000


*/