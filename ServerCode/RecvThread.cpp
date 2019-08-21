#include "../lib/packet/Packet.hpp"
#include "../lib/user/LoginedUser.hpp"
#include "../lib/CommandFilter.hpp"
#include "../lib/SockWrapper/ServerSocketManager.hpp"
#include "../lib/SockWrapper/NetworkingManager.hpp"
#include "../lib/SockWrapper/SocketManager.hpp"
#include "../lib/threadAdapter/AdapterThreadBridge.hpp"
#include "../lib/packet/SerialController.hpp"
#include "../lib/CommandList.hpp"

#include <string>
#include <deque>
#include <queue>
#include <mutex>
#include <vector>
#include <chrono>
#include <thread>
#include <string.h>

using namespace std;
using namespace SockWrapperForCplusplus;
using namespace PacketSerialData;
using namespace CommandList;

extern unsigned int LogAdapterSerial_input;
extern unsigned int DBAdapterSerial_input;
extern unsigned int DBAdapterSerial_output;

extern bool shutdownSignal; //종료 시그널

void RecvThread(Socket* socket,
                LoginedUser* user,
                CommandFilter* cmdFilter,
                shared_ptr<queue<Packet*, deque<Packet*>>> _sendPacketQueue,
                weak_ptr<ThreadAdapter::AdapterThreadBridge> _adapterBridgeQueue,
                bool* isDisConnected) {
    
    //다른 스레드로 패킷을 이동할 때 사용함
    weak_ptr<ThreadAdapter::AdapterThreadBridge> adapterBridgeQueue = _adapterBridgeQueue;

    //SendThread로 패킷을 보냄
    weak_ptr<queue<Packet*, deque<Packet*>>> sendPacketQueue = _sendPacketQueue;

    while(!shutdownSignal && !(*isDisConnected)) {

        int recvBufSize = 0;
        char* recvBuf = nullptr;
        LogPacket* logPacket = nullptr;

        //데이터받기
        if( recvData(socket, &recvBufSize, sizeof(int)) <= 0 ) {
            
            //클라이언트를 종료 시킬 경우 이 스레드가 남으므로
            //차후에 개선할 예정
            if(*isDisConnected != true) {
                logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, "Server DisConnected");
                cerr << logPacket->getStatement() << endl;
                adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);
            }

            *isDisConnected = true;
            continue;
        }

        
        PacketType recvType; //클라이언트로부터 받는 패킷 타입

        if( recvData(socket, &recvType, sizeof(PacketType)) <= 0) {

            logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, "Server DisConnected");
            cerr << logPacket->getStatement() << endl;
            adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);

            *isDisConnected = true;
            continue;
        }

        recvBuf = new char[recvBufSize]; //데이터받을 버퍼
        if( recvData(socket, recvBuf, recvBufSize) <= 0) {

            logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, "Server DisConnected");
            cerr << logPacket->getStatement() << endl;
            adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);

            *isDisConnected = true;
            continue;
        }

         //데이터 역직렬화
        SendCmdPacket* recvPacket = nullptr;

        //SendCmd나 log가 들어오면 안됨
        switch( recvType ) {

            case PACKETTYPE_SENDCMD:
            {
                recvPacket = returnToPacket<SendCmdPacket>(recvBuf);
            }
            break;  
            default:
                delete[] recvBuf;
                continue;
            break;
        }
        delete[] recvBuf;
        TaskMileStone milestone = cmdFilter->getMileStone(*recvPacket);

        //패킷이 들어갸야 할 방향 제시
        switch(milestone) {
            case TASKMILESTONE_DATABASE:
            break;
            case TASKMILESTONE_SETUSERS:
            break;
            case TASKMILESTONE_SYSTEM:
            {
                //quit : 시스템 종료
                string quit = System_Control::quit+"\n";
                if(recvPacket->getCmdArray()[0].compare(quit)) {
                    sendPacketQueue.lock()->push(new SignalPacket(user->getID(),
                                                        socket->getIP(),
                                                        recvPacket->getCmdNum(),
                                                        socket->getDiscripter(),
                                                        SIGNALTYPE_SHUTDOWN
                    ));

                    //*isDisConnected = true;
                }
            }
            break;
            default: //error noauth
            {
                //delete recvPacket;
                string errorMsg;

                if(milestone == TASKMILESTONE_ERR)
                    errorMsg = "Command Not Found";
                else
                    errorMsg = "Authority denied";

                //수신을 시작하는 패킷 삽입
                sendPacketQueue.lock()->push( new SignalPacket(user->getID(),
                                        socket->getIP(),
                                        recvPacket->getCmdNum(),
                                        socket->getDiscripter(),
                                        SIGNALTYPE_RECVSTART   
                ));

                //에러메세지를 보내는 패킷 삽입
                sendPacketQueue.lock()->push( new RecvMsgPacket(user->getID(), 
                                         socket->getIP(),
                                         recvPacket->getCmdNum(),
                                         socket->getDiscripter(),
                                         errorMsg
                ));
                //시그널 패킷 삽입
                sendPacketQueue.lock()->push( new SignalPacket(user->getID(),
                                        socket->getIP(),
                                        recvPacket->getCmdNum(),
                                        socket->getDiscripter(),
                                        SIGNALTYPE_RECVEND   
                ));
            }
            break;
        }

    }
}