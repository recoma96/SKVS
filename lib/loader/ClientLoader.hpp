/***
 *  @file : ClientLoader.hpp
 * 	@date 2019/08/18
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 클라이언트에서 login.json으로부터 데이터를 불러옵니다.
 * 
 ***/

#ifndef CLIENTLOADER_HPP
# define CLIENTLOADER_HPP

#include "Loader.hpp"

class ClientLoader : public Loader {
public:
	ClientLoader() : Loader("login.json") { }
	string getID(void);
	string getPaswd(void);
	int getConnectPort(void);
	string getConnectIP(void);

};

#endif
