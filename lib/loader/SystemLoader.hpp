/***
 *  @file : SystemLoader.hpp
 * 	@date 2019/08/18
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : system_config.json으로부터 데이터를 불러옵니다.
 * 
 ***/

#ifndef SYSTEMLOADER_HPP
# define SYSTEMLOADER_HPP

#include "Loader.hpp"
#include "SystemStyle.hpp"

#include <iostream>
#include <string>

using namespace std;

class SystemLoader : public Loader {
public:
	SystemLoader() : Loader("system_config.json") { }

	int getPort(void);
	SystemType getStyle(void);
	const string getLogRoot(void);


};


#endif
