#!/usr/bin/env bash
#SBATCH --partition=all
#SBATCH --ntasks=400
#SBATCH --job-name=my_job_name
#SBATCH --output=my_job.out
#SBATCH --time=00:10:00

mpicc -o hello Fox_Algorithm_Task_B.c
mpirun ./hello