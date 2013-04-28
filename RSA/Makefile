# 2. projekt do KRY
# Implementace a prolomeni RSA 
#
# autor: Jan Wozniak

PROJ   = kry
SRC    = src/
MDIR   = src/msieve/
FLAGS  = -std=c99 -Wall -pedantic -w -Isrc/msieve/include
CC     = gcc
LIBS   = -lgmp -lm -lpthread 
MSIEVE = src/msieve/libmsieve.a
M_FLAGS = -O3 -fomit-frame-pointer -march=core2 -D_FILE_OFFSET_BITS=64 -DNDEBUG -D_LARGEFILE64_SOURCE -Wall -W -DMSIEVE_SVN_VERSION="\"Unversioned directory\"" -I. -I$(MDIR)include -I$(MDIR)gnfs -I$(MDIR)gnfs/poly -I$(MDIR)gnfs/poly/stage1 -DNO_ZLIB

ifeq ($(ECM),1)
CFLAGS += -DHAVE_GMP_ECM
	LIBS += -lecm
endif
ifeq ($(WIN),1)
	LDFLAGS += -Wl,--large-address-aware
else
	LIBS += -ldl
endif
ifeq ($(LARGEBLOCKS),1)
	CFLAGS += -DLARGEBLOCKS
endif
ifeq ($(CUDA),1)

ifeq ($(WIN),1)
	CUDA_ROOT = $(shell echo $$CUDA_PATH)
	NVCC = "$(CUDA_ROOT)bin/nvcc"
	CUDA_LIBS = "$(CUDA_ROOT)lib/win32/cuda.lib"
else
	NVCC = "$(shell which nvcc)"
	CUDA_ROOT = $(shell dirname $(NVCC))/../
	CUDA_LIBS = -lcuda
endif
	CFLAGS += -I"$(CUDA_ROOT)include" -Ib40c -DHAVE_CUDA
	LIBS += $(CUDA_LIBS)
endif
ifeq ($(MPI),1)
	CC = mpicc
	CFLAGS += -DHAVE_MPI
endif
ifeq ($(BOINC),1)
	# fill in as appropriate
	BOINC_INC_DIR = .
	BOINC_LIB_DIR = .
	CFLAGS += -I$(BOINC_INC_DIR) -DHAVE_BOINC
	LIBS += -L$(BOINC_LIB_DIR) -lboinc_api -lboinc
endif
ifeq ($(NO_ZLIB),1)
	CFLAGS += -DNO_ZLIB
else
	LIBS += -lz
endif
								


$(PROJ): $(SRC)main.c 
	cd src/msieve && make all && cd -
	$(CC) $(FLAGS) $(M_FLAGS) $(SRC)main.c -o $(PROJ) $(LIBS) $(MSIEVE) 

run: 
	./kry -b 0x3 0x2f72f3e766a6ee4497ec836160e05363be3c0eb5adbdddb47 0x850e13a7c8826f9a8682f611dffe8cadd75e57a351ab9864 

pack:
	tar -zcvf xwozni00.tar.gz src/* Makefile doc/dokumentace.pdf

clean:
	cd src/msieve && make clean && cd -
	rm -rf kry
	rm -rf msieve.dat
	rm -rf *.o
