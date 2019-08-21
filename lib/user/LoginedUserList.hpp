#ifndef LOGINEDUSERLIST_HPP
# define LOGINEDUSERLIST_HPP


#include "LoginedUser.hpp"
#include <string>
#include <list>

using namespace std;

class LoginedUserList {
private:
	list<LoginedUser> userList;
public:
	bool insertLoginedUser(LoginedUser _loginedUser);
	bool deleteLoginedUser(LoginedUser _deleteUser);

	bool searchLoginedUser(const string _username) noexcept;

	//동기화용
	list<LoginedUser>& getList(void);

	void display(void);

};

#endif
