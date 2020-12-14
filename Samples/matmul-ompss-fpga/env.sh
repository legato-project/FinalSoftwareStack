module load gcc-arm/6.2.0_aarch64
module load openblas/0.3.6_arm64
module load vivado/2017.3
module load ompss/arm64_fpga/2.2.0
export BOARD=zcu102
export FPGA_CLOCK=250
export MATMUL_BLOCK_SIZE=256
export MATMUL_NUM_ACCS=3
export MATMUL_BLOCK_II=1
echo do make bitstream-p  to generate the bitstream for the FPGA
echo do make matmul-p     to generate the matmul binary for the host

