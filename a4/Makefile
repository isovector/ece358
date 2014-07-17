CC = g++
CXX = g++

CXXFLAGS := ${TCPFLAG} -g -Wall -MMD -pthread

EXECS = rcsapp-server rcsapp-client

# Object files
COMMON_OBJECTS = $(addsuffix .o, rcs rcs_t msg_t msg_buffer_t mybind ucp)
UNIT_TESTS:=harness.o $(shell find tests -iname '*.cc' | sed 's/\.cc$$/.o/')
TEST_OBJECTS = ${UNIT_TESTS} ${COMMON_OBJECTS}

# Builds

.PHONY : test clean cleanall

test : ${TEST_OBJECTS}
	@ pkill test; ${CXX} ${CXXFLAGS} $^ -o test && ./test ; rm test;

rcsapp : rcsapp-server rcsapp-client

rcsapp-server : ${COMMON_OBJECTS} rcsapp-server.o
	${CXX} ${CXXFLAGS} $^ -o rcsapp-server

rcsapp-client : ${COMMON_OBJECTS} rcsapp-client.o
	${CXX} ${CXXFLAGS} $^ -o rcsapp-client

clean :
	rm -f *.a *.d *.o ${EXECS}

cleanall :
	rm -f *.a *.d *.o **/*.d **/*.o ${EXECS}

