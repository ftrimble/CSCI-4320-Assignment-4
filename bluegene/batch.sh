#!/bin/sh
#SBATCH --job-name=thread_mm
#SBATCH -t 00:10:00
#SBATCH -p small
#SBATCH -D /gpfs/sb/data/PCP3/PCP3tdds/
#SBATCH -O

numtasks=$1
numthreads=$2
MM=../mm_threaded

srun -o $numthreads\by$numtasks.out --partition=small --ntasks=$numtasks $MM 
