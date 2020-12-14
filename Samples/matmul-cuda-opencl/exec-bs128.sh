#!/bin/bash
if [ x$1 == x ]; then
   echo Us: $0 '<num-threads> <num-cuda-gpus> <num-opencl-accels>'
fi
if [ x$2 == x ]; then
   echo Us: $0 '<num-threads> <num-cuda-gpus> <num-opencl-accels>'
fi
if [ x$3 == x ]; then
   echo Us: $0 '<num-threads> <num-cuda-gpus> <num-opencl-accels>'
fi
NX_ARGS="--smp-workers $1 --gpus $2 --opencl-max-devices $3 --opencl-device-type ACCELERATOR --summary" ./matmul-p.$4
