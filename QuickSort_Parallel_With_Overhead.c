/* Author : Subham Gaurav (fd0003361)
  Description: Parallel QuickSort using OpenMP tasks without cutoff, creating tasks for all subarrays to maximize concurrency.
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


        #pragma omp task shared(arr)   // No size condition here, we will always create tasks even for small subarrays
         quickSort(arr,start,p-1);
        
        #pragma omp task shared(arr) 
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
    printf("Time taken for Parallel quicksort with Overhead N %d : %f seconds\n", N, time_taken);
    printf("Number of threads used: %d\n", omp_get_num_threads());
    // printarr(arr);

return 0;


}



/*
Array length N (Size) 	Parallel Time (With Overhead) (s) 
10 	                    0.008756 
100                 	0.015452 
1,000 	                0.041944 
10,000 	                0.302786 
100,000             	3.702799 
500,000 	            23.610367 
1,000,000 	            55.205389 

    Note: This implementation lead to significant overhead due to task creation for small subarrays.
*/