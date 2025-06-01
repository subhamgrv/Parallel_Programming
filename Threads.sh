#!/usr/bin/env bash
#SBATCH --partition=all
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=10          # Number of OMP threads
#SBATCH --nodes=1
#SBATCH --job-name=my_job_name
#SBATCH --output=my_job.out
#SBATCH --time=00:10:00

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

gcc -o Sum_Threads Sum_Threads.c -lpthread
./Sum_Threads
