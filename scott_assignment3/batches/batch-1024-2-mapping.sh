#!/bin/bash
#SBATCH --job-name=PCP3tddsMM1024-2M
#SBATCH -t 00:10:00
#SBATCH -D /gpfs/sb/data/PCP3/PCP3tdds/
#SBATCH -O
#SBATCH -N 64
#SBATCH -n 2048
#SBATCH -p small


srun -o output-1024-2-mapping.txt --partition=small --time=10 --nodes=64 --ntasks=2048 --overcommit --runjob-opts="--mapping TEDCBA" ./mm