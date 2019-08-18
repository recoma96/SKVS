


#include "Loader.hpp"
#include "../Exception.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <locale>

#include "../rapidjson/document.h"
#include "../rapidjson/prettywriter.h"

using namespace std;
using namespace rapidjson;


Loader::Loader(const string _fileRoot) {

	//제이슨 데이터 확인 및 읽어서 데이터저장
	
	ifstream exportJson(_fileRoot);

	if(!exportJson)
		throw FileException("can't open file.");

	string buffer; //가변길이 string으로 데이터 저장 후
	//char* 로 변환 한 다음 document에 데이터를 삽입합니다.
	char buf = 0;

	while( (buf=exportJson.get()) != EOF ) 
	{
		buffer.append(1,buf);
	}

	this->root.Parse( buffer.c_str() );

	if( !this->root.IsObject() )
		throw FileException("failed to Parser.");

	exportJson.close();

	this->fileRoot = _fileRoot;
}

Document& Loader::getRoot(void) {

	return this->root;

}

void Loader::updateFile(void) {

	
	StringBuffer sb;
	rapidjson::PrettyWriter<StringBuffer> writer(sb);
	this->root.Accept(writer);
	string jsonString = sb.GetString();
	//cout << "filename : " << endl;
	ofstream updateJson(this->fileRoot);
	if(!updateJson)
		throw FileException("can't open file.");
	if( updateJson.is_open())
		updateJson << jsonString;
	updateJson.close();

}
