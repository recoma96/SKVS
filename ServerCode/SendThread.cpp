#include "../lib/SockWrapper/ServerSocketManager.hpp"
#include "../lib/SockWrapper/NetworkingManager.hpp"
#include "../lib/SockWrapper/SocketManager.hpp"
#include "../lib/packet/SerialController.hpp"
#include "../lib/packet/Packet.hpp"
#include "../lib/user/LoginedUser.hpp"
#include "../lib/threadAdapter/AdapterThreadBridge.hpp"


#include <string>
#include <deque>
#include <queue>
#include <mutex>
#include <vector>
#include <chrono>
#include <thread>
#include <string.h>

extern bool shutdownSignal;
extern unsigned int LogAdapterSerial_input;

using namespace std;
using namespace SockWrapperForCplusplus;
using namespace PacketSerialData;
using namespace google;

void SendThread(Socket* socket,
                LoginedUser* user,
                shared_ptr<queue<Packet*, deque<Packet*>>> _sendPacketQueue,
                weak_ptr<ThreadAdapter::AdapterThreadBridge> _adapterBridgeQueue,
                bool* isDisConnected ) {
    
    weak_ptr<queue<Packet*, deque<Packet*>>> sendPacketQueue = _sendPacketQueue;
    weak_ptr<ThreadAdapter::AdapterThreadBridge> adapterBridgeQueue = _adapterBridgeQueue;
    LogPacket* logPacket = nullptr;

    bool disconnectedBuf = false;

    while(!(*isDisConnected) && !shutdownSignal) {
        
        if(!(sendPacketQueue.lock()->empty())) {

            //패킷 수집
            Packet* sendPacket = sendPacketQueue.lock()->front();
            sendPacketQueue.lock()->pop();

            //패킷타입 수집
            PacketType packetType = sendPacket->getPacketType();

            int bufSize = 0;
            char* sendBuf = nullptr;

            switch(packetType) {
                case PACKETTYPE_RECV:
                {
                    RecvPacket* recvPacket = (RecvPacket*)sendPacket;
                    if( recvPacket->getRecvPacketType() == RECVPACKETTYPE_DATA ) {
                        sendBuf = makePacketToCharArray<RecvDataPacket>( *((RecvDataPacket*)(recvPacket)) );
                        bufSize = strlen(sendBuf);
                        delete recvPacket;
                    } else {
                        sendBuf = makePacketToCharArray<RecvMsgPacket>( *((RecvMsgPacket*)(recvPacket)) );
                        bufSize = strlen(sendBuf);
                        delete recvPacket;
                    } 
                }
                break;
                case PACKETTYPE_SIGNAL:
                {
                    sendBuf = makePacketToCharArray<SignalPacket>( *((SignalPacket*)(sendPacket)) );
                    bufSize = strlen(sendBuf);
                    if( ((SignalPacket*)(sendPacket))->getSignal() == SIGNALTYPE_SHUTDOWN ) {
                        
                        disconnectedBuf = true;
                    }

                    delete sendPacket;
                }
                break;
                
                default:
                    cout << "System Error : log or send can't put on sendThread" << endl;
                    delete sendPacket;
                    continue;
                break;

            }

            //패킷 전송

            //1.패킷 사이즈 송신
            
            if(sendData(socket, &bufSize, sizeof(int)) <= 0) {
                    
                delete sendBuf;
                *isDisConnected = true;

                logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, "Server DisConnected");
                cerr << logPacket->getStatement() << endl;
                adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);
                continue;
            }

            //데이터 타입 송신
            if(sendData(socket, &packetType, sizeof(PacketType)) <= 0) {
                    
                delete sendBuf;
                *isDisConnected = true;

                logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, "Server DisConnected");
                cerr << logPacket->getStatement() << endl;
                adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);
                continue;
            }

            //데이터 송신
            if(sendData(socket, sendBuf, bufSize) <= 0) {
                    
                delete sendBuf;
                *isDisConnected = true;

                logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, "Server DisConnected");
                cerr << logPacket->getStatement() << endl;
                adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);
                continue;
            }
            delete sendBuf;


            if(disconnectedBuf == true)
                (*isDisConnected) = true;

        }

        this_thread::sleep_for(chrono::milliseconds(1));
    }

}