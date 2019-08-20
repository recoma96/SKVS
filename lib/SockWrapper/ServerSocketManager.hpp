/***
 *  @file : ServerSocketManager.hpp
 * 	@version : 0.0.1
 * 	@date 2019/18/09
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : Server가 사용하는 함수입니다.
 *  @license : MIT-License
 * 
 ***/

#ifndef SERVERSOCKETMANAGER_HPP
# define SERVERSOCKETMANAGER_HPP

#include "SocketManager.hpp"

#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

#include <iostream>
#include <string>

using namespace std;

namespace SockWrapperForCplusplus {

	const bool bindSocket(Socket* _server );
	const bool listenClient(Socket* _server,const int _range );
	const bool acceptClient( Socket* _server, Socket* _client );

}


#endif
