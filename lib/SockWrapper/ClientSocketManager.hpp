/***
 *  @file : ClientSocketManager.hpp
 * 	@version : 0.0.1
 * 	@date 2019/18/09
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : Client가 사용하는 함수입니다.
 *  @license : MIT-License
 * 
 ***/

#ifndef CLIENTSOCKETMANAGER_HPP
# define CLIENTSOCKETMANAGER_HPP

#include "SocketManager.hpp"

#include <string>
#include <iostream>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

namespace SockWrapperForCplusplus {

	const bool connectToServer(Socket* _server);

}

#endif
