#!/bin/bash
#SBATCH --job-name=PCP3tddsMM128-4
#SBATCH -t 00:10:00
#SBATCH -D /gpfs/sb/data/PCP3/PCP3tdds/
#SBATCH -O
#SBATCH -N 8
#SBATCH -n 512
#SBATCH -p small


srun -o output-128-4.txt --partition=small --time=10 --nodes=8 --ntasks=512 --overcommit ./mm
