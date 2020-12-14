PROGRAM_     = matmul

CFLAGS_      = $(CFLAGS) -O3 -std=gnu99
MCC_FLAGS_   = --ompss --variable=disable_final_clause_transformation:1
MCC_FLAGS_I_ = --instrument
MCC_FLAGS_D_ = --debug -g -k
LDFLAGS_     = $(LDFLAGS)

MCC         ?= fpgacc
MCC_         = $(CROSS_COMPILE)$(MCC)
GCC_         = $(CROSS_COMPILE)gcc

# FPGA bitstream Variables
FPGA_CLOCK             ?= 300
FPGA_MEMORY_PORT_WIDTH ?= 512
MATMUL_BLOCK_SIZE      ?= 256
MATMUL_BLOCK_II        ?= 2
MATMUL_NUM_ACCS        ?= 1

## MKL Variables
MKL_DIR      ?= $(MKLROOT)
MKL_INC_DIR  ?= $(MKL_DIR)/include
MKL_LIB_DIR  ?= $(MKL_DIR)/lib
MKL_SUPPORT_ = $(if $(and $(wildcard $(MKL_INC_DIR)/mkl.h ), \
               $(wildcard $(MKL_LIB_DIR)/libmkl_sequential.so )),YES,NO)

## Open Blas Variables
OPENBLAS_DIR      ?= $(OPENBLAS_HOME)
OPENBLAS_INC_DIR  ?= $(OPENBLAS_DIR)/include
OPENBLAS_LIB_DIR  ?= $(OPENBLAS_DIR)/lib
OPENBLAS_SUPPORT_ = $(if $(and $(wildcard $(OPENBLAS_INC_DIR)/lapacke.h ), \
                    $(wildcard $(OPENBLAS_LIB_DIR)/libopenblas.so )),YES,NO)

ifeq ($(MKL_SUPPORT_),YES)
	CFLAGS_  += -I$(MKL_INC_DIR) -DUSE_MKL
	LDFLAGS_ += -L$(MKL_LIB_DIR) -lmkl_sequential -lmkl_core -lmkl_intel_lp64
#	LDFLAGS_ += -L$(MKL_LIB_DIR) -lmkl_sequential -lmkl_core -lmkl_rt
else ifeq ($(OPENBLAS_SUPPORT_),YES)
	CFLAGS_  += -I$(OPENBLAS_INC_DIR) -DUSE_OPENBLAS
	LDFLAGS_ += -L$(OPENBLAS_LIB_DIR) -lopenblas
endif

##CFLAGS += -DUSE_DOUBLE
##CFLAGS += -DUSE_IMPLEMENTS
CFLAGS_ += -DMATMUL_BLOCK_SIZE=$(MATMUL_BLOCK_SIZE) -DMATMUL_BLOCK_II=$(MATMUL_BLOCK_II) -DMATMUL_NUM_ACCS=$(MATMUL_NUM_ACCS)
#FPGA_LINKER_FLAGS =--Wf,"--name=$(PROGRAM_),--board=$(BOARD),-c=$(FPGA_CLOCK),--hwruntime=som,--interconnection_opt=performance,--to_step=design"
FPGA_LINKER_FLAGS =--Wf,"--name=$(PROGRAM_),--board=$(BOARD),-c=$(FPGA_CLOCK),--hwruntime=som,--interconnection_opt=performance,--to_step=bitstream"

all: help
help:
	@echo 'Supported targets:       $(PROGRAM_)-p $(PROGRAM_)-i $(PROGRAM_)-d $(PROGRAM_)-seq bitsteam-i bitstream-p clean info help'
	@echo 'Environment variables:   CFLAGS, LDFLAGS, CROSS_COMPILE, MCC'
	@echo 'MKL env. variables:      MKLROOT, MKL_DIR, MKL_INC_DIR, MKL_LIB_DIR'
	@echo 'OpenBLAS env. variables: OPENBLAS_HOME, OPENBLAS_DIR, OPENBLAS_INC_DIR, OPENBLAS_LIB_DIR'
	@echo 'FPGA env. variables:     BOARD, FPGA_CLOCK, FPGA_MEMORY_PORT_WIDTH, MATMUL_BLOCK_SIZE, MATMUL_BLOCK_II, MATMUL_NUM_ACCS'

$(PROGRAM_)-p: ./src/$(PROGRAM_).c
	$(MCC_) $(CFLAGS_) $(MCC_FLAGS_) $^ -o $@ $(LDFLAGS_)

$(PROGRAM_)-i:  ./src/$(PROGRAM_).c
	$(MCC_) $(CFLAGS_) $(MCC_FLAGS_) $(MCC_FLAGS_I_) $^ -o $@ $(LDFLAGS_)

$(PROGRAM_)-d:  ./src/$(PROGRAM_).c
	$(MCC_) $(CFLAGS_) $(MCC_FLAGS_) $(MCC_FLAGS_D_) $^ -o $@ $(LDFLAGS_)

$(PROGRAM_)-seq: ./src/$(PROGRAM_).c
	$(GCC_) $(CFLAGS_) $^ -o $@ $(LDFLAGS_)

bitstream-i: ./src/$(PROGRAM_).c
	sed -i 's/num_instances(.)/num_instances($(MATMUL_NUM_ACCS))/1' $^
	$(MCC_) $(CFLAGS_) $(MCC_FLAGS_) $(MCC_FLAGS_I_) $^ -o $(PROGRAM_)-i $(LDFLAGS_) \
	--bitstream-generation $(FPGA_LINKER_FLAGS) \
	--variable=fpga_memory_port_width:$(FPGA_MEMORY_PORT_WIDTH)

bitstream-p: ./src/$(PROGRAM_).c
	sed -i 's/num_instances(.)/num_instances($(MATMUL_NUM_ACCS))/1' $^
	$(MCC_) $(CFLAGS_) $(MCC_FLAGS_) $^ -o $(PROGRAM_)-p $(LDFLAGS_) \
	--bitstream-generation $(FPGA_LINKER_FLAGS) \
	--variable=fpga_memory_port_width:$(FPGA_MEMORY_PORT_WIDTH)

info:
	@echo "========== OPENBLAS =========="
	@echo "  SUPPORT enabled:  $(OPENBLAS_SUPPORT_)"
	@echo "  OPENBLAS_DIR:     $(OPENBLAS_DIR)"
	@echo "  OPENBLAS_INC_DIR: $(OPENBLAS_INC_DIR)"
	@echo "  OPENBLAS_LIB_DIR: $(OPENBLAS_LIB_DIR)"
	@echo "  Headers:          $(if $(wildcard $(OPENBLAS_INC_DIR)/lapacke.h ),YES,NO)"
	@echo "  Lib files (.so):  $(if $(wildcard $(OPENBLAS_LIB_DIR)/libopenblas.so ),YES,NO)"
	@echo "=============================="
	@echo "============= MKL ============"
	@echo "  SUPPORT enabled:  $(MKL_SUPPORT_)"
	@echo "  MKL_DIR:          $(MKL_DIR)"
	@echo "  MKL_INC_DIR:      $(MKL_INC_DIR)"
	@echo "  MKL_LIB_DIR:      $(MKL_LIB_DIR)"
	@echo "  Headers:          $(if $(wildcard $(MKL_INC_DIR)/mkl.h ),YES,NO)"
	@echo "  Lib files (.so):  $(if $(wildcard $(MKL_LIB_DIR)/libmkl_sequential.so ),YES,NO)"
	@echo "=============================="

clean:
	rm -fv *.o $(PROGRAM_)-? $(MCC_)_$(PROGRAM_).c *:*_hls_automatic_mcxx.cpp
