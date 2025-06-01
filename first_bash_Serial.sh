#!/usr/bin/env bash
#SBATCH --partition=all
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1          # Number of OMP threads
#SBATCH --nodes=1
#SBATCH --job-name=Parallel_with_head_6
#SBATCH --output=my_job_Serial_50.out
#SBATCH --time=00:10:00


gcc -fopenmp -o Sum_Serial Sum_Serial.c
./Sum_Serial


