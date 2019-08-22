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

    while(!dataBaseisDead) {
        //외부로부터 데이터가 들어오는 것을 확인

        recvPacket = adapterBridgeQueue.lock()->popInQueue(DBAdapterSerial_input, false);

        if( recvPacket != nullptr) {
            //pipe->pushInInputQueue(recvPacket);
            cout << "ok" << endl;
            delete recvPacket;
            recvPacket = nullptr;
            continue;
        }
        this_thread::sleep_for(chrono::microseconds(10));
    }

    sendThread.join();
    dataBaseThread.join();
}