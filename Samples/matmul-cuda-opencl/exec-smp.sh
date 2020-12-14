#!/bin/bash

for bs in 128 256 512 1024; do
for cpu in 1 2 3 4 5 6 7 8 9 10 11 12; do

echo  Executing matmul-p bs $bs cpus $cpu 
for i in 1 2 3; do
 ./exec-bs128.sh $cpu 0 0 $bs
done

done
done
