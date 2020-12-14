PROGRAM=matmul
KERNEL=kernel
PREFIX=.

TARGETS=$(PROGRAM)-p $(PROGRAM)-i $(PROGRAM)-d

JOB_SCHED_VERSION=-smp

BASE_DIR=../..
include $(BASE_DIR)/common-files/Makefile
KC = nvcc
CC = mcc

CFLAGS = --ompss --opencl --cuda -k
CFLAGS_P =
CFLAGS_I = --instrument
CFLAGS_D = --debug

LIBS  = -L$(MKL_LIB_DIR) -lmkl_sequential -lmkl_avx2 -lmkl_rt -lmkl_core
INCS  = -I$(CUDA_HOME)/include -I$(PREFIX) --opencl-build-opts=-I$(PREFIX)
KINCS  = -I$(CUDA_HOME)/include -I$(PREFIX) 

EXTRA = -O3 -Wall -Wno-unused 
OBJECTS = cclock.o driver.o prtspeed.o check.o gendat.o

$(PROGRAM)-p: $(PROGRAM).c $(KERNEL).aocx $(OBJECTS) kern.o
	$(CC) $(GFLAGS) $(CFLAGS) $(CFLAGS_P) $(EXTRA) $(INCS) -o $@ $< $(LIBS) $(KERNEL).aocx $(OBJECTS) kern.o

$(PROGRAM)-i: $(PROGRAM).c $(KERNEL).aocx $(OBJECTS) kern.o
	$(CC) $(GFLAGS) $(CFLAGS) $(CFLAGS_I) $(EXTRA) $(INCS) -o $@ $< $(LIBS) $(KERNEL).aocx $(OBJECTS) kern.o

$(PROGRAM)-d: $(PROGRAM).c $(KERNEL).aocx $(OBJECTS) kern.o
	$(CC) $(GFLAGS) $(CFLAGS) $(CFLAGS_D) $(EXTRA) $(INCS) -o $@ $< $(LIBS) $(KERNEL).aocx $(OBJECTS) kern.o

kern.o: kern.cu $(PREFIX)/$(KERNEL).h
	$(KC) $(KINCS) -DMYCUDA -o $@ -c $<


.c.o:
	$(CC) --no-openmp $(EXTRA) -c $<

clean:
	rm -f $(CC)_* *.o *~ cudacc_matmul.cu $(TARGETS) 

