#!/bin/bash
#SBATCH --job-name=PCP3tddsMM128-1M
#SBATCH -t 00:10:00
#SBATCH -D /gpfs/sb/data/PCP3/PCP3tdds/
#SBATCH -O
#SBATCH -N 8
#SBATCH -n 128
#SBATCH -p small


srun -o output-128-1-mapping.txt --partition=small --time=10 --nodes=8 --ntasks=128 --overcommit --runjob-opts="--mapping TEDCBA" ./mm
