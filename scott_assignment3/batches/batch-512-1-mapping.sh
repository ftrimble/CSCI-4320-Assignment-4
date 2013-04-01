#!/bin/bash
#SBATCH --job-name=PCP3tddsMM512-1M
#SBATCH -t 00:10:00
#SBATCH -D /gpfs/sb/data/PCP3/PCP3tdds/
#SBATCH -O
#SBATCH -N 32
#SBATCH -n 512
#SBATCH -p small


srun -o output-512-1-mapping.txt --partition=small --time=10 --nodes=32 --ntasks=512 --overcommit --runjob-opts="--mapping TEDCBA" ./mm
