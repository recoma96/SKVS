CC=gcc
CXX=g++ -g
CXXFLAGS= -std=c++17 -pthread
SERVER_TARGET=server/skvs-server
CLIENT_TARGET=client/skvs-client


STRUC_SRC=$(wildcard lib/structure/*.cpp)
PACKET_SRC=$(wildcard lib/packet/*.cpp)
USER_SRC=$(wildcard lib/user/*.cpp)
LOADER_SRC=$(wildcard lib/loader/*.cpp)
DATABASE_SRC=$(wildcard lib/database/*.cpp)
LIB_SRC=lib/CommandFilter.cpp
TADAPTER_SRC=$(wildcard lib/threadAdapter/*.cpp)
LOGBASE_SRC=$(wildcard lib/logbase/*.cpp)
SOCKET_SRC=$(wildcard lib/SockWrapper/*.cpp)

SERVER_SRC=$(wildcard ServerCode/*.cpp)
CLIENT_SRC=$(wildcard ClientCode/*.cpp)

SRCS=$(STRUC_SRC) $(PACKET_SRC) $(USER_SRC) $(LOADER_SRC) $(DATABASE_SRC) $(LIB_SRC) $(TADAPTER_SRC) $(LOGBASE_SRC) $(SOCKET_SRC)

OBJS=$(SRCS:.cpp=.o)
SERVER_OBJ=$(SERVER_SRC:.cpp=.o)
CLIENT_OBJ=$(CLIENT_SRC:.cpp=.o)

TEST_OBJ=$(TEST_SRC:.cpp=.o)

all: server client

server: $(SERIAL) $(SERVER_OBJ) $(OBJS)
	g++ -o $(SERVER_TARGET) $(SERVER_OBJ) $(OBJS) $(CXXFLAGS)

client: $(SERIAL) $(CLIENT_OBJ) $(OBJS)
	g++ -o $(CLIENT_TARGET) $(CLIENT_OBJ) $(OBJS) $(CXXFLAGS)


clean_server:
	rm $(OBJS)
	rm $(SERVER_OBJ)
	rm $(SERVER_TARGET)
	
clean_client:
	rm $(OBJS)
	rm $(CLIENT_OBJ)
	rm $(CLIENT_TARGET)
	
clean_all:
	rm $(OBJS)
	rm $(CLIENT_OBJ)
	rm $(SERVER_OBJ)
	rm $(CLIENT_TARGET)
	rm $(SERVER_TARGET)