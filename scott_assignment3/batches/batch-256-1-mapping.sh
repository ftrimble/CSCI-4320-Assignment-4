#!/bin/bash
#SBATCH --job-name=PCP3tddsMM256-1M
#SBATCH -t 00:10:00
#SBATCH -D /gpfs/sb/data/PCP3/PCP3tdds/
#SBATCH -O
#SBATCH -N 16
#SBATCH -n 256
#SBATCH -p small


srun -o output-256-1-mapping.txt --partition=small --time=10 --nodes=16 --ntasks=256 --overcommit --runjob-opts="--mapping TEDCBA" ./mm
