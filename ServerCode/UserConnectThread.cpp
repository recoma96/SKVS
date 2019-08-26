#include "../lib/packet/Packet.hpp"
#include "../lib/logbase/LogBase.hpp"
#include "../lib/SockWrapper/ServerSocketManager.hpp"
#include "../lib/SockWrapper/NetworkingManager.hpp"
#include "../lib/SockWrapper/SocketManager.hpp"
#include "../lib/user/LoginedUserList.hpp"
#include "../lib/CommandFilter.hpp"
#include "../lib/threadAdapter/AdapterThreadUtility.hpp"
#include "../lib/threadAdapter/AdapterThreadBridge.hpp"


#include <string>
#include <list>
#include <map>
#include <deque>
#include <queue>

#include <iostream>
#include <thread>

using namespace std;
using namespace SockWrapperForCplusplus;

extern bool shutdownSignal;

extern unsigned int LogAdapterSerial_input;


extern void IOThread(UserList* userList, 
					 LoginedUserList* loginedUserList, 
					 Socket* sock,
              		 CommandFilter* cmdFilter, 
					 map< int, weak_ptr<queue<Packet*, deque<Packet*>>> >* packetBridge, 
					 mutex* bridgeMutex,
					 shared_ptr<ThreadAdapter::AdapterThreadBridge> _adpaterBridgeQueue 
);


void UserConnectThread(
    Socket* mainSocket,
    UserList* userList,
    LoginedUserList* loginedUserList,
    CommandFilter* cmdFilter,
    map< int, weak_ptr<queue<Packet*, deque<Packet*>>> >* packetBridge,
    mutex* bridgeMutex,
    shared_ptr<ThreadAdapter::AdapterThreadBridge> adapterBridgeQueue


) {

    LogPacket* logPacket = nullptr;

    while(!shutdownSignal) {
        
		Socket* clientSocket = new Socket(); //IO쓰레드에서 처리하므로 pointer 처리
		listenClient(mainSocket, 1000);
		if(!acceptClient(mainSocket, clientSocket)) {

			logPacket = new LogPacket("Server", "Server", 0, 0, "Accept Error");
			adapterBridgeQueue->pushInQueue(logPacket, LogAdapterSerial_input);
			cerr << logPacket->getStatement() << endl;

			continue;
		}

		//로그 작성 : 외부 IP로부터 접근 시도

		
		
        int setTrue = 1;
		setSocketOption(clientSocket, SOL_SOCKET, SO_REUSEADDR, 
			(void*)&setTrue, sizeof(int));
		
		//ip, port 입력
		clientSocket->clientUpdate(mainSocket->getPort());

		logPacket = new LogPacket(
		"Unknown", clientSocket->getIP(),0,0,"Unknown User is tryies to access"
		);
		adapterBridgeQueue->pushInQueue(
			logPacket,
			LogAdapterSerial_input
		);
		cout << logPacket->getStatement() << endl;

		//IO Thread 생성
		thread iothread = thread(IOThread,
								 userList,
								 loginedUserList,
								 clientSocket,
								 cmdFilter,
								 packetBridge,
								 bridgeMutex,
								 adapterBridgeQueue);
		iothread.detach();

	}

}