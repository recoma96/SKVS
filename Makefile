CC=gcc
CXX=g++ -g
CXXFLAGS= -std=c++17 -lprotobuf

TEST_TARGET=test

#make buffer protocol file
SERIAL=$(shell protoc -I=lib/packet --cpp_out=lib/packet lib/packet/PacketSerial.proto)

STRUC_SRC=$(wildcard lib/structure/*.cpp)
PACKET_SRC=$(wildcard lib/packet/*.cpp)

TEST_SRC=test.cpp

SRCS=$(STRUC_SRC)
OBJS=$(SRCS:.cpp=.o)

TEST_OBJ=$(TEST_SRC:.cpp=.o)


test: $(SERIAL) lib/packet/PacketSerial.pb.o $(OBJS) $(TEST_OBJ)
	g++ -o $(TEST_TARGET) $(OBJS) $(TEST_OBJ)

clean_test:
	rm lib/packet/PacketSerial.pb.*
	rm $(OBJS)
	rm $(TEST_OBJ)
	rm $(TEST_TARGET)

#make protocol object file
lib/packet/PacketSerial.pb.o: lib/packet/PacketSerial.pb.cc
	g++ -g -c -std=c++17 -o lib/packet/PacketSerial.pb.o lib/packet/PacketSerial.pb.cc -lprotobuf