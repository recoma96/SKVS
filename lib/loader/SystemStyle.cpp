
#include "SystemStyle.hpp"
#include "../Exception.hpp"
#include <string>


using namespace std;
string SystemStyle::convertStyleToString(SystemType _sys) {
	if( _sys == SYSTEMTYPE_STANDALONE )
		return "STANDALONE";
	else
		return "DISTRIBUTED";
}
SystemType SystemStyle::convertStyleToType(string _str) {
	if( _str.compare("STANDALONE") == 0 )
		return SYSTEMTYPE_STANDALONE;
	else if( _str.compare("DISTRIBUTED") == 0 )
		return SYSTEMTYPE_DISTRIBUTED;
	else
		throw DataConvertException("DataConvertException SystemManager.cpp line 26: this string is not SystemStyle,");
}

