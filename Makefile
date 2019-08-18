CC=gcc
CXX=g++ -g
CXXFLAGS= -std=c++17 -lprotobuf

TEST_TARGET=test

#make buffer protocol file
SERIAL=$(shell sh lib/packet/MakeProtocol.sh)

STRUC_SRC=$(wildcard lib/structure/*.cpp)
PACKET_SRC=$(wildcard lib/packet/*.cpp)

TEST_SRC=test.cpp

SRCS=$(STRUC_SRC)
OBJS=$(SRCS:.cpp=.o)

TEST_OBJ=$(TEST_SRC:.cpp=.o)


test: $(SERIAL) $(OBJS) lib/packet/PacketSerial.pb.o $(TEST_OBJ)
	g++ -o $(TEST_TARGET) $(OBJS) $(TEST_OBJ)

clean_test:
	$(shell sh lib/packet/DeleteProtocol.sh)
	rm $(OBJS)
	rm $(TEST_OBJ)
	rm $(TEST_TARGET)
