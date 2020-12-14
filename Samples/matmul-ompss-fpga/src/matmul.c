/*
* Copyright (c) 2020, BSC (Barcelona Supercomputing Center)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the <organization> nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY BSC ''AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>

// General definitions
#include "matmul.h"

void child_func()
{
   _exit(0);
}

#pragma omp task
void setBlock(elem_t* v, const elem_t val) {
   for (unsigned int i = 0; i < BSIZE*BSIZE; ++i) {
      v[i] = val;
   }
}

#pragma omp task
void setBlockSeq(elem_t* v, int base) {
   for (unsigned int i = 0; i < BSIZE*BSIZE; ++i) {
      v[i] = ((elem_t)((base/1024)%2)) - 1.0 + ((elem_t)(base%512))/1000;
      base = (base*97 + 89)%65536;
   }
}

#pragma omp task
void checkBlock(unsigned int* check_ok, const elem_t* res, const elem_t* ref, const float threshold)
{
   for (unsigned int i = 0; i < BSIZE*BSIZE && ( *check_ok ); ++i) {
      const elem_t res_val = res[i];
      const elem_t ref_val = ref[i];
      const elem_t maxv = ref_val * (1.0 + (ref_val < 0 ? -threshold : threshold));
      const elem_t minv = ref_val * (1.0 - (ref_val < 0 ? -threshold : threshold));
      if (res_val > maxv || res_val < minv) {
         *check_ok = 0;
         fprintf(stderr, "ERROR:\t Expected a %lf but found %lf. %d\n", (double)ref_val, (double)res_val, i);
      }
   }
}

unsigned int matmulCheck(const unsigned int check, const elem_t* c, const unsigned int msize)
{
   const unsigned int m2size = msize*msize;
   const unsigned int b2size = BSIZE*BSIZE;
   unsigned int check_ok = 1;

   if (check == 1) {
      //Check the result matrix against the reference solution
      printf( "=================== CHECKING ===================== \n" );
      char ref_filename[64];
      sprintf(ref_filename, "ref/matmul_%s_%u_%u_%u.ref", ELEM_T_STR, msize, BSIZE, 2 /*numReps*/);
      int ref_file = open(ref_filename, O_RDONLY);
      if (ref_file == -1) {
         fprintf(stderr, "Cannot open '%s' as a reference solution\n", ref_filename);
         check_ok = 0;
      } else {
         const elem_t* c_ref = (const elem_t *)mmap(NULL, m2size*sizeof(elem_t), PROT_READ, MAP_SHARED, ref_file, 0);
         if (c_ref == (const elem_t *)-1) {
            fprintf(stderr, "Cannot map '%s' as a reference solution\n", ref_filename);
            check_ok = 0;
         } else {
            for (unsigned int i = 0; i < msize/BSIZE && check_ok; i++) {
               for (unsigned int j = 0; j < msize/BSIZE && check_ok; j++) {
                   //printf("Checking block %d, %d\n", i, j);
                  unsigned int const ci = j*b2size + i*BSIZE*msize;
                  checkBlock(&check_ok, &c[ci], &c_ref[ci], THRESHOLD);
               }
            }
            #pragma omp taskwait
            munmap((void *)c_ref, m2size*sizeof(elem_t));
            close(ref_file);
         }
      }
      printf( "Output matrix is %s!\n", (check_ok ? "OK" : "WRONG") );
      printf( "================================================== \n" );
   } else if (check == 2) {
     //Write the reference file
      printf( "============= GENERATING REFERENCE =============== \n" );
      char ref_filename[64];
      sprintf(ref_filename, "matmul_%s_%u_%u_%u.ref", ELEM_T_STR, msize, BSIZE, 2 /*numReps*/);
      FILE *ref_file = fopen(ref_filename, "w+");
      if (fwrite(c, sizeof(elem_t), m2size, ref_file) != m2size) {
         fprintf(stderr, "Error writing reference file\n");
         check_ok = 0;
      }
      fclose(ref_file);
      printf( "Output wrote to '%s'\n", ref_filename );
      printf( "Move the file inside the 'ref' folder to use it as a reference\n" );
      printf( "================================================== \n" );
   }
   return check_ok;
}

#pragma omp target device(fpga) num_instances(3)
#pragma omp task in([BSIZE*BSIZE]a, [BSIZE*BSIZE]b) inout([BSIZE*BSIZE]c)
void matmulBlock(const elem_t *a, const elem_t *b, elem_t *c)
{
   #pragma HLS INLINE // off
   #pragma HLS array_partition variable=a cyclic factor=4
   #pragma HLS array_partition variable=b cyclic factor=BSIZE/4
   #pragma HLS array_partition variable=c cyclic factor=BSIZE/2

   for (int k = 0; k < BSIZE; ++k) {
      for (int i = 0; i < BSIZE; ++i) {
         #pragma HLS pipeline II=MBLOCK_II
         for (int j = 0; j < BSIZE; ++j) {
            c[i*BSIZE + j] += a[i*BSIZE + k] * b[k*BSIZE + j];
         }
      }
   }
}

#if defined(USE_IMPLEMENTS)
//#pragma omp target device(smp) copy_deps implements(matmulBlock)
#pragma omp target device(smp) no_copy_deps implements(matmulBlock) copy_inout([BSIZE*BSIZE]c)
#pragma omp task in([BSIZE*BSIZE]a, [BSIZE*BSIZE]b) inout([BSIZE*BSIZE]c)
void matmulBlockSmp(elem_t *a, elem_t *b, elem_t *c) {
#if defined(USE_MKL)
   elem_t const alpha = 1.0;
   elem_t const beta = 1.0;
   char const transa = 'n';
   char const transb = 'n';
   GEMM(&transa, &transb, &BSIZE, &BSIZE, &BSIZE, &alpha, a,
         &BSIZE, b, &BSIZE, &beta, c, &BSIZE);
#elif defined(USE_OPENBLAS)
   elem_t const alpha = 1.0;
   elem_t const beta = 1.0;
   cblas_gemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, BSIZE, BSIZE,
      BSIZE, alpha, a, BSIZE, b, BSIZE, beta, c, BSIZE);
#else
   for (unsigned int i = 0; i < BSIZE; ++i) {
      for (unsigned int j = 0; j < BSIZE; ++j) {
         elem_t l = 0;
         for (unsigned int k = 0; k < BSIZE; ++k) {
            l += a[i*BSIZE + k] * b[k*BSIZE + j];
         }
         c[i*BSIZE + j] += l;
      }
   }
#endif
}
#endif // defined(USE_IMPLEMENTS)

void matmul(const elem_t *a, const elem_t *b, elem_t *c, const unsigned int msize) {
   const unsigned int b2size = BSIZE*BSIZE;
   for (unsigned int i = 0; i < msize/BSIZE; i++) {
      for (unsigned int k = 0; k < msize/BSIZE; k++) {
         unsigned int const ai = k*b2size + i*BSIZE*msize;
         for (unsigned int j = 0; j < msize/BSIZE; j++) {
            unsigned int const bi = j*b2size + k*BSIZE*msize;
            unsigned int const ci = j*b2size + i*BSIZE*msize;
            matmulBlock(a + ai, b + bi, c + ci);
         }
      }
   }
}

int main(int argc, char** argv) {
   if (argc < 2 || argc > 3) {
      usage(argv[0]);
      exit(1);
   }

   unsigned int const b2size = BSIZE*BSIZE;
   unsigned int const msize = atoi(argv[1]);
   unsigned int const m2size = msize*msize;
   unsigned char const check = argc > 2 ? atoi(argv[2]) : 1;
   if (msize%BSIZE != 0) {
      fprintf(stderr, "ERROR:\t<matrix size> must be multiple of <block size>\n");
      usage(argv[0]);
      exit(1);
   }

   unsigned int s = m2size*sizeof(elem_t);
   elem_t* a = (elem_t *)(malloc(s));
   elem_t* b = (elem_t *)(malloc(s));
   elem_t* c = (elem_t *)(malloc(s));
   if (a == NULL || b == NULL || c == NULL) {
      fprintf(stderr, "ERROR:\tCannot allocate memory for the matrices\n");
      exit(1);
   }


   int pid = fork();

   if (pid==0) child_func();


   double tIniStart = wall_time();

   srand(2019);
   for (unsigned int i = 0; i < m2size/b2size; i++) {
      setBlockSeq(&a[i*b2size], rand());
      setBlockSeq(&b[i*b2size], rand());
      setBlock(&c[i*b2size], 0);
   }

   #pragma omp taskwait
   const double tEndStart = wall_time();
   const double tIniWarm = tEndStart;

   //Warm up execution
   matmul(a, b, c, msize);

   #pragma omp taskwait noflush
   const double tEndWarm = wall_time();
   const double tIniExec = tEndWarm;

   //Performance execution
   matmul(a, b, c, msize);

   #pragma omp taskwait noflush
   const double tEndExec = wall_time();
   const double tIniFlush = tEndExec;

   //The following TW will copy out the data moved to FPGA devices
   #pragma omp taskwait
   const double tEndFlush = wall_time();
   const double tIniCheck = tEndFlush;

   //Check the output matrix
   unsigned int check_ok = matmulCheck(check, c, msize);

   const double tEndCheck = wall_time();

   free(a);
   free(b);
   free(c);

   //Print the execution report
   const float gflops = m2size/1000.0*msize/1000.0*2.0/1000.0/(tEndExec - tIniExec);
   printf( "==================== RESULTS ===================== \n" );
   printf( "  Benchmark: %s (%s)\n", "Matmul", "OmpSs" );
   printf( "  Elements type: %s\n", ELEM_T_STR );
   printf( "  Init. time (secs):     %f\n", tEndStart  - tIniStart );
   printf( "  Warm up time (secs):   %f\n", tEndWarm   - tIniWarm );
   printf( "  Execution time (secs): %f\n", tEndExec    - tIniExec );
   printf( "  Flush time (secs):     %f\n", tEndFlush  - tIniFlush );
   printf( "  Checking time (secs):  %f\n", tEndCheck  - tIniCheck );
   printf( "  Performance (GFLOPS):  %f\n", gflops );
   printf( "================================================== \n" );

   return check_ok ? 0 : 1;
}
