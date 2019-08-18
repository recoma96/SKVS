/***
 *  @file : Exception.hpp
 * 	@date 2019/08/11
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 서버와 클라이언트, 스레드 간의 통신을 할 때 이 클래스를 사용합니다.
 * 
 ***/

#ifndef PACKET_HPP
# define PAKCET_HPP

#include <time.h>
#include <string>
#include <sys/timeb.h>
#include <vector>

#include "../Tokenizer.hpp"
#include "../structure/DataElement.hpp"
using namespace std;

//시간 포매팅
namespace CalTime {

    string currentDataTime(void) {

	    struct timeb timebuf;
	    struct tm* now;
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
    SIGNALTYPE_SETEND,
    SIGNALTYPE_ERROR
};

class Packet {
protected:
    PacketType packetType;
    string username;
    string IP;
    int cmdNum;
    int sock;
public:
    Packet(const PacketType _ptype, const string _username, const string _IP,
            const int _cmdNum, const int _sock ) {
        
        packetType = _ptype; username = _username;
        IP = _IP; cmdNum = _cmdNum; sock = _sock;
    }
    
    const PacketType getPacketType(void) { return packetType; }
    const string getUserName(void) { return username; }
    const string getIP(void) { return IP; }
    const int getCmdNum(void) { return cmdNum; }
    const int getSock(void) { return sock; }
    void setSock(int _sock) { sock = _sock; }
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
    const vector<string>& getCmdArray(void) { return cmdArray; }
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
    const RecvPacketType getRecvPacketType(void) { return recvPacketType; }
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
    const structure::DataElement getData(void) {  return data; }
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
    const string getMsg(void) {  return  msg; }
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
    const SignalType getSignal(void) { return signal; }
};


class LogPacket : public Packet {
private:
    string dateFormat;
    string str;
public:
    LogPacket(const string _username, const string _IP, const int _cmdNum,
                const int _sock, const string _str) :
            Packet(PACKETTYPE_LOG, _username, _IP, _cmdNum, _sock) {
        str = _str;
        dateFormat = CalTime::currentDataTime();
    }
    const string getDateFormat(void) { return dateFormat; }
    const string getDateStr(void) { return str; }
    const string getStatement(void) {

        string resultStr = dateFormat;
        resultStr += " ";
        resultStr += str;

        return resultStr;
    }
};

#endif