#include "../lib/packet/Packet.hpp"
#include "../lib/logbase/LogBase.hpp"
#include "../lib/loader/SystemLoader.hpp"
#include "../lib/loader/AccountLoader.hpp"
#include "../lib/SockWrapper/ServerSocketManager.hpp"
#include "../lib/SockWrapper/NetworkingManager.hpp"
#include "../lib/SockWrapper/SocketManager.hpp"
#include "../lib/user/LoginedUserList.hpp"
#include "../lib/CommandFilter.hpp"

#include "../lib/database/DataBaseCmd.hpp"
#include <string>
#include <list>
#include <map>
#include <deque>
#include <queue>

#include <iostream>
#include <mutex>
#include <thread>

using namespace std;
using namespace SockWrapperForCplusplus;

bool shutdownSignal; //종료 시그널

extern void IOThread(UserList* userList, LoginedUserList* loginedUserList, Socket* sock,
              CommandFilter* cmdFilter, map<int, queue<Packet*, deque<Packet*>>>* packetBridge, mutex* bridgeMutex   );

int main(void) {

	cout << "================== SDKVS[SERVER] 0.1.0 Alpha ==================" << endl;
	cout << endl;
	shutdownSignal = false;
	SystemLoader* systemLoader = nullptr;
	AccountLoader* accountLoader = nullptr;

	//json파일로부터 데이터 파싱
	try {
		systemLoader = new SystemLoader();
		accountLoader = new AccountLoader();
	} catch(FileException e) {
		cerr << e.getErrorMsg() << endl;
		return 0;
	}

	//시스템 스타일 파싱
	SystemType systemType;

	try {
		systemType = systemLoader->getStyle();
	} catch(DataConvertException e) {
		cerr << e.getErrorMsg() << endl;
	}

	//상수화
	const short connectPort = systemLoader->getPort();
	//메인 소켓 생성
	Socket mainSocket("127.0.0.1", connectPort, true);
	if(!setSocket(&mainSocket)) {
		cerr << "Socket Error" << endl;
		return 0;
	}

	//REUSEADDR
	
	int setTrue = 1;
	setSocketOption(&mainSocket, SOL_SOCKET, SO_REUSEADDR,
					(void*)&setTrue, sizeof(int));
	
	//bind
	if(!bindSocket(&mainSocket)) {
		cerr << "bind error" << endl;
		closeSocket(&mainSocket);
		return 0;
	}

	//소켓-패킷큐 맵 생성
	map<int, queue<Packet*, deque<Packet*>>> packetBridge;
	mutex bridgeMutex; //패킷큐를 추가나 삭제할 때 사용

	//유저리스트 생성
	UserList* userList = nullptr;
	try {
		userList = accountLoader->makeUserList();
	} catch(UserLengthException e) {
		cerr << e.getErrorMsg() << endl;
		closeSocket(&mainSocket);
		return 0;
	}
	LoginedUserList loginedUserList;

	//커멘드필터 생성
	CommandFilter cmdFilter(userList);

	//TODO공통적으로 사용하는 쓰레드 생성

	if(systemType == SYSTEMTYPE_DISTRIBUTED) {
		//TODO차후에 생성 예정
	} else if (systemType == SYSTEMTYPE_STANDALONE) {
		//AdapterBridgeThread

		//Database, LogStorage Thread
	}

	
	cout << "System Setting Complelete" << endl;
	//클라이언트 연결 요청 대기
	while(true) {

		Socket* clientSocket = new Socket(); //IO쓰레드에서 처리하므로 pointer 처리
		listenClient(&mainSocket, 1000);
		if(!acceptClient(&mainSocket, clientSocket)) {
			cerr << "failed to connect" << endl;
			return 0;
			//continue;
		}
		
		setSocketOption(clientSocket, SOL_SOCKET, SO_REUSEADDR, 
			(void*)&setTrue, sizeof(int));
		
		//ip, port 입력
		clientSocket->clientUpdate(mainSocket.getPort());

		//IO Thread 생성
		thread iothread = thread(IOThread,
								 userList,
								 &loginedUserList,
								 clientSocket,
								 &cmdFilter,
								 &packetBridge,
								 &bridgeMutex);
		iothread.detach();
	
	}

	closeSocket(&mainSocket);

	return 0;
}