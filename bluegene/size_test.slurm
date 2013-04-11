#!/bin/sh
#SBATCH --job-name=size_mm
#SBATCH --nodes=64
#SBATCH -t 01:00:00
#SBATCH -p small
#SBATCH -D /gpfs/sb/data/PCP3/PCP3trmb/
#SBATCH -O

MM=mm_threaded
numtasks=`echo 64*4 | bc`

for i in `seq 12 16`; do
    matrixsize=`echo "2^$i" | bc`
    srun -o $matrixsize.out --nodes=64 --ntasks=$numtasks $MM $matrixsize
done
