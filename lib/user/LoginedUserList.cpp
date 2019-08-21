#include "LoginedUserList.hpp"

#include <iostream>
#include <string>
#include <list>
#include <algorithm>

using namespace std;

bool LoginedUserList::insertLoginedUser(LoginedUser _loginedUser ) {


	if( find( this->userList.begin(), this->userList.end(),
				 _loginedUser ) != this->userList.end() ) return false;
	else {
		userList.push_back(_loginedUser);
		return true;
	}

}

bool LoginedUserList::deleteLoginedUser(LoginedUser _deleteUser ) {

	for( list<LoginedUser>::iterator iter = this->userList.begin();
			iter != this->userList.end();
			iter++ ) {

		if( iter->getSocket() == _deleteUser.getSocket() ) {
			this->userList.erase( iter );
			return true;
		}

	}

	return false;

}

list<LoginedUser>& LoginedUserList::getList(void) {

	return this->userList;

}

void LoginedUserList::display(void) {

	for( list<LoginedUser>::iterator iter = this->userList.begin();
			iter != this->userList.end();
			iter++ ) {
		
		cout << "name : " << iter->getID() << " pswd: " << iter->getPassword() << endl;

	}

}

bool LoginedUserList::searchLoginedUser(const string _username) noexcept {
	for(list<LoginedUser>::iterator iter = this->userList.begin();
		iter != userList.end(); iter++ ) {
		
		if(iter->getID().compare(_username) == 0)
			return true;
	}
	return false;
}