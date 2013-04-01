#!/bin/bash
#SBATCH --job-name=PCP3tddsMM64-1M
#SBATCH -t 00:10:00
#SBATCH -D /gpfs/sb/data/PCP3/PCP3tdds/
#SBATCH -O
#SBATCH -N 4
#SBATCH -n 64
#SBATCH -p small


srun -o output-64-1-mapping.txt --partition=small --time=10 --nodes=4 --ntasks=64 --overcommit --runjob-opts="--mapping TEDCBA" ./mm
