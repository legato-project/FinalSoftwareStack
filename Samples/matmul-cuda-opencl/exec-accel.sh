#!/bin/bash

for bs in 128 256 512 1024; do
for gpu in 0 1; do
for fpga in 0 1; do
for cpu in 1 4 8 10 11 12; do

echo  Executing matmul-p bs $bs cpus $cpu gpus $gpu fpgas $fpga
for i in 1 2 3; do
 ./exec-bs128.sh $cpu $gpu $fpga $bs
done

done
done
done
done
