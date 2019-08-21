#include "../lib/threadAdapter/AdapterThreadUtility.hpp"
#include "../lib/threadAdapter/AdapterThreadBridge.hpp"
#include "../lib/logbase/LogBase.hpp"
#include"../lib/packet/Packet.hpp"

#include <string>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;
using namespace ThreadAdapter;

extern bool shutdownSignal;
extern string logRoot;

void LogThread(shared_ptr<AdapterThreadUtility> adapterUtility, bool* logThreadisDead) {
    
    LogBase logBase(logRoot, adapterUtility);

    //TODO log활용 관련 기능을 가진 스레드 추가 예정

    while(!shutdownSignal) {
        
        //input queue에 데이터들어올때까지 대기
        while(adapterUtility->isInputQueueEmpty()) {
            this_thread::sleep_for(chrono::milliseconds(1));
            if(shutdownSignal) return;
        }

        Packet* recvPacket = adapterUtility->popInInputQueue();

        //데이터 타입 확인
        if(recvPacket->getPacketType() != PACKETTYPE_LOG) {
            cerr << "System Error : Packet Leak From LogThread" << endl;
            cerr << "Packet Type Number : " << recvPacket->getPacketType() << endl;
            delete recvPacket;
            continue;
        }
        
        //파일에 로그 데이터 쓰기
        if(!logBase.writeLogToFile((LogPacket*)(recvPacket))) {
            cout << "Log Directory is not exist" << endl;
            *logThreadisDead = true;
            return;
        }
        delete (LogPacket*)(recvPacket);
        continue;


        //서버에 프린팅    

    }
    
}