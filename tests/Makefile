BASE := ~/arc/dev/math

INCLUDE_DIR := ${BASE}/include/
TESTS_DIR := ${BASE}/tests
BIN_DIR := ${TESTS_DIR}/bin
FLAGS := -std=c++23

%: %.cpp
	${CXX} $< ${FLAGS} -I ${INCLUDE_DIR} -I ${TESTS_DIR} -o ${BIN_DIR}/$@
