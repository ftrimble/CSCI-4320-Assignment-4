#!/bin/sh
#
#SBATCH --job-name=exp1_mm_PCP3trmb
#SBATCH -t 00:30:00
#SBATCH -p small
#SBATCH -D /gpfs/sb/data/PCP3/PCP3trmb/experiment1
#SBATCH -O
#


#SBATCH --mail-type=ALL
#SBATCH --mail-user=trimbf@rpi.edu

numprocs=$1
numnodes=$2

srun -o 1by$numprocs.out --time 10 --nodes=$numnodes \
    --ntasks=$numprocs --overcommit --contiguous \
    --runjob-opts="--mapping TEDCBA" ../trimbf_mm
srun -o 2by$numprocs.out --time 10 --nodes=$numnodes \
    --ntasks=`echo "$numprocs*2" | bc` --overcommit --contiguous \
    --runjob-opts="--mapping TEDCBA" ../trimbf_mm
srun -o 4by$numprocs.out --time 10 --nodes=$numnodes \
    --ntasks=`echo "$numprocs*4" | bc` --overcommit --contiguous \
    --runjob-opts="--mapping TEDCBA" ../trimbf_mm
