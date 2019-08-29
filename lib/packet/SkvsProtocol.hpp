#ifndef SKVSPROTOCOL_HPP
# define SKVSPROTOCOL_HPP

#include <string>

#include "Packet.hpp"
#include "../structure/DataElement.hpp"

/******
 * 공통 패킷 모델 \n로 토큰화해서 데이터 판단)
 * Packet
 * pt-{integer} //packettype
 * un-{string}  //username
 * ip-{string}  //ip
 * cn-{integer} //cmdnum
 * sk-{integer} //socket
 * 
 * 1.SendCmdPacket
 * pt-{integer} 
 * un-{string}
 * ip-{string}
 * cn-{integer}
 * sk-{integer}
 * cv-{string string string strin}....
 * 
 * 2. RecvDataPacket
 * pt-{integer}
 * un-{string}
 * ip-{string}
 * cn-{integer}
 * sk-{integer}
 * rt-{integer}     //recvPacketType
 * ded-{'string'}   //dataelement-string
 * dedt-{integer}   //dataelement-datatype
 * dest-{integer}   //dataelement-structtype
 * 
 * 
 * 3.RecvMsgPacket
 * pt-{integer}
 * un-{string}
 * ip-{string}
 * cn-{integer}
 * sk-{integer}
 * rt-{integer}   
 * msg-{'string'}
 * 
 * 4.signalPacket
 * pt-{integer}
 * un-{string}
 * ip-{string}
 * cn-{integer}
 * sk-{integer}
 * sg-{integer}
 * 
 * 5.logPacket
 * pt-{integer}
 * un-{string}
 * ip-{string}
 * cn-{integer}
 * sk-{integer}
 * dft-{string}
 * lm-{'string'}
 * 
 */
namespace SkvsProtocol {

    char* makePacketSerial(Packet* _targetPacket);

    template<class TargetSerial>
    TargetSerial* returnToPacket(const char* _targetStr);

    //recvpacket 종류 판별
    RecvPacketType checkRecvPacketType(const char* _targetStr);

}
#endif