/*
  Author: Subham Gaurav (fd0003361)
  Description: Using OpenMP with two parallel sections. Each section computes a partial sum,
  and the results are combined using a critical section.
 */


#include <stdio.h>
#include <omp.h>
#define N 100000000



int main(){

 
    long long total_sum = 0;

        double starting_time = omp_get_wtime();

    #pragma omp parallel sections
    {


        #pragma omp section 
        {
    long long partial_sum = 0;

            for (int i = 1; i < N/2; i++) {
                partial_sum += i;
            }
           #pragma omp critical
    {
        total_sum += partial_sum;
    }

        }


        #pragma omp section 
        {
    long long partial_sum = 0;

            for (int i = N/2; i <= N; i++) {
                partial_sum += i;
            }
            #pragma omp critical
    {
        total_sum += partial_sum;
    }



        }

    

    }

    double end_time = omp_get_wtime();

    printf("Time taken: %f seconds\n", end_time - starting_time);

    printf("Total sum: %lld\n", total_sum); 

    return 0;


}

//  With two different variable for each section have more time  at around 0.006000 seconds as soo on 

//  Using Critical it reduced the time to around 0.002000 seconds and soo on 

/*

[fd0003361@manager:~]$ cat my_job_Sections_50.out
Time taken: 0.269548 seconds
Total sum: 5000000050000000

*/