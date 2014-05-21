CXX = g++

ifneq "$(findstring tcp, $(MAKECMDGOALS))" ""
TCPFLAG = -DTCP
endif

CXXFLAGS := ${TCPFLAG} -g -Wall -MMD

EXECS = client server

# Object files
CLIENT_COMMON_OBJ = Client ClientMain
TCP_CLIENT_OBJECTS = ${addsuffix .o, TCPClient ${CLIENT_COMMON_OBJ}}
UDP_CLIENT_OBJECTS = ${addsuffix .o, UDPClient ${CLIENT_COMMON_OBJ}}

SERVER_COMMON_OBJ = Server ServerMain
TCP_SERVER_OBJECTS = ${addsuffix .o, TCPServer ${SERVER_COMMON_OBJ}}
UDP_SERVER_OBJECTS = ${addsuffix .o, UDPServer ${SERVER_COMMON_OBJ}}

tcp : TCPClient TCPServer

udp : UDPClient UDPServer

clean :
	rm -f *.d *.o ${EXECS}

# TCP commands
TCPClient : ${TCP_CLIENT_OBJECTS}
	${CXX} ${CXXFLAGS} $^ -o client

TCPServer : ${TCP_SERVER_OBJECTS}
	${CXX} ${CXXFLAGS} $^ -o server

UDPClient : ${UDP_CLIENT_OBJECTS}
	${CXX} ${CXXFLAGS} $^ -o client

UDPServer : ${UDP_SERVER_OBJECTS}
	${CXX} ${CXXFLAGS} $^ -o server
