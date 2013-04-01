#!/bin/bash
#SBATCH --job-name=PCP3tddsMM1024-1M
#SBATCH -t 00:10:00
#SBATCH -D /gpfs/sb/data/PCP3/PCP3tdds/
#SBATCH -O
#SBATCH -N 64
#SBATCH -n 1024
#SBATCH -p small


srun -o output-1024-1-mapping.txt --partition=small --time=10 --nodes=64 --ntasks=1024 --overcommit --runjob-opts="--mapping TEDCBA" ./mm
