#include "../lib/threadAdapter/AdapterThreadUtility.hpp"
#include "../lib/threadAdapter/AdapterThreadBridge.hpp"
#include "../lib/logbase/LogBase.hpp"

#include <thread>
#include <chrono>
#include <map>
#include <queue>
#include <deque>

extern bool shutdownSignal;
extern unsigned int LogAdapterSerial_input;
extern unsigned int DBAdapterSerial_input;

using namespace ThreadAdapter;

//데이터베이스로부터 추출된 패킷을 보내는 스레드
void StandAloneAdapterSendThreadToDataBase(
    shared_ptr<AdapterThreadBridge> _adpaterBridgeQueue,
    map< int, weak_ptr<queue<Packet*, deque<Packet*>>> >* packetBridge,
    shared_ptr<AdapterThreadUtility> _pipe
) {

    weak_ptr<AdapterThreadBridge> adapterBridgeQueue = _adpaterBridgeQueue;
    weak_ptr<AdapterThreadUtility> pipe = _pipe;

    Packet* sendPacket = nullptr;
    LogPacket* logPacket = nullptr;
    //내부에서의 데이터 패킷 큐를 검사 (Output)
    while(!shutdownSignal) {
        
        
        
        if(!pipe.lock()->isOutputQueueEmpty()) {
            //패킷 검사
            sendPacket = pipe.lock()->popInOutputQueue();
            // DB로부터 추출되는 패킷은
            // Cmd를 제외한 나머지 3가지
            switch( sendPacket->getPacketType()) {
                case PACKETTYPE_LOG:
                    //로그스레드로 이동
                    adapterBridgeQueue.lock()->pushInQueue(sendPacket, LogAdapterSerial_input);
                break;

                case PACKETTYPE_SIGNAL:
                case PACKETTYPE_RECV:
                    //해당 소켓에 따른 sendthread로 이동
                {
                    //해당 소켓에 대한 패킷큐가 없을 경우 
                    //패킷 소멸

                    map< int, weak_ptr<queue<Packet*, deque<Packet*>>> >::iterator foundPacket = 
                        packetBridge->find(sendPacket->getSock());
                    if( foundPacket == packetBridge->end()) {
                        delete sendPacket;
                    } else {
                        foundPacket->second.lock()->push(sendPacket);
                    }
                }
                break;
                default: //SendCmdPacket -> 오류
                    logPacket = new LogPacket(sendPacket->getUserName(), sendPacket->getIP(), 
                                0, 0, "System Error : SendCmdPacket is generated from DataBase Thread" );
                    adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);
                    delete sendPacket;
                break;
            }

            sendPacket = nullptr;
        }
        
        this_thread::sleep_for(chrono::microseconds(10));
    }
}