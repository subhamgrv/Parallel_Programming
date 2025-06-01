/* Author : Subham Gaurav (fd0003361)
Description: This program implements a serial QuickSort to sort an array of size N
*/

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include <omp.h>
#define N 500000

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

        
        quickSort(arr,start,p-1);
        quickSort(arr,p+1,end);


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
        arr[i]=rand()%10000;
    }
        // printarr(arr);

         double Starting_time = omp_get_wtime();


    quickSort(arr,0,N-1);
  
      double end_time = omp_get_wtime();


    double time_taken = (end_time - Starting_time); 
    printf("Time taken for serial quicksort with openMPTIme N %d : %f seconds\n", N, time_taken);

    // printarr(arr);

return 0;
}



// Used OpenMP instead of Clock() for timing the execution of the serial quicksort algorithm to accurately test with other parallel implementations.

/*
Time taken for serial quicksort with Different ArrayLength on Cluster : Node 1 partition : all  with 64 threads

Array length N (Size) 	Serial Time (s) 
10 	                    0.000001 
100 	                0.000013 
1,000 	                0.000180 
10,000              	0.002377 
100,000             	0.030481 
500,000             	0.207337 
1,000,000           	0.563983 

*/

