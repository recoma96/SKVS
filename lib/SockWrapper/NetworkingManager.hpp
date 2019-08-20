/***
 *  @file : NetworkingSocketManager.hpp
 * 	@version : 0.0.1
 * 	@date 2019/18/09
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : Server-client간의 데이터를 송수신 할때 사용하는 함수입니다.
 *  @license : MIT-License
 * 
 ***/

#ifndef NETWORKINGMANAGER_HPP
# define NETWORKINGMANAGER_HPP

#include "SocketManager.hpp"

#include <iostream>
#include <string>

#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

using namespace std;


namespace SockWrapperForCplusplus {
	
	const ssize_t sendData(Socket* _targetServer, void* _sendData, const size_t _dataSize, int _flag=0);

	//Throw SocketDataSizeZeroException!
	const ssize_t recvData(Socket* _recvServer, void* _readData, const size_t _dataSize, int _flag=0);

}


#endif
