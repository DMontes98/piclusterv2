#!/bin/bash

# Wake nodes up before running batch job
#sudo scontrol update nodename=piclusterv2,p[1-4] state=idle

# Formatting
BLUE='\e[34m'
LIGHT_BLUE='\e[94m'
LIGHT_RED='\e[91m'
BOLD='\e[1m'
RESET='\e[0m'

echo -e "${LIGHT_BLUE}${BOLD}[SLURM]${RESET} Running ${BOLD}Multicore RT App${RESET} in ${LIGHT_RED}${BOLD}PiClusterV2-0${RESET}${RESET}"
# Run our program using OpenMPI. OpenMPI will automatically discover resources from SLURM.
sbatch --quiet --nodelist=piclusterv2,p[1-4] --job-name multicore-rt-app --cpus-per-task=1 --ntasks=20 ./slurm-submission.sh 

