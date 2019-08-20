#include "Socket.hpp"

#include <iostream>
#include <string>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;
using namespace SockWrapperForCplusplus;

Socket::Socket(const string _IP, const short _port, const bool _isServer) noexcept {

	this->IP = _IP; this->port = _port;	
	this->sock = -1;
	
	//sockaddr_in
	this->sockItem = new sockaddr_in;
	
	this->sockItem->sin_family = AF_INET;
	this->sockItem->sin_port=htons(this->port);

	//Server는 이 프로그램 기준으로 모든 IP가 접근 가능하게 해야 하고
	//Client는 클래스 당 하나의 서버를 접근해야 힙니다.
	if( ( this->serverFlag = _isServer) == true)
		this->sockItem->sin_addr.s_addr = htonl(INADDR_ANY);
	else 
		this->sockItem->sin_addr.s_addr = inet_addr( this->IP.c_str() );

	socklen_t _sockSize = sizeof( (*sockItem) );
	this->socketSize = new socklen_t;

}

//Client로부터 정보를 받아오는 경우
Socket::Socket(void) noexcept {
	
	this->port = 0;
	this->sock = -1;

	this->sockItem = new sockaddr_in;
	this->socketSize = new socklen_t ;
	

}

Socket::~Socket(void) {

	if( this->sockItem != nullptr)
		delete socketSize;
	if( this->socketSize != nullptr)
		delete sockItem;

}

//get Function
string Socket::getIP(void) noexcept { return this->IP; }
short Socket::getPort(void) noexcept {  return this->port; }
int Socket::getDiscripter(void) noexcept { return this->sock; }
bool Socket::isServer(void) noexcept {  return this->serverFlag; }

sockaddr_in* Socket::getSocketItem(void) noexcept {

	return this->sockItem;

}
socklen_t* Socket::getSocketSizePointer(void) noexcept { return this->socketSize; }
Socket* Socket::getCopiedSocket(void) noexcept {

	Socket* copiedSocket = new Socket(this->IP, this->port, this->serverFlag);

	copiedSocket->setDiscripter(this->sock);

	return copiedSocket;
}
Socket& Socket::getSocket(void) { return *this; }

int Socket::setDiscripter(const int _sock) {
	this->sock = _sock;
	return _sock;
}

void Socket::clientUpdate(short _port) {

	string ipStr(inet_ntoa(this->sockItem->sin_addr));

	if( ipStr.compare("0.0.0.0") == 0)
		ipStr = "127.0.0.1";
	this->IP = ipStr;

	this->port = _port;
}
void Socket::changeSize(void) {
	*(this->socketSize) = sizeof( *(this->sockItem));
}

