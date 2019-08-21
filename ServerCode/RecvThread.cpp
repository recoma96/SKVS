#include "../lib/packet/Packet.hpp"
#include "../lib/user/LoginedUser.hpp"
#include "../lib/CommandFilter.hpp"
#include "../lib/SockWrapper/ServerSocketManager.hpp"
#include "../lib/SockWrapper/NetworkingManager.hpp"
#include "../lib/SockWrapper/SocketManager.hpp"
#include "../lib/threadAdapter/AdapterThreadBridge.hpp"
#include "../lib/packet/SerialController.hpp"

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
    
    weak_ptr<ThreadAdapter::AdapterThreadBridge> adapterBridgeQueue = _adapterBridgeQueue;
    weak_ptr<queue<Packet*, deque<Packet*>>> sendPacketQueue = _sendPacketQueue;

    while(!shutdownSignal && !(*isDisConnected)) {

        int recvBufSize = 0;
        char* recvBuf = nullptr;
        LogPacket* logPacket = nullptr;

        //데이터받기
        if( recvData(socket, &recvBufSize, sizeof(int)) <= 0) {
            
            logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, "Server DisConnected");
            cerr << logPacket->getStatement() << endl;
            adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);

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
            break;
            default: //error noauth
            {
                //delete recvPacket;
                string errorMsg;

                if(milestone == TASKMILESTONE_ERR)
                    errorMsg = "Command Not Found";
                else
                    errorMsg = "Authority denied";

                //패킷 작성
                RecvMsgPacket sendPacket(user->getID(), 
                                         socket->getIP(),
                                         recvPacket->getCmdNum(),
                                         socket->getDiscripter(),
                                         errorMsg
                );

                //시그널 패킷 작성
                SignalPacket sigPacket(user->getID(),
                                       socket->getIP(),
                                       recvPacket->getCmdNum(),
                                       socket->getDiscripter(),
                                       SIGNALTYPE_RECVEND
                );

                int sendBufSize = 0; //직렬화된 데이터 사이즈
                char* sendBuf = nullptr; //직렬화할때 사용

                sendBuf = makePacketToCharArray<RecvMsgPacket>(sendPacket);
                sendBufSize = strlen(sendBuf);
               
                //메시지 패킷 송신
                if(sendData(socket, &sendBufSize, sizeof(int)) <= 0) {
                    
                    delete sendBuf;
                    *isDisConnected = true;

                    logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, "Server DisConnected");
                    cerr << logPacket->getStatement() << endl;
                    adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);
                    continue;
                }

                //데이터타입 송신
                PacketType sendType = sendPacket.getPacketType();
                if(sendData(socket, &sendType, sizeof(PacketType)) <= 0) {
                    
                    delete sendBuf;
                    *isDisConnected = true;

                    logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, "Server DisConnected");
                    cerr << logPacket->getStatement() << endl;
                    adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);
                    continue;
                }

                //데이터 송신
                if(sendData(socket, sendBuf, sendBufSize) <= 0) {
                    
                    delete sendBuf;
                    *isDisConnected = true;

                    logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, "Server DisConnected");
                    cerr << logPacket->getStatement() << endl;
                    adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);
                    continue;
                }

                delete sendBuf;

                //시그널 패킷 송신
                sendBuf = makePacketToCharArray<SignalPacket>(sigPacket);
                sendBufSize = strlen(sendBuf);

                //메시지 패킷 송신
                if(sendData(socket, &sendBufSize, sizeof(int)) <= 0) {
                    
                    delete sendBuf;
                    *isDisConnected = true;

                    logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, "Server DisConnected");
                    cerr << logPacket->getStatement() << endl;
                    adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);
                    continue;
                }

                //데이터타입 송신
                sendType = sigPacket.getPacketType();
                if(sendData(socket, &sendType, sizeof(PacketType)) <= 0) {
                    
                    delete sendBuf;
                    *isDisConnected = true;

                    logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, "Server DisConnected");
                    cerr << logPacket->getStatement() << endl;
                    adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);
                    continue;
                }

                if(sendData(socket, sendBuf, sendBufSize) <= 0) {
                    
                    delete sendBuf;
                    *isDisConnected = true;

                    logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, "Server DisConnected");
                    cerr << logPacket->getStatement() << endl;
                    adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);
                    continue;
                }
                delete sendBuf;
            }
            break;
        }

    }
}