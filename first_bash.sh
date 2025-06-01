#!/usr/bin/env bash
#SBATCH --partition=all
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=64        # Number of OMP threads
#SBATCH --nodes=1
#SBATCH --job-name=Parallel_with_head_6
#SBATCH --output=my_job_Sections_50.out
#SBATCH --time=00:10:00

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

gcc -o sumwithopenmp sumwithopenmp.c -fopenmp
./sumwithopenmp


