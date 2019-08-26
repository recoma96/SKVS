#include "../lib/packet/Packet.hpp"
#include "../lib/logbase/LogBase.hpp"
#include "../lib/loader/SystemLoader.hpp"
#include "../lib/loader/AccountLoader.hpp"
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
#include <mutex>
#include <thread>
#include <chrono>

using namespace std;
using namespace SockWrapperForCplusplus;

bool shutdownSignal; //종료 시그널
string logRoot; //로그 저장 위치

//사용자 정보 작성
mutex writeUserInfoMutex;

//System, AccountLoader
SystemLoader* systemLoader = nullptr;
AccountLoader* accountLoader = nullptr;

//어댑터 브릿지 패킷 일련변호
unsigned int LogAdapterSerial_input = 0;
unsigned int DBAdapterSerial_input = 1;

//사용자 로그인을 중계하는 스레드
void UserConnectThread(
    Socket* mainSocket,
    UserList* userList,
    LoginedUserList* loginedUserList,
    CommandFilter* cmdFilter,
    map< int, weak_ptr<queue<Packet*, deque<Packet*>>> >* packetBridge,
    mutex* bridgeMutex,
    shared_ptr<ThreadAdapter::AdapterThreadBridge> adapterBridgeQueue


);

//로그스레드를 연결할 어뎁터 스레드 (Standalone)
void StandaloneAdapterThreadToLog(shared_ptr<ThreadAdapter::AdapterThreadBridge> _adpaterBridgeQueue);

//데이터베이스를 연결할 어댑터 스레드 (Standalone)
extern void StandAloneAdapterThreadToDataBase(
    shared_ptr<ThreadAdapter::AdapterThreadBridge> _adpaterBridgeQueue,
    map< int, weak_ptr<queue<Packet*, deque<Packet*>>> >* packetBridge
);

int main(void) {

	cout << "SKVS[SERVER] 0.1.1 (Relesae August 26 2019 )" << endl;
	cout << "Alpha Test Version 1" << endl;
	cout << "Writen by SweetCase Project" << endl;

	cout << endl;
	shutdownSignal = false;
	LogPacket* logPacket = nullptr; //로그패킷 제작 틀

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
	map< int, weak_ptr<queue<Packet*, deque<Packet*>>> > packetBridge;
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

	//어댑터 브릿지 패킷 큐 생성
	shared_ptr<ThreadAdapter::AdapterThreadBridge> adapterBridgeQueue = 
		make_shared<ThreadAdapter::AdapterThreadBridge>();

	thread logAdapterThread;
	thread dataBaseAdapterThread;

	//로그 위치 구하기
	logRoot = systemLoader->getLogRoot();

	//TODO공통적으로 사용하는 쓰레드 생성

	if(systemType == SYSTEMTYPE_DISTRIBUTED) {
		//TODO차후에 생성 예정
	} else if (systemType == SYSTEMTYPE_STANDALONE) {
		
		//어댑터 브랫지 패킷 큐 생성
		//0번 : log input : 다른 스레드로부터 로그패킷 수집
		//1번 : DB-input : 클라이언트로부터 명령패킷 수집

		adapterBridgeQueue->insertQueue();
		adapterBridgeQueue->insertQueue();

		//Database, LogStorage Thread 생성
		logAdapterThread = thread(StandaloneAdapterThreadToLog,
									adapterBridgeQueue);
		dataBaseAdapterThread = thread(
			StandAloneAdapterThreadToDataBase,
			adapterBridgeQueue,
			&packetBridge
		);

	}

	logPacket = new LogPacket("Server", "Server", 0, 0, "System Setting Complelete");
	cout << logPacket->getStatement() << endl;
	adapterBridgeQueue->pushInQueue(logPacket, LogAdapterSerial_input);
	

	//로그인 중계하는 스레드 생성
	
	//클라이언트 연결 요청 대기
	thread userConnectThread(
		UserConnectThread,
		&mainSocket,
		userList,
		&loginedUserList,
		&cmdFilter,
		&packetBridge,
		&bridgeMutex,
		adapterBridgeQueue

	);
	userConnectThread.detach();
	
	while(!shutdownSignal) {

		this_thread::sleep_for(chrono::milliseconds(1));

	}
	

	logPacket = new LogPacket("Server", "Server", 0, 0, "System Shutdown");
	cout << logPacket->getStatement() << endl;
	adapterBridgeQueue->pushInQueue(logPacket, LogAdapterSerial_input);

	logAdapterThread.join();
	dataBaseAdapterThread.join();
	closeSocket(&mainSocket);
	

	return 0;
}