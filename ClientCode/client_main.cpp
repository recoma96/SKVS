#include <string>
#include <list>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

#include "../lib/loader/ClientLoader.hpp"
#include "../lib/SockWrapper/ClientSocketManager.hpp"
#include "../lib/SockWrapper/NetworkingManager.hpp"
#include "../lib/user/User.hpp"

#include "../lib/Exception.hpp"

using namespace SockWrapperForCplusplus;
using namespace std;

extern void IOThread(User* userInfo, Socket* socket);


bool isShutdown = false; //shotdown flag
int main(void) {

	cout << "SKVS[CLIENT] 0.1.1 (Relesae August 26 2019 )" << endl;
	cout << "Alpha Test Version 1" << endl;
	cout << "Writen by SweetCase Project" << endl;
	cout << endl;

	//1.login.json에서 로그인 정보 파싱
	ClientLoader* clientLoader = nullptr;

	try {
		clientLoader = new ClientLoader();
	} catch(FileException e) {
		cerr << e.getErrorMsg() << endl;
		return 0;
	}

	//서버와의 연결 시도
	Socket socket(clientLoader->getConnectIP(),
				clientLoader->getConnectPort() , false);
	
	if(!setSocket(&socket)) {
		cerr << "Socket Error" << endl;
		return 0;
	}
	
	
	if( !connectToServer(&socket)) {
		cerr << "Failed to Connect Server " << clientLoader->getConnectIP() << endl;
		closeSocket(&socket);
		return 0;
	}
	

	//서버에게 로그인 정보 전송
	//ID-pswd
	
	
	string sendLoginData = clientLoader->getID() + "-" + clientLoader->getPaswd();
	
	if( sendData(&socket, (char*)(sendLoginData.c_str()), sendLoginData.length()) <= 0) {
		cerr << "Failed to Send Login Data To Server " << clientLoader->getConnectIP() << endl;
		closeSocket(&socket);
		return 0;
	}

	//로그인 허용 어부
	bool checkConnect = false;
	if( recvData(&socket, &checkConnect, sizeof(bool)) <= 0) {
		cerr << "Failed to recv data from server " << clientLoader->getConnectIP() << endl;
		closeSocket(&socket);
		return 0;
	}

	if(checkConnect == false) {
		cerr << "Login Denied" << endl;
		return 0;
	}

	//유저 레벨 부여
	UserLevel userLv;
	if( recvData(&socket, &userLv, sizeof(UserLevel)) <= 0) {
		cerr << "Failed to recv data from server " << clientLoader->getConnectIP() << endl;
		closeSocket(&socket);
		return 0;
	}

	//User객체 생성
	User myUser(clientLoader->getID(), clientLoader->getPaswd(),userLv);

	//얘 이제 필요없음
	delete clientLoader;
	cout << "Complete" << endl; 

	//IO Thread 생성
	thread iothread = thread(IOThread, &myUser, &socket);

	//while문
	while(!isShutdown) {
		this_thread::sleep_for(chrono::microseconds(10));
	}

	//종료 시그널이 송출됬을 경우
	//iothread가 종료될 때까지 기다림
	iothread.join();
	cout << "SKVS out" << endl;
	closeSocket(&socket);
	return 0;
}
