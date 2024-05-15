#!/bin/bash

## Configure Cluster to run 4 tasks, one on each slot ##
#SBATCH -o ./console/slurm_console.out

# Run our program using OpenMPI. OpenMPI will automatically discover resources from SLURM.
mpirun --allow-run-as-root --hostfile ./mpi-config/hostfile ../build/multicore_rt_app