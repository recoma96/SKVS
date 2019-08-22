/***
 *  @file : Tokenizer.hpp
 * 	@date 2019/08/11
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 해당 문자열을 토큰화 합니다.
 * 
 ***/

#ifndef TOKENIZER_HPP
# define TOKENIZER_HPP


//Original : https://gist.github.com/psychoria/4e59f963569bc5bc8e1fdd3d8223250b#file-tokenizer-cpp

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

namespace tok {

	inline static vector<string> tokenizer(const string& data, const char delimiter=' ') {

		vector<string> result;
		string token;

		string getData = data;

		stringstream ss(getData);
		while( getline(ss, token, delimiter)) {
			result.push_back(token);
		}
		return result;
	}

	inline static bool IsAllowedCharacter(string targetStr, string allowedCharacters) {

	    for(int i = 0; i < targetStr.length(); i++) {

		    for(int j = 0; j < allowedCharacters.length(); j++) {
			    if( targetStr[i] == allowedCharacters[j] )
				    return false;
		    }
	    }
	    return true;
    }
}

#endif
