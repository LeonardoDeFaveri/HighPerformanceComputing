#!/bin/bash

#PBS -l select=1:ncpus=2:mem=2gb
#PBS -l walltime=0:01:00
#PBS -q short_cpuQ
module load mpich-3.2
pwd
mpirun.actual -n 2 ./HighPerformanceComputing/ping_pong/ping_pong 5