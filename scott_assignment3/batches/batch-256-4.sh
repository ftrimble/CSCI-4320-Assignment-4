#!/bin/bash
#SBATCH --job-name=PCP3tddsMM256-4
#SBATCH -t 00:10:00
#SBATCH -D /gpfs/sb/data/PCP3/PCP3tdds/
#SBATCH -O
#SBATCH -N 16
#SBATCH -n 1024
#SBATCH -p small


srun -o output-256-4.txt --partition=small --time=10 --nodes=16 --ntasks=1024 --overcommit ./mm
