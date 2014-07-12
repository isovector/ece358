CXX = g++

CXXFLAGS := ${TCPFLAG} -g -Wall -MMD

EXECS =

# Object files
COMMON_OBJECTS = $(addsuffix .o, rcs rcs_t msg_t msg_buffer_t)
BUILD_OBJECTS = ${COMMON_OBJECTS}
UNIT_TESTS:=harness.o ucp_c.o $(shell find tests -iname '*.cc' | sed 's/\.cc$$/.o/')
TEST_OBJECTS = ${UNIT_TESTS} ${COMMON_OBJECTS}
TEST_CLIENT_OBJECTS = ${COMMON_OBJECTS} tests/client_main.o
TEST_SERVER_OBJECTS = ${COMMON_OBJECTS} tests/server_main.o


# Builds

.PHONY : test clean cleanall

librcs.a : ${COMMON_OBJECTS}
	ar crf librcs.a ${COMMON_OBJECTS}

test : ${TEST_OBJECTS}
	@ pkill test; ${CXX} ${CXXFLAGS} $^ -o test && ./test ; rm test;

clientserver : server client

server : ${TEST_SERVER_OBJECTS}
	@ ${CXX} ${CXXFLAGS} $^ -o server

client : ${TEST_CLIENT_OBJECTS}
	@ ${CXX} ${CXXFLAGS} $^ -o client

clean :
	rm -f *.a *.d *.o ${EXECS}

cleanall :
	rm -f *.a *.d *.o **/*.d **/*.o ${EXECS}

