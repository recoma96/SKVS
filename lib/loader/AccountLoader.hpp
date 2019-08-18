/***
 *  @file : AccountLoader.hpp
 * 	@date 2019/08/18
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : acocunt.json에서 데이터를 불러오거나 데이터를 수정합니다.
 * 
 ***/


#ifndef ACCOUNTLOADER_HPP
# define ACCOUNTLOADER_HPP

#include "Loader.hpp"
#include "../user/UserList.hpp"

using namespace std;

class AccountLoader : public Loader { 
public:
	AccountLoader() : Loader("account.json") { }

	UserList* makeUserList(void);

	bool addUser(User& newUser);
	bool deleteUser(const string _deleteUserName);
	bool setUserPswd(const string _userName, const string _newPaswd);
	bool setUserLevel( const string _userName, const UserLevel _userLevel );


};

#endif
