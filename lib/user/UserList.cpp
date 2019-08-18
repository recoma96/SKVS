#include "UserList.hpp"
#include "../Exception.hpp"

#include "User.hpp"
#include <string>
#include <list>
#include <algorithm>
#include <iostream>

using namespace std;

bool UserList::insertUser(const User _newUser) {

	if( find( this->userList.begin(), this->userList.end(),
				_newUser ) != this->userList.end() ) return false;
	else
		userList.push_back(_newUser);

}

bool UserList::deleteUser(const string _username) {

	for(list<User>::iterator iter = this->userList.begin();
			iter != this->userList.end();
			iter++ ) {

		if( _username.compare( iter->getID() ) == 0 ) {

			this->userList.erase( iter );
			return true;

		}
	}

	return false;

}

bool UserList::changePassword( const string _username, const string _newPassword ) {

	for( list<User>::iterator iter = this->userList.begin();
			iter != this->userList.end();
			iter++ ) {
		
		if( _username.compare( iter->getID() ) == 0 ) {
			iter->setPassword(_newPassword);
			return true;
		}
	}
	return false;
}

bool UserList::changeLevel(const string _username, const UserLevel _newUserLV ) {

	for( list<User>::iterator iter = this->userList.begin(); 
			iter != this->userList.end();
			iter++ ) {
		
		if( _username.compare( iter->getID() ) == 0 ) {
			iter->setUserLevel( _newUserLV );
			return true;
		}

	}
	return false;

}


void UserList::ayncUserList(LoginedUserList& _targetList) {

	list<LoginedUser>& targetList = _targetList.getList();

	//동기화 대상의 로그인리스트
	for( list<LoginedUser>::iterator iter = targetList.begin();
			iter != targetList.end();
			iter++ ) {

		//변경을 시키는 리스트
		for( list<User>::iterator searchIter = this->userList.begin();
				searchIter != this->userList.end();
				searchIter++ ) {

			//데이터 동기화
			if( (User)(*iter) == ( *searchIter ) ) {
				iter->setID( searchIter->getID() );
				iter->setPassword( searchIter->getPassword());
				iter->setUserLevel( searchIter->getUserLevel());
				break;
			}
		}
	}
}

User* UserList::getUserData(const string _searchName) {

	for( list<User>::iterator iter = this->userList.begin();
			iter != this->userList.end();
			iter++ ) {

		if( iter->getID().compare(_searchName) == 0 ) {
			User* searchedUser = nullptr;

			searchedUser = new User( iter->getID(), iter->getPassword(), iter->getUserLevel() );

			return searchedUser;
		}

	}

	throw new UserException("UserException UserList.cpp line 120 : User can't be found.");

}
