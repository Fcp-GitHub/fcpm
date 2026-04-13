BASE := ~/arc/dev/math

INCLUDE_DIR := ${BASE}/include/
GBENCH_BASE := ${BASE}/benchmarks/benchmark
GBENCH_INCLUDE_DIR  := ${GBENCH_BASE}/include
BIN_DIR := ${BASE}/benchmarks/bin
FLAGS := -std=c++23 -L ${GBENCH_BASE}/build/src
LINKER_FLAGS := -lbenchmark -lpthread

%: %.cpp
	${CXX} $< ${FLAGS} -I ${INCLUDE_DIR} -isystem ${GBENCH_INCLUDE_DIR} ${LINKER_FLAGS} -o ${BIN_DIR}/$@
