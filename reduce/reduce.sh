#!/bin/bash

#PBS -l select=1:ncpus=8:mem=2gb
#PBS -l walltime=00:01:30
#PBS -q short_cpuQ

module load mpich-3.2
mpirun.actual -n 8 ./HighPerformanceComputing/reduce/reduce