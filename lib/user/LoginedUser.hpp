/***
 *  @file : LoginedUser.hpp
 * 	@date 2019/08/18
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 로그인된 사용자의 기본 클래스 입니다.
 * 
 ***/


#ifndef LOGINEDUSER_H
# define LOGINEDUSER_H

#include <string>
#include "User.hpp"

using namespace std;

class LoginedUser : public User {
private:
	string IP;
	int sock;
public:
	LoginedUser(const string _ID, const string _password, const UserLevel _userLV,
					const string _IP, const int sock);

	int getSocket(void);
	string getIP(void);

	bool operator==(LoginedUser arg);
};

#endif
