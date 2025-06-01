/* Author : Subham Gaurav (fd0003361)
Description: This program implements a QuickSort in parallel using OpenMP tasks with a cutoff threshold to reduce task overhead
*/

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<omp.h>
#define N 100000


void quickSort(int arr[], int start, int end){
    if(start<end){
        int pivot = arr[end];
        int p = start;
        for(int j=start; j<end; j++){
            if(arr[j]<pivot){
                int temp = arr[p];
                 arr[p]= arr[j]; 
                 arr[j]=temp;
                 p++;
            }
        

        }
        int temp = arr[p];
        arr[p]=arr[end];
        arr[end]=temp;


        #pragma omp task shared(arr)  if (p - 1 - start > 1000)  // Create a task only if the subarray is large enough will help in reducing overhead
         quickSort(arr,start,p-1);
        
        #pragma omp task shared(arr)  if (end - (p + 1) > 1000)
        quickSort(arr,p+1,end);

        #pragma omp taskwait
    }

}

void printarr (int arr[]){

    for(int i=0;i<N;i++){
        printf("%d ",arr[i]);
    }
      printf("\n");
}

int main(){

    srand(time(NULL));
int arr[N];
    for(int i =0;i<N;i++){
        arr[i]=rand()%100000;
    }
        // printarr(arr);

        double Starting_time = omp_get_wtime();



    #pragma omp parallel
     {
        #pragma omp single nowait
         {

    quickSort(arr,0,N-1);
        }
    }  

double end_time = omp_get_wtime();

    double time_taken = (end_time - Starting_time); 
    printf("Time taken for Parallel quicksort without Overhead N %d : %f seconds\n", N, time_taken);
    printf("Number of threads used: %d\n", omp_get_num_threads());
    // printarr(arr);

return 0;


}



/*
Time taken for quicksort with Different ArrayLenght on Cluster : Node 1 partition : all  with 64 threads

Array length N (Size) 	Parallel Time (No Overhead) (s) 
10                  	0.006295 
100 	                0.006148 
1,000 	                0.006232 
10,000 	                0.013880 
100,000 	            0.014640 
500,000 	            0.046309 
1,000,000 	            0.078146 


*/


/*
Executed the code on a cluster with different number of threads to see the performance difference with Array Length N = 1,000,000

Threads Used 	Time (s) 
2 	            0.508326 
4 	            0.188577 
8 	            0.140402 
16 	            0.072966 
32          	0.068468 
64 	            0.076879 
*/