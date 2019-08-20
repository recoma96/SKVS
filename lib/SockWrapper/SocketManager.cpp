#include "Socket.hpp"
#include "SocketManager.hpp"

#include <string>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;
//using namespace SockWrapperForCplusplus;

const bool SockWrapperForCplusplus::setSocket(Socket* _socket) {

	if( _socket->setDiscripter( socket(PF_INET, SOCK_STREAM, 0)) >= 3 )
		return true;
	else return false;

}

const bool SockWrapperForCplusplus::closeSocket(Socket* _socket ) {

	if( close( _socket->getDiscripter()) == 0 ) {
		_socket->setDiscripter(-1);
		return true;
	} else false;
	
}

const bool SockWrapperForCplusplus::setSocketOption(Socket* _socket, int _level, int _optname, 
									void* _optival, socklen_t _optlen) {
	
	if( setsockopt(_socket->getDiscripter(), _level, _optname, _optival, _optlen) == 0)
		return true;
	else return false;
									
}
