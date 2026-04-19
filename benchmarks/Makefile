BASE := ~/arc/dev/math

INCLUDE_DIR := ${BASE}/include/
GBENCH_BASE := ${BASE}/benchmarks/benchmark
GBENCH_INCLUDE_DIR  := ${GBENCH_BASE}/include
GLM_BASE := ${BASE}/benchmarks/glm
GLM_INCLUDE_DIR := ${GLM_BASE}
BIN_DIR := ${BASE}/benchmarks/bin
ADD_FLAGS ?= 
FLAGS := -std=c++23 ${ADD_FLAGS} -L ${GBENCH_BASE}/build/src -L ${GLM_BASE}/build/glm
LINKER_FLAGS := -lglm -lbenchmark -lpthread

LINALG_FILES := $(wildcard linalg/*)
FILES := $(basename $(LINALG_FILES))

COMPILER ?= g++
EXECUTABLE_SUFFIX ?= 

all: ${FILES}
.PHONY: all

%: %.cpp
	${COMPILER} $< ${FLAGS} -I ${INCLUDE_DIR} -isystem ${GLM_INCLUDE_DIR} -isystem ${GBENCH_INCLUDE_DIR} ${LINKER_FLAGS} -o ${BIN_DIR}/$@${EXECUTABLE_SUFFIX}
