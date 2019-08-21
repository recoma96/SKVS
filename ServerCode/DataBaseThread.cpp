#include "../lib/threadAdapter/AdapterThreadUtility.hpp"
#include "../lib/logbase/LogBase.hpp"
#include "../lib/database/DataBase.hpp"
#include "../lib/packet/Packet.hpp"

#include <thread>
#include <chrono>
#include <string>

using namespace std;
using namespace ThreadAdapter;
extern bool shutdownSignal;

//DataBase Cmd Thread
void DataBaseCmdThread(
    SendCmdPacket* requestPacket,
    SKVS_DataBase::DataBase* DB
);

void DataBaseThread(shared_ptr<AdapterThreadUtility> _pipe) {

    weak_ptr<AdapterThreadUtility> pipe = _pipe;
    Packet* recvPacket = nullptr;

    //데이터베이스
    SKVS_DataBase::DataBase dataBase(_pipe);

    //패킷이 들어와있는지에 대한 검색
    while(!shutdownSignal) {
        recvPacket = pipe.lock()->popInInputQueue();

        //패킷 검색됨
        if( recvPacket != nullptr) {
            
            if(recvPacket->getPacketType() == PACKETTYPE_SENDCMD) {
                
                thread cmdThread(
                    DataBaseCmdThread,
                    (SendCmdPacket*)recvPacket,
                    &dataBase
                );
                //cmdThread 실행
            }
            else { //SemdCmd가 아닌 다른 패킷이 들어오는 경우 -> 시스템 에러
                string errorMsg = "This Packet that is not SendCmdPacket is try to access DataBase Thread";
                //log 패킷을 생성해서 오류보고 후 패킷 소멸
                LogPacket* logPacket = new LogPacket(recvPacket->getUserName(),
                                                recvPacket->getIP(),
                                                0,
                                                0,
                                                errorMsg
                );
                cout << logPacket->getStatement() << endl;
                pipe.lock()->pushInOutputQueue(logPacket);
                delete recvPacket;
            }

            recvPacket = nullptr;
        }


        this_thread::sleep_for(chrono::milliseconds(1));
    }
}
