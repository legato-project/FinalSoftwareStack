#!/bin/bash
#SBATCH --job-name=ompss-ee
#SBATCH --workdir=.
#SBATCH --output=ompss-ee_%j.out
#SBATCH --error=ompss-ee_%j.err
#SBATCH --cpus-per-task=48
#SBATCH --ntasks=1
#SBATCH --time=00:15:00
#SBATCH --qos=debug
PROGRAM=matmul-p

export NX_SMP_WORKERS=1
export NX_OPENCL_MAX_DEVICES=2 #max number of opencl devices (GPUs in this case) to use
export NX_OPENCL_DEVICE_TYPE=GPU

# Creating input file
touch test.in
echo "4096 4096 4096 3" > test.in

# Executing the program
./$PROGRAM

