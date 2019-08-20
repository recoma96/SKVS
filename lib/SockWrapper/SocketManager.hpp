/***
 *  @file : SocketManager.hpp
 * 	@version : 0.0.1
 * 	@date 2019/18/09
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : Server, Client 두개 다 사용하는 함수입니다.
 *  @license : MIT-License
 * 
 ***/

#ifndef SOCKETMANAGER_HPP
# define SOCKETMANAGER_HPP

#include "Socket.hpp"

#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

namespace SockWrapperForCplusplus {

	const bool setSocket(Socket* _socket); ///<Socket Class를 socket함수를 사용하여 socket discripter를 생성합니다.
	const bool closeSocket(Socket* _socket); ///<해당 Socket을 닫습니다.

	const bool setSocketOption(Socket* _socket, int _level, int _optname, 
									void* _optival, socklen_t _optlen);
									// 해당 socket option을 설정합니다. 사용법은 C-POSIX의 setsockoption과 동일합니다.

}

#endif
