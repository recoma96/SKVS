#include <string>
#include <list>
#include <iostream>
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
#include "../lib/Tokenizer.hpp"

using namespace SockWrapperForCplusplus;
using namespace std;
using namespace SkvsProtocol;

//명령스레드 시리얼 번호 계산
inline int setCmdSerial(vector<int>& serialList) {

	int counter = 0;

	if( serialList.empty() ) {

		return 0;
	}
	for( vector<int>::iterator iter = serialList.begin();
			iter != serialList.end(); iter++ ) {

		if( counter <= (*iter))
            counter = (*iter)+1;
	}
	return counter;
}

//시리얼 번호 제거
bool removeSerialNum(vector<int>& serialList, int removeNum) {

	for( vector<int>::iterator deleteCursor = serialList.begin();
			deleteCursor != serialList.end();
			deleteCursor++ ) {

		if( (*deleteCursor) == removeNum ) {

			serialList.erase(deleteCursor);
			return true;
		}
	}
	return false;
}

mutex printMutex;
//멀티스레드 기반 프로그램이므로
//동시에 결과가 끝나는 것을 대비해
//상호배제를 사용하여 결과 출력을 순차적으로 합니다.

extern bool isShutdown;
extern void RecvThread(Socket* socket, queue<Packet*, deque<Packet*>>* packetQueue);
extern void CmdThread(int cmdNum, 
                        queue<Packet*, deque<Packet*>>* packetQueue, 
                        vector<int>* cmdSerialList, 
                        string cmdInterface, 
                        mutex* packetQueueMutex);

//인터페이스
string cmdInterFace;

//IO Thread
void IOThread(User* userInfo, Socket* socket) {
    
    string cmd; //명령어 입력하는 부분
    queue<Packet*, deque<Packet*>> packetQueue; //패킷 큐
    mutex packetQueueMutex; //패킷 큐 mutex

    vector<int> cmdSerialList; //cmd시리얼 리스트
    mutex cmdSerialMutex; //cmd시리얼 번호를 생성/삭제에 있어서 사용하는 mutex

    cmdInterFace.clear();
    cmdInterFace += userInfo->getID();

    if( userInfo->getUserLevel() == USERLEVEL_ROOT)
        cmdInterFace += " #> ";
    else
        cmdInterFace += " $> ";

    //RecvThread 생성
    thread recvThread = thread(RecvThread, socket, &packetQueue );
    recvThread.detach();

    while(!isShutdown) {

        int cmdSerial = 0; //시리얼 번호 할당 변수
        int packetSize = 0; //서버에게 보낼 패킷 사이즈

        cout << cmdInterFace;
        getline(cin, cmd, '\n');

        if(isShutdown) return; //서버측에서 종료요청을 받았을 경우

        if( cmd.length() == 0) continue;
        //탭키 사용했는지 확인
        vector<string> cmdVec = tok::tokenizer(cmd);

        if(cmdVec.empty()) continue;

        //시리얼 넘버 계산
        cmdSerialMutex.lock();
        cmdSerial = setCmdSerial(cmdSerialList);
        cmdSerialList.push_back(cmdSerial);
        cmdSerialMutex.unlock();

        //명령 스레드를 생성합니다.
        thread cmdThread = thread(CmdThread,
                                    cmdSerial, 
                                    &packetQueue, 
                                    &cmdSerialList, 
                                    cmdInterFace, 
                                    &packetQueueMutex);
        cmdThread.detach();

        //서버에 데이터를 보냅니다.
        
        //길이 -> 패킷 직렬화 -> 직렬화된 패킷 전송
        SendCmdPacket sendPacket(userInfo->getID(), socket->getIP(), cmdSerial, 0, cmd);
        char* sendStr = makePacketSerial(&sendPacket);
        int sendStrSize = strlen(sendStr);
        
        //패킷 전송 (데이터 길이, 데이터 타입, 데이터)
        if( sendData(socket, &sendStrSize, sizeof(int)) <= 0) {
            cerr << "Server Disconnected" << endl;
            isShutdown = true;
            continue;
        }

        PacketType sendType = sendPacket.getPacketType();
        if( sendData(socket, &sendType, sizeof(PacketType)) <= 0) {
            cerr << "Server Disconnected" << endl;
            isShutdown = true;
            continue;
        }

        if( sendData(socket, sendStr, sendStrSize) <= 0) {
            cerr << "Server Disconnected" << endl;
            isShutdown = true;
            continue;
        }
        delete sendStr;

    }

}
