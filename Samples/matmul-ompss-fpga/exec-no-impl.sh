#!/bin/bash
if [ x$1 == x ]; then
   echo Us: exec.sh smp-threads fpga-num
elif [ x$2 == x ]; then
   echo Us: exec.sh smp-threads fpga-num
else
   #OMP_NUM_THREADS=$1 NX_ARGS="--fpga-num=$2 --summary --no-fpga-hybrid-worker --no-fpga-idle-callback --fpga-max-pending-tasks=8 " ./matmul-p-no-implements 1792 1
#   OMP_NUM_THREADS=$1 NX_ARGS="--fpga-num=$2 --summary --fpga-max-pending-tasks=16 " ./matmul-p-no-implements-flush 1792 1
   OMP_NUM_THREADS=$1 NX_ARGS="--fpga-num=$2 --summary --fpga-max-pending-tasks=16 " ./matmul-p-no-implements 2816 1
fi
