/***
 *  @file : SerialController.hpp
 * 	@date 2019/08/18
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 패킷의 직렬화/역직렬화를 할 때 사용하는 함수 입니다.
 * 
 ***/

#ifndef SERIALCONTROLLER_HPP
# define SERIALCONTORLLER_HPP

#include "Packet.hpp"
#include "PacketSerial.pb.h"

#include <string>

using namespace std;
using namespace structure;
using namespace PacketSerialData;

//PacketSerial 생성
PacketSerial* makePacketSerial(Packet& _targetPacket) noexcept;

//DataElementSerai 생성
DataElementSerial* makeDataElementSerial(DataElement& _dataElement) noexcept;

//각 protobuf 클래스에 따른 직렬화
template <class TargetSerial>
char* makePacketToCharArray(TargetSerial& _targetPacket) noexcept;

//Recv패킷 시리얼에 대한 세부 종류 (Msg, Data)를 알아보는 함수
//<직전에 패킷 타입을 알아보는 함수를 사용해야 이 함수를 사용 할 수 있습니다.>
const RecvPacketType whatIsRecvPacketTypeInRecvDataSerial(char* _targetStr);

//역직렬화
template <class TargetSerial>
TargetSerial* returnToPacket(char* _targetSerializeStr) noexcept;

#endif