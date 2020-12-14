
// Thread block size
#define BLOCK_SIZE 128

//Mercurium pragmas can't "read" values from #defines, so we "save" the value as integer
const int BL_SIZE= BLOCK_SIZE;

#ifdef DP
#define REAL double
#else
#define REAL float
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef MYCUDA
//Kernel declaration as a task in OpenCL (fpga) should be here
//Remember, we want to multiply two matrices, (A*B=C) where all of them have size NB*NB
#pragma omp target device(opencl) ndrange(2,NB,NB,BL_SIZE,BL_SIZE) copy_deps
#pragma omp task inout([NB*NB]C) in([NB*NB]A,[NB*NB]B)
__kernel void matrixMult(__global REAL* C,__global REAL* A, __global REAL* B,int wA, int wB);
#endif


//Kernel declaration as a task in CUDA (GPU) should be here
//Remember, we want to multiply two matrices, (A*B=C) where all of them have size NB*NB
#pragma omp target device(cuda) ndrange(2,NB,NB,16,16) copy_deps \
     implements(matrixMult)
#pragma omp task inout([NB*NB]C) in([NB*NB]A,[NB*NB]B)
__global__ void matrixMult_cuda(REAL* C, REAL* A,  REAL * B, int wA, int wB);

#ifdef __cplusplus
}
#endif
