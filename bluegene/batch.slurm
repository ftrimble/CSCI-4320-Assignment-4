#!/bin/sh
#SBATCH --job-name=thread_mm
#SBATCH --nodes=64
#SBATCH -t 02:00:00
#SBATCH -p small
#SBATCH -D /gpfs/sb/data/PCP3/PCP3trmb/
#SBATCH -O

MM=mm_threaded

for i in `seq 0 6`; do
    numtasks=`echo "2^$i" | bc`
    numthreads=`echo "64/$numtasks" | bc`
    srun -o $numthreads\by$numtasks.out --nodes=64 --ntasks=$numtasks $MM 
done

for i in `seq 12 16`; do
    matrixsize=`echo "2^$i" | bc`
    numtasks=`echo "2^($i-2)" | bc`
    srun -o $matrixsize.out --nodes=64 --ntasks=$numtasks $MM $matrixsize
done