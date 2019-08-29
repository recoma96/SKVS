#include <string>
#include <list>
#include <thread>
#include <mutex>
#include <chrono>
#include <queue>
#include <deque>
#include <vector>
#include <string.h>

#include "../lib/SockWrapper/ClientSocketManager.hpp"
#include "../lib/SockWrapper/NetworkingManager.hpp"
#include "../lib/SockWrapper/SocketManager.hpp"
#include "../lib/user/User.hpp"
#include "../lib/packet/Packet.hpp"
#include "../lib/packet/SkvsProtocol.hpp"

#include "../lib/Exception.hpp"

extern bool isShutdown;

using namespace std;
using namespace SockWrapperForCplusplus;
using namespace SkvsProtocol;

//서버로부터 패킷을 받고 패킷 큐에다 패킷을 전달하는 스레드입니다.
void RecvThread(Socket* socket, queue<Packet*, deque<Packet*>>* packetQueue) {

    //데이터받는 순서 -> size -> serialize Packet;
    while(!isShutdown) {

        int recvBufSize = 0;
        char* recvBuf = nullptr;
        
       

        //데이터받기
        while(true) {
            if( recvData(socket, &recvBufSize, sizeof(int), MSG_DONTWAIT) > 0) {
                break;
            } else {
                if(isShutdown) {
                    cout << endl;
                    cout << "Server Disconnected" << endl;
                    cout << "Press Enter Key" << endl;
                    return;
                }
                this_thread::sleep_for(chrono::milliseconds(1));
            }
        }
        
        PacketType recvType;
        if( recvData(socket, &recvType, sizeof(PacketType)) <= 0) {
            cout << "Server Disconnected" << endl;
            isShutdown = true;
            continue;
        }

        recvBuf = new char[recvBufSize+1];
        //recvBuf = {0};

        if( recvData(socket, recvBuf, recvBufSize+1) <= 0) {

            cout << "Server Disconnected" << endl;
            isShutdown = true;
            continue;
        }
        //널 초기화
        recvBuf[recvBufSize] = '\0';
        
        
        Packet* savePacket = nullptr;
        //SendCmd나 log가 들어오면 안됨
        switch( recvType ) {

            case PACKETTYPE_RECV:
            {
                RecvPacketType checkType;
                try {

                    checkType = checkRecvPacketType(recvBuf);
                } catch (Exception& e) {
                    //데이터 파괴
                    delete recvBuf;
                    cout << e.getErrorMsg() << endl;
                    continue;
                }
                if( checkType == RECVPACKETTYPE_DATA)
                    savePacket = returnToPacket<RecvDataPacket>(recvBuf);
                else {
                    savePacket = returnToPacket<RecvMsgPacket>(recvBuf);
                    
                }
            }
            break;

            case PACKETTYPE_SIGNAL:
            {
                savePacket = returnToPacket<SignalPacket>(recvBuf);
            }
            break;

            case PACKETTYPE_SENDCMD:
            {
                SendCmdPacket* sendPacket = returnToPacket<SendCmdPacket>(recvBuf);
                cout << "System error : SendCmdPacket Leak" << endl;
                delete[] recvBuf;
                continue;
            }
            break;
            case PACKETTYPE_LOG:
                cout << "System error : log Data Leak" << endl;
                delete[] recvBuf;
                continue;
            break;
        }
        //패킷 큐에 삽입

        if(savePacket == nullptr) {
            //패킷 파괴
            delete[] recvBuf;
            continue;
        }
        packetQueue->push(savePacket);
        
        
        delete[] recvBuf;

    }


}