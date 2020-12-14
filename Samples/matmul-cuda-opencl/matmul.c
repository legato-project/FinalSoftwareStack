#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <kernel.h>

#include "driver.h"

#ifdef DP
#define REAL double
#else
#define REAL float
#endif

 extern void cblas_sgemm(char, char, char, int, int, int, REAL, REAL *, int, REAL *, int, REAL, REAL *, int);

#pragma omp target device(smp) copy_deps implements(matrixMult)
#pragma omp task in([bsize*bsize]A, [bsize*bsize]B) inout([bsize*bsize]C)
void matrixMult_mkl(REAL  *C, REAL *A, REAL * B, int wa, int bsize)
{
#if 1
typedef enum {CblasRowMajor=101, CblasColMajor=102} CBLAS_LAYOUT;
enum   CBLAS_TRANSPOSE { CblasNoTrans = 111, CblasTrans = 112, CblasConjTrans = 113} ;
  //ntasks++;
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, bsize, bsize, bsize,
                1.0f, A, bsize, B, bsize, 1.0f, C, bsize);
#else
   int i, j, k, I;
   float tmp;
   for (i = 0; i < bsize; i++)
   {
     I=i*bsize;
     for (j = 0; j < bsize; j++)
     {
       tmp=C[I+j];
       for (k = 0; k < bsize; k++)
       {
          tmp+=A[I+k]*B[k*bsize+j];
       }
       C[I+j]=tmp;
     }
   }
#endif
}
//


const int NB = BSIZE;


void matmul( int m, int l, int n, int mDIM, int lDIM, int nDIM, REAL **tileA, REAL **tileB,
             REAL **tileC )
{	
	int i, j, k;
	for(i = 0;i < mDIM; i++){
		for (j = 0; j < nDIM; j++){ 
			for (k = 0; k < lDIM; k++){
				//Kernel call
				//Muld(tileA[i*lDIM+k], tileB[k*nDIM+j],NB,NB, tileC[i*nDIM+j],NB);
				printf ("NB %d nDIM %d\n", NB, nDIM);
				matrixMult(tileC[i*nDIM+j], tileA[i*lDIM+k], tileB[k*nDIM+j],NB,NB);
			}
		}
	}
   #pragma omp taskwait
}


