aoc -o bin/matrix_mult.aocx -fp-relaxed -fpc -no-interleaving=default \
  -DBLOCK_SIZE=128 -DSIMD_WORK_ITEMS=4 \
  -board=pac_s10_dc  device/matrix_mult.cl
