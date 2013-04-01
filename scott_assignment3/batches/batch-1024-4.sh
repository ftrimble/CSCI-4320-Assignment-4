#!/bin/bash
#SBATCH --job-name=PCP3tddsMM1024-4
#SBATCH -t 00:10:00
#SBATCH -D /gpfs/sb/data/PCP3/PCP3tdds/
#SBATCH -O
#SBATCH -N 64
#SBATCH -n 4096
#SBATCH -p small


srun -o output-1024-4.txt --partition=small --time=10 --nodes=64 --ntasks=4096 --overcommit ./mm
