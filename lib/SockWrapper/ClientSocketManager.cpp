#include "SocketManager.hpp"
#include "ClientSocketManager.hpp"

#include <string>
#include <iostream>

#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>


const bool SockWrapperForCplusplus::connectToServer(Socket* _server) {

	if( connect(_server->getDiscripter(), 
				(struct sockaddr*)_server->getSocketItem(),
				sizeof( *(_server->getSocketItem())) )
					 == -1 ) { 
		return false;
	} 
	else return true;

}
