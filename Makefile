CC=gcc
CXX=g++ -g
CXXFLAGS= -std=c++1y -lprotobuf -pthread -I/usr/local/include

SERVER_TARGET=server/skvs-server
CLIENT_TARGET=client/skvs-client

#make buffer protocol file
SERIAL=$(shell protoc -I=lib/packet --cpp_out=lib/packet lib/packet/PacketSerial.proto)

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

server: $(SERIAL) $(SERVER_OBJ) lib/packet/PacketSerial.pb.o $(OBJS)
	g++ -o $(SERVER_TARGET) $(SERVER_OBJ) $(OBJS) lib/packet/PacketSerial.pb.o $(CXXFLAGS)

client: $(SERIAL) $(CLIENT_OBJ) lib/packet/PacketSerial.pb.o $(OBJS)
	g++ -o $(CLIENT_TARGET) $(CLIENT_OBJ) $(OBJS) lib/packet/PacketSerial.pb.o $(CXXFLAGS)


clean_server:
	rm lib/packet/PacketSerial.pb.*
	rm $(OBJS)
	rm $(SERVER_OBJ)
	rm $(SERVER_TARGET)
	
clean_client:
	rm lib/packet/PacketSerial.pb.*
	rm $(OBJS)
	rm $(CLIENT_OBJ)
	rm $(CLIENT_TARGET)
	
clean_all:
	rm lib/packet/PacketSerial.pb.*
	rm $(OBJS)
	rm $(CLIENT_OBJ)
	rm $(SERVER_OBJ)
	rm $(CLIENT_TARGET)
	rm $(SERVER_TARGET)


#make protocol object file
lib/packet/PacketSerial.pb.o: lib/packet/PacketSerial.pb.cc
	g++ -g -c -std=c++1y -o lib/packet/PacketSerial.pb.o lib/packet/PacketSerial.pb.cc -lprotobuf -pthread -I/usr/local/include
