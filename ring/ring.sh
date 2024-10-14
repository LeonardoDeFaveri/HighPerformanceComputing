#!/bin/bash

#PBS -l select=1:ncpus=4:mem=2gb
#PBS -l walltime=0:01:00
#PBS -q short_cpuQ
module load mpich-3.2
pwd
mpirun.actual -n 4 ./HighPerformanceComputing/ring/ring