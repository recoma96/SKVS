#include "Loader.hpp"
#include "SystemLoader.hpp"
#include "SystemStyle.hpp"

#include "../rapidjson/prettywriter.h"
#include "../rapidjson/document.h"

#include <iostream>
#include <string>

using namespace std;
using namespace rapidjson;

int SystemLoader::getPort(void) {

	return root["port"].GetInt();

}

SystemType SystemLoader::getStyle(void) {

	return SystemStyle::convertStyleToType(root["style"].GetString());

}

const string SystemLoader::getLogRoot(void) {
	return root["log"]["location"].GetString();
}