#include <string>
#include <list>
#include <iostream>
#include <thread>
#include <mutex>

#include "../lib/loader/ClientLoader.hpp"
#include "../lib/SockWrapper/ClientSocketManager.hpp"
#include "../lib/SockWrapper/NetworkingManager.hpp"
#include "../lib/user/User.hpp"

#include "../lib/Exception.hpp"

using namespace SockWrapperForCplusplus;
using namespace std;


int main(void) {

	cout << "================== SDKVS[CLIENT] 0.1.0 Alpha ==================" << endl;
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

	//쓰레드 생성
	
	

	closeSocket(&socket);
	return 0;
}
