/***
 *  @file : Loader.hpp
 * 	@date 2019/08/18
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : Json파일에서 데이터를 불러오거나 수정합니다.
 * 
 ***/

#ifndef LOADER_HPP
# define LOADER_HPP

#include "../rapidjson/document.h"
#include <iostream>
#include <string>

using namespace std;
using namespace rapidjson;

class Loader {
protected:
	string fileRoot;
	Document root; //실제 사용하는 json구조체
public:
	Loader(const string _fileRoot);
	Document& getRoot(void);

	//throw FileException
	void updateFile(void);

};

#endif
