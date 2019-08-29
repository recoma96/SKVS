#include "SkvsProtocol.hpp"
#include "../Tokenizer.hpp"
#include "../structure/DataElement.hpp"
#include "../structure/TypePrinter.hpp"
#include "../Exception.hpp"

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <cstring>

using namespace std;
using namespace structure;
using namespace PacketTypeConverter;


char* SkvsProtocol::makePacketSerial(Packet* _targetPacket) {

    string writeMsg;
    writeMsg += "pt\t"+to_string(_targetPacket->getPacketType())+"\n"
                    +"un\t"+_targetPacket->getUserName()+"\n"
                    +"ip\t"+_targetPacket->getIP()+"\n"
                    +"cn\t"+to_string(_targetPacket->getCmdNum())+"\n"
                    +"sk\t"+to_string(_targetPacket->getSock())+"\n";

    
    switch(_targetPacket->getPacketType()) {
        case PACKETTYPE_SENDCMD:
        {
            vector<string> cmdVec = ((SendCmdPacket*)(_targetPacket))->getCmdArray();
            writeMsg += "cv\t";
            //명령어 벡터에 존재하는 string 입력
            for(int i = 0; i < cmdVec.size(); i++) {
                writeMsg += cmdVec[i];
                if(i != cmdVec.size()-1)
                    writeMsg += " ";      
            }
        }   
        break;

        case PACKETTYPE_RECV:
            writeMsg += "rt\t" + to_string(((RecvPacket*)(_targetPacket))->getRecvPacketType()) + "\n";
            if( ((RecvPacket*)(_targetPacket))->getRecvPacketType() == RECVPACKETTYPE_DATA ) {
                DataElement copiedElement = ((RecvDataPacket*)(_targetPacket))->getCopiedData();
                writeMsg += "ded\t" + copiedElement.getDataToString() + "\n";
                writeMsg += "dedt\t" + to_string(copiedElement.getDataType()) + "\n";
                writeMsg += "dest\t" + to_string(copiedElement.getStructType());
            } else {
                writeMsg += "msg\t" + ((RecvMsgPacket*)(_targetPacket))->getMsg();
            }
        break;

        case PACKETTYPE_SIGNAL:
            writeMsg += "sg\t" + to_string(((SignalPacket*)(_targetPacket))->getSignal());
        break;
        case PACKETTYPE_LOG:
            writeMsg += "dft\t"+((LogPacket*)(_targetPacket))->getDateFormat() + "\n";
            writeMsg += "lm\t"+((LogPacket*)(_targetPacket))->getDateStr();
        break;
    }
    

    int strLength = writeMsg.size();
    char* returnSerial = new char[strLength+1];
    strcpy(returnSerial, writeMsg.c_str());
    returnSerial[strLength] = '\0';
    
    return returnSerial;
}

RecvPacketType SkvsProtocol::checkRecvPacketType(const char* _targetStr) {
    //string으로 변경
    string checkStr = _targetStr;
    
    //토큰화
    vector<string> strVec = tok::tokenizer(checkStr, '\n');

    map<string, string> strMap;

    
    //해시맵으로 다시 필터링
    for(int i = 0; i < strVec.size(); i++) {
        vector<string> toked = tok::tokenizer(strVec[i], '\t');
        
        //2개가 아니면 nullptr
        if( toked.size() != 2 )
            throw DataConvertException("Convert Error");
        strMap.insert(pair<string, string>(toked[0], toked[1]));
    }  

    //recvpackettype 추출
    map<string, string>::iterator findRt = strMap.find("rt");
    if( findRt == strMap.end())
        throw DataTypeException("Can't find recv packet type.");
    
    //integer 판정
    CheckDataType checkDataType;
    if(!checkDataType(findRt->second, DATATYPE_NUMBER))
        throw DataTypeException("This Data is not number");
    
    //recvpackettype 판정
    //throw DataConvertException
    RecvPacketType returnType = intToDataType<RecvPacketType>(atoi((findRt->second).c_str()));

    return returnType;

}


template<>
SendCmdPacket* SkvsProtocol::returnToPacket<SendCmdPacket>(const char* _targetStr) {

    //string으로 변경
    string checkStr = _targetStr;
    
    //토큰화
    vector<string> strVec = tok::tokenizer(checkStr, '\n');

    map<string, string> strMap;

    
    //해시맵으로 다시 필터링
    for(int i = 0; i < strVec.size(); i++) {
        vector<string> toked = tok::tokenizer(strVec[i], '\t');
        
        //2개가 아니면 nullptr
        if( toked.size() != 2 ) return nullptr;
        strMap.insert(pair<string, string>(toked[0], toked[1]));
    }  
    
    CheckDataType checkDataType;

    //키/값 검토 및 추출
    //1. pakettype -> pt -> int -> PacketType
    PacketType packetType;
    map<string, string>::iterator findIt = strMap.find("pt");
    if( findIt == strMap.end()) return nullptr;

    if(!checkDataType(findIt->second, DATATYPE_NUMBER))
        return nullptr;
    try 
    {
        packetType = intToDataType<PacketType>(atoi((findIt->second).c_str()));
    } catch(DataConvertException& e) {
        return nullptr;
    }
    
    //2. username
    findIt = strMap.find("un");
    if(findIt == strMap.end()) return nullptr;
    string username = findIt->second;

    //3. ip
    findIt = strMap.find("ip");
    if(findIt == strMap.end()) return nullptr;
    string ip = findIt->second;

    //4.cn -> cmd num
    int cmdNum = 0;
    findIt = strMap.find("cn");
    if(findIt == strMap.end()) return nullptr;

    if(!checkDataType(findIt->second, DATATYPE_NUMBER))
        return nullptr;
    cmdNum = atoi(findIt->second.c_str());
    
    //5. sk-> socket
    int sock = 0;
    findIt = strMap.find("sk");
    if(findIt == strMap.end()) return nullptr;
    if(!checkDataType(findIt->second, DATATYPE_NUMBER))
        return nullptr;
    sock = atoi(findIt->second.c_str());
    //여기까지가 공통 패킷 데이터 수집


    //Cmd수집
    findIt = strMap.find("cv");
    if(findIt == strMap.end()) return nullptr;
    
    //string 쪼개기
    vector<string> cmds = tok::tokenizer(findIt->second, ' ');
    if(cmds.empty()) return nullptr;

    //패킷 생성
    SendCmdPacket* returnPacket = new SendCmdPacket(
        username,
        ip,
        cmdNum,
        sock,
        cmds
    );

    return returnPacket;
}


template<>
RecvDataPacket* SkvsProtocol::returnToPacket<RecvDataPacket>(const char* _targetStr) {

    //string으로 변경
    string checkStr = _targetStr;
    
    //토큰화
    vector<string> strVec = tok::tokenizer(checkStr, '\n');

    map<string, string> strMap;

    
    //해시맵으로 다시 필터링
    for(int i = 0; i < strVec.size(); i++) {
        vector<string> toked = tok::tokenizer(strVec[i], '\t');
        
        //2개가 아니면 nullptr
        if( toked.size() != 2 ) return nullptr;
        strMap.insert(pair<string, string>(toked[0], toked[1]));
    }  
    
    CheckDataType checkDataType;

    //키/값 검토 및 추출
    //1. pakettype -> pt -> int -> PacketType
    PacketType packetType;
    map<string, string>::iterator findIt = strMap.find("pt");
    if( findIt == strMap.end()) return nullptr;

    if(!checkDataType(findIt->second, DATATYPE_NUMBER))
        return nullptr;
    try 
    {
        packetType = intToDataType<PacketType>(atoi((findIt->second).c_str()));
    } catch(DataConvertException& e) {
        return nullptr;
    }
    
    //2. username
    findIt = strMap.find("un");
    if(findIt == strMap.end()) return nullptr;
    string username = findIt->second;

    //3. ip
    findIt = strMap.find("ip");
    if(findIt == strMap.end()) return nullptr;
    string ip = findIt->second;

    //4.cn -> cmd num
    int cmdNum = 0;
    findIt = strMap.find("cn");
    if(findIt == strMap.end()) return nullptr;

    if(!checkDataType(findIt->second, DATATYPE_NUMBER))
        return nullptr;
    cmdNum = atoi(findIt->second.c_str());
    
    //5. sk-> socket
    int sock = 0;
    findIt = strMap.find("sk");
    if(findIt == strMap.end()) return nullptr;
    if(!checkDataType(findIt->second, DATATYPE_NUMBER))
        return nullptr;
    sock = atoi(findIt->second.c_str());


    //DataElement 수집
    string data;
    findIt = strMap.find("ded");
    if(findIt == strMap.end()) return nullptr;
    data = findIt->second;
    

    DataType dType;
    StructType sType;

    findIt = strMap.find("dedt");

    if(findIt == strMap.end()) return nullptr;
    
    if(!checkDataType(findIt->second, DATATYPE_NUMBER)) {
        return nullptr;
    }
    try {
        
        dType = intToDataType<DataType>(atoi((findIt->second).c_str()));
    } catch (DataConvertException& e) {
        return nullptr;
    }

    findIt = strMap.find("dest");
    if(findIt == strMap.end()) return nullptr;
    if(!checkDataType(findIt->second, DATATYPE_NUMBER))
        return nullptr;
    try {
        sType = intToDataType<StructType>(atoi((findIt->second).c_str()));
    } catch (DataConvertException& e) {
        return nullptr;
    }

    //패킷 생성
    DataElement resultElement(data, dType, sType);
    RecvDataPacket* returnPacket = new RecvDataPacket(
        username,
        ip,
        cmdNum,
        sock,
        resultElement
    );

    return returnPacket;

}

template<>
RecvMsgPacket* SkvsProtocol::returnToPacket<RecvMsgPacket>(const char* _targetStr) {

    //string으로 변경
    string checkStr = _targetStr;
    
    //토큰화
    vector<string> strVec = tok::tokenizer(checkStr, '\n');

    map<string, string> strMap;

    
    //해시맵으로 다시 필터링
    for(int i = 0; i < strVec.size(); i++) {
        vector<string> toked = tok::tokenizer(strVec[i], '\t');
        
        //2개가 아니면 nullptr
        if( toked.size() != 2 ) return nullptr;
        strMap.insert(pair<string, string>(toked[0], toked[1]));
    }  
    
    CheckDataType checkDataType;

    //키/값 검토 및 추출
    //1. pakettype -> pt -> int -> PacketType
    PacketType packetType;
    map<string, string>::iterator findIt = strMap.find("pt");
    if( findIt == strMap.end()) return nullptr;

    if(!checkDataType(findIt->second, DATATYPE_NUMBER))
        return nullptr;
    try 
    {
        packetType = intToDataType<PacketType>(atoi((findIt->second).c_str()));
    } catch(DataConvertException& e) {
        return nullptr;
    }
    
    //2. username
    findIt = strMap.find("un");
    if(findIt == strMap.end()) return nullptr;
    string username = findIt->second;

    //3. ip
    findIt = strMap.find("ip");
    if(findIt == strMap.end()) return nullptr;
    string ip = findIt->second;

    //4.cn -> cmd num
    int cmdNum = 0;
    findIt = strMap.find("cn");
    if(findIt == strMap.end()) return nullptr;

    if(!checkDataType(findIt->second, DATATYPE_NUMBER))
        return nullptr;
    cmdNum = atoi(findIt->second.c_str());
    
    //5. sk-> socket
    int sock = 0;
    findIt = strMap.find("sk");
    if(findIt == strMap.end()) return nullptr;
    if(!checkDataType(findIt->second, DATATYPE_NUMBER))
        return nullptr;
    sock = atoi(findIt->second.c_str());

    //6. msg
    string msg;
    findIt = strMap.find("msg");
    if(findIt == strMap.end()) return nullptr;
    msg = findIt->second;


    //패킷 생성
    RecvMsgPacket* returnPacket = new RecvMsgPacket(
        username,
        ip,
        cmdNum,
        sock,
        msg     
    );

    return returnPacket;
}

template<>
SignalPacket* SkvsProtocol::returnToPacket<SignalPacket>(const char* _targetStr) {

    //string으로 변경
    string checkStr = _targetStr;
    
    //토큰화
    vector<string> strVec = tok::tokenizer(checkStr, '\n');

    map<string, string> strMap;

    
    //해시맵으로 다시 필터링
    for(int i = 0; i < strVec.size(); i++) {
        vector<string> toked = tok::tokenizer(strVec[i], '\t');
        
        //2개가 아니면 nullptr
        if( toked.size() != 2 ) return nullptr;
        strMap.insert(pair<string, string>(toked[0], toked[1]));
    }  
    
    CheckDataType checkDataType;

    //키/값 검토 및 추출
    //1. pakettype -> pt -> int -> PacketType
    PacketType packetType;
    map<string, string>::iterator findIt = strMap.find("pt");
    if( findIt == strMap.end()) return nullptr;

    if(!checkDataType(findIt->second, DATATYPE_NUMBER))
        return nullptr;
    try 
    {
        packetType = intToDataType<PacketType>(atoi((findIt->second).c_str()));
    } catch(DataConvertException& e) {
        return nullptr;
    }
    
    //2. username
    findIt = strMap.find("un");
    if(findIt == strMap.end()) return nullptr;
    string username = findIt->second;

    //3. ip
    findIt = strMap.find("ip");
    if(findIt == strMap.end()) return nullptr;
    string ip = findIt->second;

    //4.cn -> cmd num
    int cmdNum = 0;
    findIt = strMap.find("cn");
    if(findIt == strMap.end()) return nullptr;

    if(!checkDataType(findIt->second, DATATYPE_NUMBER))
        return nullptr;
    cmdNum = atoi(findIt->second.c_str());
    
    //5. sk-> socket
    int sock = 0;
    findIt = strMap.find("sk");
    if(findIt == strMap.end()) return nullptr;
    if(!checkDataType(findIt->second, DATATYPE_NUMBER))
        return nullptr;
    sock = atoi(findIt->second.c_str());

    //6.signal
    findIt = strMap.find("sg");
    if(findIt == strMap.end()) return nullptr;
    
    if(!checkDataType(findIt->second, DATATYPE_NUMBER))
        return nullptr;
    
    int sigInt = atoi(findIt->second.c_str());
    
    SignalType sig;

    try{
        sig = intToDataType<SignalType>(sigInt);
    } catch (DataConvertException& e) {
        
        return nullptr;
    }
    


    //패킷 생성
    SignalPacket* returnPacket = new SignalPacket(
        username,
        ip,
        cmdNum,
        sock,
        sig 
    );
    return returnPacket;
}


template<>
LogPacket* SkvsProtocol::returnToPacket<LogPacket>(const char* _targetStr) {

    //string으로 변경
    string checkStr = _targetStr;
    
    //토큰화
    vector<string> strVec = tok::tokenizer(checkStr, '\n');

    map<string, string> strMap;

    
    //해시맵으로 다시 필터링
    for(int i = 0; i < strVec.size(); i++) {
        vector<string> toked = tok::tokenizer(strVec[i], '\t');
        
        //2개가 아니면 nullptr
        if( toked.size() != 2 ) return nullptr;
        strMap.insert(pair<string, string>(toked[0], toked[1]));
    }  
    
    CheckDataType checkDataType;

    //키/값 검토 및 추출
    //1. pakettype -> pt -> int -> PacketType
    PacketType packetType;
    map<string, string>::iterator findIt = strMap.find("pt");
    if( findIt == strMap.end()) return nullptr;

    if(!checkDataType(findIt->second, DATATYPE_NUMBER))
        return nullptr;
    try 
    {
        packetType = intToDataType<PacketType>(atoi((findIt->second).c_str()));
    } catch(DataConvertException& e) {
        return nullptr;
    }
    
    //2. username
    findIt = strMap.find("un");
    if(findIt == strMap.end()) return nullptr;
    string username = findIt->second;

    //3. ip
    findIt = strMap.find("ip");
    if(findIt == strMap.end()) return nullptr;
    string ip = findIt->second;

    //4.cn -> cmd num
    int cmdNum = 0;
    findIt = strMap.find("cn");
    if(findIt == strMap.end()) return nullptr;

    if(!checkDataType(findIt->second, DATATYPE_NUMBER))
        return nullptr;
    cmdNum = atoi(findIt->second.c_str());
    
    //5. sk-> socket
    int sock = 0;
    findIt = strMap.find("sk");
    if(findIt == strMap.end()) return nullptr;
    if(!checkDataType(findIt->second, DATATYPE_NUMBER))
        return nullptr;
    sock = atoi(findIt->second.c_str());


    //6. dateformat
    findIt = strMap.find("dft");
    if(findIt == strMap.end()) return nullptr;
    string dateformat = findIt->second;

    //7. logMsg
    findIt = strMap.find("lm");
    if(findIt == strMap.end()) return nullptr;
    string logMsg = findIt->second;

    //packet
    //패킷 생성
    LogPacket* returnPacket = new LogPacket(
        username,
        ip,
        cmdNum,
        sock,
        logMsg,
        dateformat 
    );
    return returnPacket;
}