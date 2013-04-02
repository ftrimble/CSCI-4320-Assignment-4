#!/bin/sh

#SBATCH --job-name=exp2_mm_PCP3trmb
#SBATCH -t 00:10:00
#SBATCH -p small
#SBATCH -D /gpfs/sb/data/PCP3/PCP3trmb/experiment2
#SBATCH -O
#
#SBATCH --mail-type=ALL
#SBATCH --mail-user=trimbf@rpi.edu

numprocs=$1
numnodes=$2

srun -o 4by$numprocs.out --time 10 --nodes=$numnodes \
    --ntasks=`echo "$numprocs*4" | bc` --overcommit \
    --contiguous ../trimbf_mm



