#include "../lib/threadAdapter/AdapterThreadUtility.hpp"
#include "../lib/threadAdapter/AdapterThreadBridge.hpp"
#include "../lib/logbase/LogBase.hpp"

#include <thread>
#include <chrono>
#include <map>
#include <queue>
#include <deque>

using namespace ThreadAdapter;

extern unsigned int LogAdapterSerial_input;
extern unsigned int DBAdapterSerial_input;
extern bool shutdownSignal;

extern void DataBaseThread(shared_ptr<AdapterThreadUtility> _pipe);

extern void StandAloneAdapterSendThreadToDataBase(
    shared_ptr<AdapterThreadBridge> _adpaterBridgeQueue,
    map< int, weak_ptr<queue<Packet*, deque<Packet*>>> >* packetBridge,
    shared_ptr<AdapterThreadUtility> _pipe
);


void StandAloneAdapterThreadToDataBase(
    shared_ptr<AdapterThreadBridge> _adpaterBridgeQueue,
    map< int, weak_ptr<queue<Packet*, deque<Packet*>>> >* packetBridge
) {

    
    weak_ptr<AdapterThreadBridge> adapterBridgeQueue = _adpaterBridgeQueue;
    shared_ptr<AdapterThreadUtility> pipe = make_shared<AdapterThreadUtility>();
    bool dataBaseisDead = false;

    Packet* recvPacket = nullptr;

    //SendThread 실행

    //DataBaseThread 실행
    thread dataBase(
        DataBaseThread,
        pipe
    );
    thread sendThread(
        StandAloneAdapterSendThreadToDataBase,
        _adpaterBridgeQueue,
        packetBridge,
        pipe
    );
    while(!shutdownSignal) {
        //외부로부터 데이터가 들어오는 것을 확인

        recvPacket = adapterBridgeQueue.lock()->popInQueue(DBAdapterSerial_input, false);

        if( recvPacket != nullptr) {
            pipe->pushInInputQueue(recvPacket);
            recvPacket = nullptr;
            continue;
        }
        this_thread::sleep_for(chrono::milliseconds(1));
    }
    dataBase.join();
    sendThread.join();


}