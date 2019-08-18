/***
 *  @file : LoginedUser.hpp
 * 	@date 2019/08/18
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 해당 프로그램에 등록되어 있는 User를 관리합니다. 단 한 군데만 정의합니다.
 * 
 ***/

#ifndef USERLIST_HPP
# define USERLIST_HPP

#include "User.hpp"
#include "LoginedUserList.hpp"

#include <string>
#include <list>

class UserList {
private:
	list<User> userList;
public:
	bool insertUser(const User _newUser);
	bool deleteUser(const string _username);

	//list에 있는 User데이터를 꺼내서 출력 및 수정
	//데이터가 존재하지 않을 경우 UserException 호출
	User& useUser(const string _username);

	//만약에 Server가 User관련 정보를 변경했을 경우 LoginedUser에게도 동기화를 해야 합니다.
	void ayncUserList(LoginedUserList& _targetList);

	User getCopiedUserData(const string _searchName);

};





#endif
