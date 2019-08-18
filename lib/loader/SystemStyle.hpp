/***
 *  @file : SystemStyle.hpp
 * 	@date 2019/08/18
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : systemType (standalone, distributed) 를 지정합니다.
 * 
 ***/

#ifndef SYSTEMSTYLE_HPP
#define SYSTEMSTYLE_HPP
#include <string>

using namespace std;
enum SystemType{ 

	SYSTEMTYPE_STANDALONE = 1,
	SYSTEMTYPE_DISTRIBUTED = 2	
};

class SystemStyle {
public:

	static string convertStyleToString(SystemType _sys);
	static SystemType convertStyleToType(string _str);

};

#endif
