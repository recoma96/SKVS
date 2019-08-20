#include <string>
#include <list>
#include <iostream>
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

#include "../lib/Exception.hpp"

using namespace SockWrapperForCplusplus;
using namespace std;

//명령스레드 시리얼 번호 계산
inline int setCmdSerial(vector<int>& serialList) {

	int counter = 0;

	if( serialList.empty() ) {

		serialList.push_back(0);
		return 0;
	}
	for( vector<int>::iterator iter = serialList.begin();
			iter != serialList.end(); iter++ ) {

		if( counter != (*iter) ) {
			return counter;
		}
		counter++;
	}
	return counter;
}

//시리얼 번호 제거
inline bool removeSerialNum(vector<int>& serialList, int removeNum) {

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

extern bool isShutdown;


//IO Thread
void IOThread(User* userInfo, Socket* socket) {
    
    string cmd; //명령어 입력하는 부분
    queue<Packet*, deque<Packet*>> packetQueue; //패킷 큐
    vector<int> cmdSerialList; //cmd시리얼 리스트

    string cmdInterFace; cmdInterFace.clear();
    cmdInterFace += userInfo->getID();

    if( userInfo->getUserLevel() == USERLEVEL_ROOT)
        cmdInterFace += " #> ";
    else if(userInfo->getUserLevel() == USERLEVEL_ADMIN)
        cmdInterFace += " %> ";
    else
        cmdInterFace += " > ";

    while(!isShutdown) {
        cout << cmdInterFace;
        getline(cin, cmd, '\n');
    }

}