/***
 *  @file : Socket.hpp
 * 	@version : 0.0.1
 * 	@date 2019/18/09
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : C-POSIX TCP SOCKET Function을 C++상에서 편하게 사용하기 위해
 * 				Socket연결에 필요한 정보를 class Socket으로 Wrapping
 *  @license : MIT-License
 * 
 ***/

#ifndef SOCKET_HPP
# define SOCKET_HPP


#include <iostream>
#include <string>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;

namespace SockWrapperForCplusplus {
	
	class Socket {
	private:
		string IP;
		short port;
		bool serverFlag;
		int sock;
		struct sockaddr_in* sockItem;
		socklen_t* socketSize;
	public:

		explicit Socket(const string _IP, const short _port, const bool _isServer) noexcept;

		//클라이언트로부터 정보를 받아오는 경우
		Socket(void) noexcept;

		
		~Socket(void);

		//get function
		string getIP(void) noexcept;
		short getPort(void) noexcept;
		int getDiscripter(void) noexcept;
		bool isServer(void) noexcept;
	
	
		// get Sturct/Class Function
		// socketitem이나 socksize는 실제 C계열의 TCP함수에서 사용하므로
		// 참조차 사용 불가
		sockaddr_in* getSocketItem(void) noexcept;
		socklen_t* getSocketSizePointer(void) noexcept;

		Socket* getCopiedSocket(void) noexcept;
		Socket& getSocket(void);
	

		//setting
		int setDiscripter(const int _sock);
		void clientUpdate(short _port);
		void changeSize(void); //bind에 사용


		//getSockopt
	};
}

#endif
