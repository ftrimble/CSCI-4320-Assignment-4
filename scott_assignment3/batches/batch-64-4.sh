#!/bin/bash
#SBATCH --job-name=PCP3tddsMM64-4
#SBATCH -t 00:10:00
#SBATCH -D /gpfs/sb/data/PCP3/PCP3tdds/
#SBATCH -O
#SBATCH -N 4
#SBATCH -n 256
#SBATCH -p small


srun -o output-64-4.txt --partition=small --time=10 --nodes=4 --ntasks=256 --overcommit ./mm
