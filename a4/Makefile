CXX = g++

CXXFLAGS := ${TCPFLAG} -g -Wall -MMD

EXECS = 

# Object files
COMMON_OBJECTS = $(addsuffix .o, ucp_c rcs rcs_socket message msg_buffer)
BUILD_OBJECTS = ${COMMON_OBJECTS}
UNIT_TESTS:=harness.o $(shell find tests -iname '*.cc' | sed 's/\.cc$$/.o/')
TEST_OBJECTS = ${UNIT_TESTS} ${COMMON_OBJECTS}


# Builds

.PHONY : test clean

test : ${TEST_OBJECTS}
	@ ${CXX} ${CXXFLAGS} $^ -o test && ./test ; rm test

clean :
	rm -f *.d *.o **/*.d **/*.o ${EXECS}

