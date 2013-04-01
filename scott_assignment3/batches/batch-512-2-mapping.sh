#!/bin/bash
#SBATCH --job-name=PCP3tddsMM512-2M
#SBATCH -t 00:10:00
#SBATCH -D /gpfs/sb/data/PCP3/PCP3tdds/
#SBATCH -O
#SBATCH -N 32
#SBATCH -n 1024
#SBATCH -p small


srun -o output-512-2-mapping.txt --partition=small --time=10 --nodes=32 --ntasks=1024 --overcommit --runjob-opts="--mapping TEDCBA" ./mm
