CXX = g++

CXXFLAGS := ${TCPFLAG} -g -Wall -MMD

EXECS =

# Object files
COMMON_OBJECTS = $(addsuffix .o, ucp_c rcs rcs_socket message msg_buffer)
BUILD_OBJECTS = ${COMMON_OBJECTS}
UNIT_TESTS:=harness.o $(shell find tests -iname '*.cc' | sed 's/\.cc$$/.o/')
TEST_OBJECTS = ${UNIT_TESTS} ${COMMON_OBJECTS}
TEST_CLIENT_OBJECTS = ${COMMON_OBJECTS} tests/client_main.o
TEST_SERVER_OBJECTS = ${COMMON_OBJECTS} tests/server_main.o


# Builds

.PHONY : test clean

test : ${TEST_OBJECTS}
	@ pkill test; ${CXX} ${CXXFLAGS} $^ -o test && ./test ; rm test;

clientserver : server client

server : ${TEST_SERVER_OBJECTS}
	@ ${CXX} ${CXXFLAGS} $^ -o server

client : ${TEST_CLIENT_OBJECTS}
	@ ${CXX} ${CXXFLAGS} $^ -o client

clean :
	rm -f *.d *.o **/*.d **/*.o ${EXECS}

