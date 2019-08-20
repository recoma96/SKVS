#include <string>
#include <list>
#include <thread>
#include <mutex>
#include <chrono>
#include <queue>
#include <deque>
#include <vector>

#include "../lib/SockWrapper/ClientSocketManager.hpp"
#include "../lib/SockWrapper/NetworkingManager.hpp"
#include "../lib/SockWrapper/SocketManager.hpp"
#include "../lib/user/User.hpp"
#include "../lib/packet/Packet.hpp"
#include "../lib/packet/SerialController.hpp"

#include "../lib/Exception.hpp"

extern bool isShutdown;

using namespace std;
using namespace SockWrapperForCplusplus;
using namespace PacketSerialData;

//서버로부터 패킷을 받고 패킷 큐에다 패킷을 전달하는 스레드입니다.
void RecvThread(Socket* socket, queue<Packet*, deque<Packet*>>* packetQueue) {

    //데이터받는 순서 -> size -> serialize Packet;
    while(!isShutdown) {

        int recvBufSize = 0;
        char* recvBuf = nullptr;

        //데이터받기
        if( recvData(socket, &recvBufSize, sizeof(int)) <= 0) {
            cout << "Server Disconnected" << endl;
            isShutdown = true;
            continue;
        }

        recvBuf = new char[recvBufSize];

        if( recvData(socket, recvBuf, recvBufSize) <= 0) {
            cout << "Server Disconnected" << endl;
            isShutdown = true;
            continue;
        }

        //데이터 역직렬화
        Packet* savePacket = nullptr;
        

        //SendCmd나 log가 들어오면 안됨
        switch( whatIsPacketTypeInSerializedStr(recvBuf)) {

            case PACKETTYPE_RECV:
            {
                RecvPacketType checkType = whatIsRecvPacketTypeInRecvDataSerial(recvBuf);
                if( checkType == RECVPACKETTYPE_DATA)
                    savePacket = returnToPacket<RecvDataPacket>(recvBuf);
                else
                    savePacket = returnToPacket<RecvMsgPacket>(recvBuf);
            }
            break;

            case PACKETTYPE_SIGNAL:
            {
                savePacket = returnToPacket<SignalPacket>(recvBuf);
            }
            break;

            case PACKETTYPE_SENDCMD:
                cout << "System error : SendCmdPacket Leak" << endl;
            break;
            case PACKETTYPE_LOG:
                cout << "System error : log Data Leak" << endl;
            break;
        }
        //패킷 큐에 삽입
        packetQueue->push(savePacket);
    }


}