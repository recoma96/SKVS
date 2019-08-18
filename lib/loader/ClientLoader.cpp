#include "ClientLoader.hpp"

#include <string>
#include "../rapidjson/document.h"

using namespace std;
using namespace rapidjson;

string ClientLoader::getID(void) {

	return root["user"]["id"].GetString();

}

string ClientLoader::getPaswd(void) {

	return root["user"]["paswd"].GetString();

}

int ClientLoader::getConnectPort(void) {
	return root["server"]["port"].GetInt();
}

string ClientLoader::getConnectIP(void) {

	return root["server"]["ip"].GetString();
}
