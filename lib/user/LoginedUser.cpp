#include "LoginedUser.hpp"

#include <string>

using namespace std;

LoginedUser::LoginedUser( const string _ID, const string _password, const UserLevel _userLV,
							const string _IP, const int _sock) : 
			User( _ID, _password, _userLV) {

		this->IP = _IP;
		this->sock = _sock;

}

int LoginedUser::getSocket(void) { return this->sock; }
string LoginedUser::getIP(void) { return this->IP; }

bool LoginedUser::operator==(LoginedUser arg) {

	return ( (this->ID.compare(arg.getID()) == 0) && 
		( this->sock == arg.getSocket()) );
}
