#!bin/bash
protoc -I=lib/packet --cpp_out=lib/packet lib/packet/PacketSerial.proto
g++ -g -c -std=c++17 -o lib/packet/PacketSerial.pb.o lib/packet/PacketSerial.pb.cc -lprotobuf