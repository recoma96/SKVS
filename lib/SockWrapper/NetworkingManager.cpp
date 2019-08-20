#include "NetworkingManager.hpp"
#include "SocketManager.hpp"
#include "SocketException.hpp"

#include <iostream>
#include <string>

#include <errno.h>

#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>

using namespace std;

const ssize_t SockWrapperForCplusplus::sendData(Socket* _targetServer, void* _sendData, const size_t _dataSize, int _flag) {

	size_t dataSize = _dataSize;
	if( _dataSize == 0)
		throw SocketDataSizeZeroException("size of sendData arg is Zero");

	return send( _targetServer->getDiscripter(), (void*)_sendData, dataSize, _flag);

}

const ssize_t SockWrapperForCplusplus::recvData(Socket* _recvServer, void* _readData, const size_t _dataSize, int _flag ) {

	size_t dataSize = _dataSize;
	if( dataSize == 0 ) 
		throw SocketDataSizeZeroException("size of recvData arg is zero.");
	
	return recv( _recvServer->getDiscripter(), _readData, dataSize, _flag);

}
