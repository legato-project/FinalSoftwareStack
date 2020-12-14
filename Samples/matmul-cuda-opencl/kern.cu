#include <kernel.h>

// Thread block size
#define THREADS_BS 16

// Device multiplication function called by Mul() 
// Compute C = A * B 
//	wA is the width of A 
//	wB is the width of B
__global__ void matrixMult_cuda(REAL* C, REAL* A,  REAL * B, int wA, int wB)
{
   // Block and thread index 
   int bx = blockIdx.x;
   int by = blockIdx.y;
   int tx = threadIdx.x;
   int ty = threadIdx.y;

   // Index of the first/last sub-matrix of A processed by the block 
   int aBegin = wA * THREADS_BS * by;
   int aEnd   = aBegin + wA - 1;

   // Step size used to iterate through the sub-matrices of A 
   int aStep = THREADS_BS;
   // Index of the first sub-matrix of B processed by the block 
   int bBegin = THREADS_BS * bx;
   // Step size used to iterate through the sub-matrices of B 
   int bStep = THREADS_BS * wB;

   // The element of the block sub-matrix that is computed  by the thread 
   REAL Csub = 0;

   // Loop over all sub-matrices (A & B) required to compute the block sub-matrix 
   for (int a = aBegin, b = bBegin; a <= aEnd; a += aStep, b += bStep) {

      // Shared memory for sub-matrix of A & B
      __shared__ REAL As[THREADS_BS][THREADS_BS];
      __shared__ REAL Bs[THREADS_BS][THREADS_BS];

      // Load the matrices from global memory to shared memory;  
      // each thread loads one element of each matrix 
      As[ty][tx] = A[a + wA * ty + tx];  
      Bs[ty][tx] = B[b + wB * ty + tx];

      // Synchronize to make sure the matrices are loaded 
      __syncthreads();

      // Multiply the two matrices together; each thread computes one element 
      // of the block sub-matrix  
      for (int k = 0; k < THREADS_BS; ++k)
         Csub += As[ty][k] * Bs[k][tx];

      // Synchronize to make sure that the preceding computation is done before
      // loading two new sub-matrices of A and B in the next iteration
      __syncthreads();
   }

   // Write the block sub-matrix to global memory; each thread writes one element
   int c = wB * THREADS_BS * by + THREADS_BS * bx;
   C[c + wB*ty + tx] += Csub;
}
