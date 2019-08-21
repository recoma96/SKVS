#include "Packet.hpp"
#include <malloc.h>

using namespace structure;

//시간 포맷팅
string CalTime::currentDataTime(void) {

	struct timeb timebuf;
	struct tm* now = nullptr;
	time_t time;
	int millisec;

	ftime(&timebuf);
	time = timebuf.time;
	millisec = timebuf.millitm;

	now = localtime(&time);
	string resultStr("[");
	resultStr.append( to_string(now->tm_year+1900) ).append("/").append( to_string(now->tm_mon) ).
		    append("/").append( to_string(now->tm_mday) ).append(" ").append( to_string(now->tm_hour)).
		    append(":").append( to_string(now->tm_min) ).append(":").append( to_string(now->tm_sec) ).
		    append(":").append( to_string(millisec) ).append("]");

	return resultStr;
}

string CalTime::makeLogDataFileName(void) {
    struct timeb timebuf;
    struct tm* now = nullptr;
    time_t time;

    ftime(&timebuf);
    time = timebuf.time;
    now = localtime(&time);

    string resultStr;
    resultStr.append(to_string(now->tm_year+1900)).append("_").append(to_string(now->tm_mon)).
            append("_").append(to_string(now->tm_mday)).append(".log");


    return resultStr;
}


template<>
const DataType PacketTypeConverter::intToDataType<DataType>(const int _type) {
    switch(_type) {
        case 0: return DATATYPE_STRING;
        case 1: return DATATYPE_NUMBER;
        case 2: return DATATYPE_FLOAT;
        default:
            throw DataConvertException("This integer is over about Data Type");
    }
}

template<>
const StructType PacketTypeConverter::intToDataType<StructType>(const int _type) {
    switch(_type) {
        case 0: return STRUCTTYPE_ELEMENT;
        case 1: return STRUCTTYPE_BASIC;
        case 2: return STRUCTTYPE_ONESET;
        case 3: return STRUCTTYPE_MULTISET;
        case 4: return STRUCTTYPE_DYNAMICLIST;
        case 5: return STRUCTTYPE_STATICLIST;
        case 6: return STRUCTTYPE_DYNAMICHASHMAP;
        case 7: return STRUCTTYPE_STATICHASHMAP;
        default:
            throw DataConvertException("This integer is over about Struct Type");
    }
}

template<>
const PacketType PacketTypeConverter::intToDataType<PacketType>(const int _type) {
    switch(_type) {
        case 0: return PACKETTYPE_SENDCMD;
        case 1: return PACKETTYPE_RECV;
        case 2: return PACKETTYPE_SIGNAL;
        case 3: return PACKETTYPE_LOG;
        default:
            throw DataConvertException("This integer is over about Struct Type");
    }
}

template<>
const RecvPacketType PacketTypeConverter::intToDataType<RecvPacketType>(const int _type) {
    switch(_type) {
        case 0: return RECVPACKETTYPE_DATA;
        case 1: return RECVPACKETTYPE_MSG;
        default:
            throw DataConvertException("This integer is over about RecvPacket Type");
    }
}

template<>
const SignalType  PacketTypeConverter::intToDataType<SignalType>(const int _type) {
    switch(_type) {
        case 0: return SIGNALTYPE_SHUTDOWN;
        case 1: return SIGNALTYPE_RECVSTART;
        case 2: return SIGNALTYPE_RECVEND;
        case 3: return SIGNALTYPE_ERROR;
        default:
            throw DataConvertException("This integer is over about Signal Type");
    }
}