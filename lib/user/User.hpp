/***
 *  @file : User.hpp
 * 	@date 2019/08/18
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 사용자의 기본 클래스 입니다.
 * 
 ***/

#ifndef USER_HPP
# define USER_HPP

#include <string>
using namespace std;

enum UserLevel {

	USERLEVEL_CLIENT = 1,
	USERLEVEL_ROOT

};

enum userDataLength {

	MIN_ID_LENGTH = 4,
	MAX_ID_LENGTH = 16,
	MIN_PSWD_LENGTH = 8,
	MAX_PSWD_LENGTH = 16

};

class User { 

protected:
	string ID;
	string password;
	UserLevel userLV;

public:
	//Throw userlengthexception
	User( const string _ID, const string _password, const UserLevel _userLV );
	User() {
		
	} //비어있는 객체 선언

	string getID(void);
	string getPassword(void);
	UserLevel getUserLevel(void);

	void setID(string _newID);
	void setPassword(string _newPassword);
	void setUserLevel(UserLevel _newUserLV );

	bool operator==(User arg);
	
	//throws DataConvertException
	static UserLevel userLevelConverter(string _str);
	static string userLevelDeConverter(UserLevel _level);
};


#endif
