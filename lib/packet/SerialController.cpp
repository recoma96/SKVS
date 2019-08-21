#include "SerialController.hpp"
#include "PacketSerial.pb.h"
#include <string>
#include <string.h>
#include <vector>

//Google protocol Buffer
//library code : https://github.com/protocolbuffers/protobuf
//Copyright 2008 Google Inc.  All rights reserved.
//Released under the BSD License

#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/text_format.h>

using namespace std;
using namespace structure;
using namespace PacketSerialData;
using namespace google;

//기본 요소 패킷시리얼 만들기
PacketSerial* makePacketSerial(Packet& _targetPacket) noexcept {
    PacketSerial* returnSerial = new PacketSerial();

    returnSerial->set_packettype(_targetPacket.getPacketType());
    returnSerial->set_username(_targetPacket.getUserName());
    returnSerial->set_ip(_targetPacket.getIP());
    returnSerial->set_cmdnum(_targetPacket.getCmdNum());
    returnSerial->set_sock(_targetPacket.getSock());

    return returnSerial;
}

DataElementSerial* makeDataElementSerial(DataElement& _dataElement) noexcept {

    DataElementSerial* returnSerial = new DataElementSerial();

    returnSerial->set_data(_dataElement.getDataToString());
    returnSerial->set_datatype(_dataElement.getDataType());
    returnSerial->set_structtype(_dataElement.getStructType());

    return returnSerial;
}

RecvPacketSerial* makeRecvPacketSerial(RecvPacket& _recvPacket) noexcept {

    RecvPacketSerial* returnSerial = new RecvPacketSerial();

    PacketSerial* insertedPacket = makePacketSerial(_recvPacket);

    returnSerial->set_allocated_packet(insertedPacket);
    returnSerial->set_recvpackettype(_recvPacket.getRecvPacketType());

    return returnSerial;

}


//패킷 클래스에 따른 직렬화
//1. SendCmdPacket
template<>
char* makePacketToCharArray<SendCmdPacket>(SendCmdPacket& _targetPacket) noexcept {

    SendCmdPacketSerial packetSerial;

    //message 삽입
    PacketSerial* insertedPacket = makePacketSerial(_targetPacket);
    packetSerial.set_allocated_packet( insertedPacket );

    //vector에 있는 데이터 삽입
    vector<string> strArray = _targetPacket.getCmdArray();

    for(vector<string>::iterator iter = strArray.begin(); 
        iter != strArray.end(); iter++ ) {
        packetSerial.add_cmdvector(*iter);
    }

    //직렬화 시행
    int bufSize = packetSerial.ByteSizeLong();
    char* outputBuf = new char[bufSize];

    protobuf::io::ArrayOutputStream os(outputBuf, bufSize);
    packetSerial.SerializeToZeroCopyStream(&os);

    //할당한거 삭제
    //어차피 소멸자에서 삭제함
    //delete insertedPacket;
    return outputBuf;

}

//2. RecvDataPacket
template<>
char* makePacketToCharArray<RecvDataPacket>(RecvDataPacket& _targetPacket) noexcept {

    RecvDataPacketSerial packetSerial;

    //RecvDataPacketSerial에 데이터 삽입
    //1.RecvPacket;
    RecvPacketSerial* insertedPacket = makeRecvPacketSerial(_targetPacket);
    packetSerial.set_allocated_recvpacket(insertedPacket);

    //2.DataElement
    DataElementSerial* insertedData = makeDataElementSerial(_targetPacket.getData());
    packetSerial.set_allocated_data(insertedData);

    //직렬화 시행
    int bufSize = packetSerial.ByteSizeLong();
    char* outputBuf = new char[bufSize];

    protobuf::io::ArrayOutputStream os(outputBuf, bufSize);
    packetSerial.SerializeToZeroCopyStream(&os);

    return outputBuf;
}

//3. RecvMsgPacket
template<>
char* makePacketToCharArray<RecvMsgPacket>(RecvMsgPacket& _targetPacket) noexcept {

    RecvMsgPacketSerial packetSerial;

    //데이터 삽입
    //1.RecvPacket;
    RecvPacketSerial* insertedPacket = makeRecvPacketSerial(_targetPacket);
    packetSerial.set_allocated_recvpacket(insertedPacket);

    packetSerial.set_msg(_targetPacket.getMsg());

    //직렬화 시행
    int bufSize = packetSerial.ByteSizeLong();
    char* outputBuf = new char[bufSize];

    protobuf::io::ArrayOutputStream os(outputBuf, bufSize);
    packetSerial.SerializeToZeroCopyStream(&os);

    return outputBuf;
}

//4.signalPacket
template<>
char* makePacketToCharArray<SignalPacket>(SignalPacket& _targetPacket) noexcept {
    
    SignalPacketSerial packetSerial;

    //message 삽입
    PacketSerial* insertedPacket = makePacketSerial(_targetPacket);
    packetSerial.set_allocated_packet( insertedPacket );

    packetSerial.set_signal(_targetPacket.getSignal());

    //직렬화 시행
    int bufSize = packetSerial.ByteSizeLong();
    char* outputBuf = new char[bufSize];

    protobuf::io::ArrayOutputStream os(outputBuf, bufSize);
    packetSerial.SerializeToZeroCopyStream(&os);

    return outputBuf;
}

//5. LogPacket
template <>
char* makePacketToCharArray<LogPacket>(LogPacket& _targetPacket) noexcept {

    LogPacketSerial packetSerial;
    PacketSerial* insertedPacket = makePacketSerial(_targetPacket);
    packetSerial.set_allocated_packet(insertedPacket);

    packetSerial.set_dateformat(_targetPacket.getDateFormat());
    packetSerial.set_str(_targetPacket.getDateFormat());

    //직렬화 시행
    int bufSize = packetSerial.ByteSizeLong();
    char* outputBuf = new char[bufSize];

    protobuf::io::ArrayOutputStream os(outputBuf, bufSize);
    packetSerial.SerializeToZeroCopyStream(&os);

    return outputBuf;

}
//리시브패킷의 새부타입
const RecvPacketType whatIsRecvPacketTypeInRecvDataSerial(char* _targetStr) {
    try {

        const int bufSize = strlen(_targetStr);
        protobuf::io::ArrayInputStream is(_targetStr, bufSize);

        RecvDataPacketSerial packetSerial;
        packetSerial.ParseFromZeroCopyStream(&is);
        RecvPacketSerial* check = packetSerial.release_recvpacket();

        RecvPacketType returnType = PacketTypeConverter::intToDataType<RecvPacketType>(check->recvpackettype());
        return returnType;
    } catch(DataConvertException e) {
        throw e;
    }
}

//역직렬화
//1.SendPacket
template <>
SendCmdPacket* returnToPacket<SendCmdPacket>(char* _targetSerializeStr) noexcept {

    const int bufSize = strlen(_targetSerializeStr);
    protobuf::io::ArrayInputStream is(_targetSerializeStr, bufSize);

    SendCmdPacketSerial recvSerial;
    PacketSerial* recvSuperSerial = nullptr; //Packet 클래스

    recvSerial.ParseFromZeroCopyStream(&is);

    //packet추출
    recvSuperSerial = recvSerial.release_packet();

    //SendPacket의 string덩어리 복사
    vector<string> recvVec;
    const int vecSize = recvSerial.cmdvector_size();
    recvVec.resize(vecSize);

    for(int i = 0; i<vecSize; i++) {
        recvVec[i] = recvSerial.cmdvector(i);
    }

    //Packet으로부터 데이터 추출
    string username = recvSuperSerial->username();
    string IP = recvSuperSerial->ip();
    int cmdNum = recvSuperSerial->cmdnum();
    int sock = recvSuperSerial->sock();

    delete recvSuperSerial;

    return new SendCmdPacket(
        username,
        IP,
        cmdNum,
        sock,
        recvVec
    );
}

//2. RecvDataPacket
template <>
RecvDataPacket* returnToPacket<RecvDataPacket>(char* _targetSerializeStr) noexcept {

    const int bufSize = strlen(_targetSerializeStr);
    protobuf::io::ArrayInputStream is(_targetSerializeStr, bufSize);

    RecvDataPacketSerial recvSerial;
    PacketSerial* superSerial = nullptr; //packet 상위클래스
    DataElementSerial* dataElementSerial = nullptr; //dataElementSerial;

    recvSerial.ParseFromZeroCopyStream(&is);

    //DataElement 수집
    dataElementSerial = recvSerial.release_data();
    DataElement recvElement = DataElement(
        dataElementSerial->data(),
        PacketTypeConverter::intToDataType<DataType>(dataElementSerial->datatype()),
        PacketTypeConverter::intToDataType<StructType>(dataElementSerial->structtype())
    );

    delete dataElementSerial;

    //recv 상위클래스
    superSerial = recvSerial.release_recvpacket()->release_packet();

    //패킷 대이터 수집
    string username = superSerial->username();
    string IP = superSerial->ip();
    int cmdNum = superSerial->cmdnum();
    int sock = superSerial->sock();

    delete superSerial;

    return new RecvDataPacket(
        username,
        IP,
        cmdNum,
        sock,
        recvElement
    );

}

//3. recvMsgPacket
template <>
RecvMsgPacket* returnToPacket<RecvMsgPacket>(char* _targetSerializeStr) noexcept {

    const int bufSize = strlen(_targetSerializeStr);
    protobuf::io::ArrayInputStream is(_targetSerializeStr, bufSize);

    RecvMsgPacketSerial recvSerial;
    PacketSerial* superSerial = nullptr; //packet 상위클래스

    recvSerial.ParseFromZeroCopyStream(&is);

    string msg = recvSerial.msg();

    //recv 상위클래스
    superSerial = recvSerial.release_recvpacket()->release_packet();

    //패킷 대이터 수집
    string username = superSerial->username();
    string IP = superSerial->ip();
    int cmdNum = superSerial->cmdnum();
    int sock = superSerial->sock();

    delete superSerial;

    return new RecvMsgPacket(
        username,
        IP,
        cmdNum,
        sock,
        msg
    );

}

//4. signalPacket
template <>
SignalPacket* returnToPacket<SignalPacket>(char* _targetSerializeStr) noexcept {
    
    const int bufSize = strlen(_targetSerializeStr);
    protobuf::io::ArrayInputStream is(_targetSerializeStr, bufSize);

    SignalPacketSerial recvSerial;
    PacketSerial* superSerial = nullptr;

    recvSerial.ParseFromZeroCopyStream(&is);

    SignalType signal = PacketTypeConverter::intToDataType<SignalType>(recvSerial.signal());

    superSerial = recvSerial.release_packet();

    //패킷 대이터 수집
    string username = superSerial->username();
    string IP = superSerial->ip();
    int cmdNum = superSerial->cmdnum();
    int sock = superSerial->sock();

    delete superSerial;

    return new SignalPacket(
        username,
        IP,
        cmdNum,
        sock,
        signal
    );
}

//5.LogPacket
template <>
LogPacket* returnToPacket<LogPacket>(char* _targetSerializeStr) noexcept {

    const int bufSize = strlen(_targetSerializeStr);
    protobuf::io::ArrayInputStream is(_targetSerializeStr, bufSize);

    LogPacketSerial recvSerial;
    PacketSerial* superSerial = nullptr;

    recvSerial.ParseFromZeroCopyStream(&is);

    //dateformat, str 수집
    string dateformat = recvSerial.dateformat();
    string str = recvSerial.str();

    superSerial = recvSerial.release_packet();

    //패킷 대이터 수집
    string username = superSerial->username();
    string IP = superSerial->ip();
    int cmdNum = superSerial->cmdnum();
    int sock = superSerial->sock();

    delete superSerial;

    return new LogPacket(
        username,
        IP,
        cmdNum,
        sock,
        str,
        dateformat
    );
}