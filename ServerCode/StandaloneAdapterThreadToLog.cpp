#include "../lib/threadAdapter/AdapterThreadUtility.hpp"
#include "../lib/threadAdapter/AdapterThreadBridge.hpp"
#include "../lib/logbase/LogBase.hpp"

#include <thread>

using namespace ThreadAdapter;

extern unsigned int LogAdapterSerial_input;
extern bool shutdownSignal;

//로그 스레드
void LogThread(shared_ptr<AdapterThreadUtility> adapterUtility, bool* logThreadisDead);

void StandaloneAdapterThreadToLog(shared_ptr<AdapterThreadBridge> _adpaterBridgeQueue) {   
    
    weak_ptr<AdapterThreadBridge> adapterBridgeQueue = _adpaterBridgeQueue;

    //어댑터 내부의 input, output 패킷 큐
    shared_ptr<AdapterThreadUtility> pipe = make_shared<AdapterThreadUtility>();

    bool logThreadisDead = false; //logthread 실행 여부

    //로그스레드 실행
    thread logThread = thread(LogThread, pipe, &logThreadisDead);

    //AdapterSendThread 실행 <로그에 대한 추가 기능이 들어갈 경우 구현 예정>
    Packet* recvPacket = nullptr;
    while(!logThreadisDead) {

        shared_ptr<AdapterThreadBridge> checkPacket = adapterBridgeQueue.lock();

        //shutdownSignal확인하는것도 포함해야 하므로 nonblock
        recvPacket = checkPacket->popInQueue(LogAdapterSerial_input, false);

        if( recvPacket != nullptr) {
            //adapterQueue에 삽입
            pipe->pushInInputQueue(recvPacket);
            recvPacket = nullptr;
            continue;
        }

        this_thread::sleep_for(chrono::milliseconds(1));
    }

    logThread.join();
}