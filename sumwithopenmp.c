/*
  Author: Subham Gaurav (fd0003361)
  Description: Auto parallelization of a simple sum using OpenMP. It turns out to be the fastest
  among all the implementations in this project.
 */


#include <stdio.h>
#include <omp.h>

#define N 100000000


int main(){

    
    long long total_sum = 0;
    double starting_time = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp for reduction(+:total_sum)
        for (int i = 1; i <= N; i++) {
            total_sum += i;
        }
    }
    double end_time = omp_get_wtime();

printf("Time taken: %f seconds\n", end_time - starting_time);
printf("Total sum reductions: %lld\n", total_sum); 

    return 0;


}
/*
[fd0003361@manager:~]$ cat my_job_Sections_50.out
Time taken: 0.023426 seconds
Total sum reductions: 5000000050000000

*/
