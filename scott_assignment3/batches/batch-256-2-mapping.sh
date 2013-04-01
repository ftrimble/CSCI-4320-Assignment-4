#!/bin/bash
#SBATCH --job-name=PCP3tddsMM256-2M
#SBATCH -t 00:10:00
#SBATCH -D /gpfs/sb/data/PCP3/PCP3tdds/
#SBATCH -O
#SBATCH -N 16
#SBATCH -n 512
#SBATCH -p small


srun -o output-256-2-mapping.txt --partition=small --time=10 --nodes=16 --ntasks=512 --overcommit --runjob-opts="--mapping TEDCBA" ./mm
