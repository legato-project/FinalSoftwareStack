#!/bin/bash
if [ x$1 == x ]; then
   echo Us: exec.sh smp-threads fpga-num
elif [ x$2 == x ]; then
   echo Us: exec.sh smp-threads fpga-num
else
#   OMP_NUM_THREADS=$1 NX_ARGS="--fpga-num=$2 --summary --fpga-max-pending-tasks=16" ./matmul-p-implements 2816 1 #1792 1
   echo OMP_NUM_THREADS=1 NX_ARGS="--smp-workers=$1 --fpga-num=$2 --summary --fpga-max-pending-tasks=16" ./matmul-p-implements 2816 1 #1792 1
   OMP_NUM_THREADS=1 NX_ARGS="--smp-workers=$1 --fpga-num=$2 --summary --fpga-max-pending-tasks=16" ./matmul-p-implements 2816 1 #1792 1
fi
