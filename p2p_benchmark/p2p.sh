#!/bin/bash

#PBS -l select=2:ncpus=1:mem=2gb -l place=scatter:excl
#PBS -l walltime=00:05:00
#PBS -q short_cpuQ

module load mpich-3.2
mpirun.actual -n 2 ./HighPerformanceComputing/p2p_benchmark/p2p