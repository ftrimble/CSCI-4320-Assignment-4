#!/bin/sh
#SBATCH --job-name=thread_mm
#SBATCH -t 00:10:00
#SBATCH -p small
#SBATCH -D /gpfs/sb/data/PCP3/PCP3trmb/assignment4/
#SBATCH -O
#
#SBATCH --mail-type=ALL
#SBATCH --mail-user=voricd@rpi.edu

numtasks=$1
numthreads=$2
MM=../mm_threaded

echo srun -o $numtasks\by$numthreads.out --ntasks=$numtasks \
    $MM $numtasks $numthreads
