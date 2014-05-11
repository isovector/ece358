CXX = g++

ifneq "$(findstring tcp, $(MAKECMDGOALS))" ""
TCPFLAG = -DTCP
endif

CXXFLAGS := ${TCPFLAG} -g -Wall -MMD -std=c++11 

# Object files
CLIENT_COMMON_OBJ = Client ClientMain
TCP_CLIENT_OBJECTS = ${addsuffix .o, TCPClient ${CLIENT_COMMON_OBJ}}
UDP_CLIENT_OBJECTS = ${addsuffix .o, UDPClient ${CLIENT_COMMON_OBJ}}

SERVER_COMMON_OBJ = Server ServerMain
TCP_SERVER_OBJECTS = ${addsuffix .o, TCPServer ${SERVER_COMMON_OBJ}}
UDP_SERVER_OBJECTS = ${addsuffix .o, TCPClient ${SERVER_COMMON_OBJ}}

# Dependencies
TCP_CLIENT_DEPENDENCIES = ${TCP_CLIENT_OBJECTS:.o=.d}
UDP_CLIENT_DEPENDENCIES = ${UDP_CLIENT_OBJECTS:.o=.d}

TCP_SERVER_DEPENDENCIES = ${TCP_SERVER_OBJECTS:.o=.d}
UDP_SERVER_DEPENDENCIES = ${UDP_SERVER_OBJECTS:.o=.d}

-include ${TCP_CLIENT_DEPENDENCIES}
-include ${UDP_CLIENT_DEPENDENCIES}
-include ${TCP_SERVER_DEPENDENCIES}
-include ${UDP_SERVER_DEPENDENCIES}

tcp : TCPClient TCPServer

udp : UDPClient UDPServer

clean :
	rm -f *.d *.o ${EXECS}

# TCP commands
TCPClient : ${TCP_CLIENT_OBJECTS}
	${CXX} ${CXXFLAGS} $^ ${TCP_FLAG} -o client

TCPServer : ${TCP_SERVER_OBJECTS}
	${CXX} ${CXXFLAGS} $^ ${TCP_FLAG} -o server

UDPClient : ${UDP_CLIENT_OBJECTS}
	${CXX} ${CXXFLAGS} $^ -o client

UDPServer : ${UDP_SERVER_OBJECTS}
	${CXX} ${CXXFLAGS} $^ -o server
