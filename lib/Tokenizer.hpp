#ifndef TOKENIZER_HPP
# define TOKENIZER_HPP


//Original : https://gist.github.com/psychoria/4e59f963569bc5bc8e1fdd3d8223250b#file-tokenizer-cpp

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

namespace tok {

	static vector<string> tokenizer(const string& data, const char delimiter=' ') {

		vector<string> result;
		string token;

		string getData = data;

		stringstream ss(getData);
		while( getline(ss, token, delimiter)) {
			result.push_back(token);
		}
		return result;
	}
}

#endif
