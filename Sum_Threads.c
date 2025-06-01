
/* Author : Subham Gaurav (fd0003361)
Description: This program calculates the sum of the first 100,000,000 natural numbers
 using multithreading with 10 threads. */

 
#include<stdio.h>
#include<pthread.h>

#define N 100000000
#define NUM_OF_THREADS 10
long long partial_sum[NUM_OF_THREADS];


void *sumpart(void *arg) {
    int thread_id = *(int*)arg;
    long start = thread_id * (N / NUM_OF_THREADS) + 1;
    long end = (thread_id + 1) * (N / NUM_OF_THREADS);
    long long sum = 0;
    for (long i = start; i <= end; i++) {
        sum += i;
    }
    partial_sum[thread_id] = sum;
    return NULL; 
}

int main (){
    pthread_t threads[NUM_OF_THREADS];
    int thread_ids[NUM_OF_THREADS];
    for(int i =0;i<NUM_OF_THREADS;i++){
        thread_ids[i]=i;
        if(pthread_create(&threads[i],NULL,&sumpart,&thread_ids[i])){
            printf("Error: Could not create thread %d\n", i);
            return 1;
        }
    }
    for(int i =0; i<NUM_OF_THREADS;i++){
    if(pthread_join(threads[i],NULL)){
            printf("Error: Was not able to join thread %d\n", i);
        return 1;
    }
}

long long total_sum=0;
for(int i=0;i<NUM_OF_THREADS;i++){
    total_sum+=partial_sum[i];
}
printf("Total sum : %lld\n", total_sum);
return 0;
}


/*
[fd0003361@manager:~]$ sbatch Threads.sh
Submitted batch job 11882

[fd0003361@manager:~]$ cat my_job.out
Total sum : 5000000050000000
// Time taken: 0.521109 seconds (example output, actual time may vary)
*/