/***
 *  @file : Packet.hpp
 * 	@date 2019/08/18
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 서버와 클라이언트, 스레드 간의 통신을 할 때 이 클래스를 사용합니다.
 * 
 ***/

#ifndef PACKET_HPP
# define PACKET_HPP

#include <time.h>
#include <string>
#include <sys/timeb.h>
#include <vector>

#include "../Tokenizer.hpp"
#include "../structure/DataElement.hpp"
#include "../Exception.hpp"
using namespace std;

//시간 포매팅
namespace CalTime {

    string currentDataTime(void);
    string makeLogDataFileName(void); //Log파일 정할 때 사용
};


enum PacketType {
    PACKETTYPE_SENDCMD,
    PACKETTYPE_RECV,
    PACKETTYPE_SIGNAL,
    PACKETTYPE_LOG
};

enum RecvPacketType {
    RECVPACKETTYPE_DATA,
    RECVPACKETTYPE_MSG
};

enum SignalType {
    SIGNALTYPE_SHUTDOWN,
    SIGNALTYPE_RECVSTART,
    SIGNALTYPE_RECVEND,
    SIGNALTYPE_ERROR
};

//throw DataConvertException
namespace PacketTypeConverter {
    
    template<typename T>
    const T intToDataType(const int _type);
    
};

class Packet {
protected:
    PacketType packetType;
    string username;
    string IP;
    int cmdNum;
    int sock; //소켓 번호(서버측)
public:
    Packet(const PacketType _ptype, const string _username, const string _IP,
            const int _cmdNum, const int _sock ) {
        
        packetType = _ptype; username = _username;
        IP = _IP; cmdNum = _cmdNum; sock = _sock;
    }
    
    inline const PacketType getPacketType(void) { return packetType; }
    inline const string getUserName(void) { return username; }
    inline const string getIP(void) { return IP; }
    inline const int getCmdNum(void) { return cmdNum; }
    inline const int getSock(void) { return sock; }
    inline void setSock(int _sock) { sock = _sock; }
};

//서버에 요청을 보낼 때 사용하는 패킷
class SendCmdPacket : public Packet {
private:
    vector<string> cmdArray;
public:
    SendCmdPacket(const string _username, const string _IP,
            const int _cmdNum, const int _sock, const string _cmd) : 
            Packet(PACKETTYPE_SENDCMD, _username, _IP, _cmdNum, _sock) {
        cmdArray = tok::tokenizer(_cmd);
    }
    SendCmdPacket(const string _username, const string _IP,
            const int _cmdNum, const int _sock, vector<string> _cmdVec) : 
            Packet(PACKETTYPE_SENDCMD, _username, _IP, _cmdNum, _sock) {
        cmdArray = _cmdVec;
    }
    inline const vector<string>& getCmdArray(void) { return cmdArray; }
};

//서버로부터 데이터를 받을 때 사용합니다.
class RecvPacket : public Packet {
private:
    RecvPacketType recvPacketType;
public:
    RecvPacket(const string _username, const string _IP,
            const int _cmdNum, const int _sock, const RecvPacketType _recvPType) :
            Packet(PACKETTYPE_RECV, _username, _IP, _cmdNum, _sock) {
        recvPacketType = _recvPType;
    }
    inline const RecvPacketType getRecvPacketType(void) { return recvPacketType; }
};

//데이터 수신 패킷
class RecvDataPacket : public RecvPacket {
private:
    structure::DataElement data;
public:
    RecvDataPacket(const string _username, const string _IP,
                    const int _cmdNum, const int _sock, structure::DataElement _data) :
                RecvPacket(_username, _IP, _cmdNum, _sock, RECVPACKETTYPE_DATA) {
        data = _data;
    }
    //클라이언트로부터 받은 패킷정보를 이용해서 패킷을 생성합니다.
    RecvDataPacket(SendCmdPacket& _reqPacket, structure::DataElement _data) : 
        RecvPacket(_reqPacket.getUserName(), _reqPacket.getIP(), _reqPacket.getCmdNum(), 
                    _reqPacket.getSock(), RECVPACKETTYPE_DATA) {

        data = _data;
    }
    inline const structure::DataElement getCopiedData(void) {  return data; }
    inline structure::DataElement& getData(void) { return data; }
};

class RecvMsgPacket : public RecvPacket {
private:
    string msg;
public:
    RecvMsgPacket(const string _username, const string _IP,
                    const int _cmdNum, const int _sock, const string _msg) :
                RecvPacket(_username, _IP, _cmdNum, _sock, RECVPACKETTYPE_MSG) {
        msg = _msg;
    }
    //클라이언트로부터 받은 패킷정보를 이용해서 패킷을 생성합니다.
    RecvMsgPacket(SendCmdPacket& _reqPacket, const string _msg) : 
        RecvPacket(_reqPacket.getUserName(), _reqPacket.getIP(), _reqPacket.getCmdNum(), 
                    _reqPacket.getSock(), RECVPACKETTYPE_MSG) {

        msg = _msg;
    }
    inline const string getMsg(void) {  return  msg; }
};


class SignalPacket : public Packet {
private:
    SignalType signal;
public:
    SignalPacket(const string _username, const string _IP,
                    const int _cmdNum, const int _sock, const SignalType _signal ) :
                Packet(PACKETTYPE_SIGNAL, _username, _IP, _cmdNum, _sock) {
        signal = _signal;
    }
    //클라이언트로부터 받은 패킷정보를 이용해서 패킷을 생성합니다.
    SignalPacket(SendCmdPacket& _reqPacket, const SignalType _signal) : 
        Packet(PACKETTYPE_SIGNAL, _reqPacket.getUserName(), _reqPacket.getIP(), _reqPacket.getCmdNum(), 
                    _reqPacket.getSock()) {

        signal = _signal;
    }
    inline const SignalType getSignal(void) { return signal; }
};


class LogPacket : public Packet {
private:
    string dateFormat;
    string str;
public:
    //특정 장소에서 생성되었을 경우
    LogPacket(const string _username, const string _IP, const int _cmdNum,
                const int _sock, const string _str) :
            Packet(PACKETTYPE_LOG, _username, _IP, _cmdNum, _sock) {
        str = _str;
        dateFormat = CalTime::currentDataTime();
    }
    //클라이언트로부터 받은 패킷정보를 이용해서 패킷을 생성합니다.
    LogPacket(SendCmdPacket& _reqPacket, const string _str) : 
        Packet(PACKETTYPE_LOG, _reqPacket.getUserName(), _reqPacket.getIP(), _reqPacket.getCmdNum(), 
                    _reqPacket.getSock()) {
        
        str = _str;
        dateFormat = CalTime::currentDataTime();
    }

    //다른 스레드 또는 다른 서버로부터 오는 경우
    //dateformat의 날짜가 변경되지 말아야 함
    LogPacket(const string _username, const string _IP, const int _cmdNum,
                const int _sock, const string _str, const string _dateformat) :
            Packet(PACKETTYPE_LOG, _username, _IP, _cmdNum, _sock) {
        str = _str;
        dateFormat = _dateformat;
    }

    inline const string getDateFormat(void) { return dateFormat; }
    inline const string getDateStr(void) { return str; }
    inline const string getStatement(void) {

        string resultStr = dateFormat;

        resultStr += " [IP : ";
        resultStr += IP;
        resultStr += "]";
        
        resultStr += " ";
        resultStr += str;

        return resultStr;
    }
};

#endif