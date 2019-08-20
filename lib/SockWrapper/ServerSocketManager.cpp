#include "ServerSocketManager.hpp"
#include "Socket.hpp"

#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

#include <iostream>
#include <string>

#include <string.h>
#include <stdlib.h>
#include <errno.h>

using namespace std;

const bool SockWrapperForCplusplus::bindSocket(Socket* _server) {

	if( bind( _server->getDiscripter(),
				(struct sockaddr*)_server->getSocketItem(),
				sizeof( *(_server->getSocketItem()))) == -1 )
		return false;
	else {

		_server->changeSize();
		return true;
	}

}

const bool SockWrapperForCplusplus::listenClient(Socket* _server, const int _range) {

	if( listen( _server->getDiscripter(), _range) < 0 ) return false;
	else return true;

}

const bool SockWrapperForCplusplus::acceptClient( Socket* _server, Socket* _client ) {

	int newSock =  accept( _server->getDiscripter(), 
							(struct sockaddr*)_client->getSocketItem(),
							_client->getSocketSizePointer() );

	
	if( newSock == -1 )  {
		cout << strerror(errno) << endl;
		return false;
	}
	else {
		_client->setDiscripter(newSock);		
		return true;
	}
}
