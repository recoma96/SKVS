#include <queue>
#include <deque>
#include <mutex>

#include "../lib/packet/Packet.hpp"
#include "../lib/structure/TypePrinter.hpp"

using namespace std;
using namespace structure;

extern bool removeSerialNum(vector<int>& serialList, int removeNum);
extern bool isShutdown;
//Cmd가 받는 데이터 순서
//SIGNAL-RECVSTART -> RECV-STRING OR DATA (시스템에러 발생 시 SIGNAL) -> SIGNAL-RECV-END

void CmdThread(int cmdNum, 
                queue<Packet*, deque<Packet*>>* packetQueue,
                vector<int>* cmdSerialList, 
                string cmdInterface, 
                mutex* packetQueueMutex) {
    //cmdInterface는 사용자 입장에서 멀티스레드형식으로 수행될 경우
    //입력 인터페이스가 안보이므로 cmdThread가 명령수행이 끝나면 cmdIterface를 출력합니다.
    while(!isShutdown) {
        if(!packetQueue->empty()) {
            //cmd번호 확인
            
            if(packetQueue->front()->getCmdNum() == cmdNum) {

                //패킷 큐에서 패킷 받기
                packetQueueMutex->lock();

                Packet* recvPacket = packetQueue->front();

                packetQueue->pop();
                packetQueueMutex->unlock();

                //패킷 타입 검색
                switch(recvPacket->getPacketType()) {

                    //데이터 수신
                    case PACKETTYPE_RECV:
                    {
                        RecvPacket* recvPacketAboutData = (RecvPacket*)recvPacket;
                        if( recvPacketAboutData->getRecvPacketType() == RECVPACKETTYPE_DATA ) {
                            //프린트 양식 : key \t\t structtype \t\t datatype \n

                            RecvDataPacket* printDataPacket = (RecvDataPacket*)recvPacketAboutData;
                            cout << printDataPacket->getData().getDataToString() << "\t\t" 
                                << convertDataTypeToString(printDataPacket->getData().getDataType()) << "\t\t"
                                << convertStructTypeToString(printDataPacket->getData().getStructType()) << endl;

                            delete printDataPacket;
                            
                        } else {
                            //프린트 양식 : 걍 다 필요없고 그냥 싹다 프린팅
                            RecvMsgPacket* printDataPacket = (RecvMsgPacket*)recvPacketAboutData;
                            cout << printDataPacket->getMsg() << endl;

                            delete printDataPacket;
                        }
                    }
                    break;
                    case PACKETTYPE_SIGNAL:
                    {
                        SignalPacket* signalPacket = (SignalPacket*)recvPacket;

                        switch( signalPacket->getSignal()) {

                            case SIGNALTYPE_SHUTDOWN: //종료
                                isShutdown = true;
                                continue;
                            break;
                            case SIGNALTYPE_RECVSTART: //수신 시작
                                delete signalPacket;
                                cout << endl;
                            break;
                            case SIGNALTYPE_RECVEND: //수신 종료
                                delete signalPacket;
                                removeSerialNum(*cmdSerialList, cmdNum);
                                return;
                            break;

                            case SIGNALTYPE_ERROR: //시스템 에러
                                //근데 얘는 클라이언트로 올라오면 안됨
                                cerr << "System Error : Error Signal must not go to client" << endl;
                                delete signalPacket;
                            break;
                        }
                    }
                    break;

                    default:
                    break;
                }
            }
        }
    }


    removeSerialNum(*cmdSerialList, cmdNum);

}