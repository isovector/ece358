CXX = g++

CXXFLAGS := ${TCPFLAG} -g -Wall -MMD

EXECS = 

# Object files

TEST_OBJECTS:=$(shell find tests -iname '*.cc' | sed 's/\.cc$$/.o/')

# Builds

.PHONY : tests clean

tests : harness.o ${TEST_OBJECTS}
	@ ${CXX} ${CXXFLAGS} $^ -o testHarness && ./testHarness && rm testHarness

clean :
	rm -f *.d *.o ${EXECS}

