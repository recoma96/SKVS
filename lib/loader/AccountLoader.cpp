#include "AccountLoader.hpp"


#include <iostream>
#include <string>
#include <fstream>
#include "../rapidjson/document.h"
#include "../user/UserList.hpp"

#include <unistd.h>
using namespace rapidjson;
using namespace std;

//jsong데이터르 추출해 유저리스트를 생성
//Server에서 사용
UserList* AccountLoader::makeUserList(void) {

	UserList* userList = new UserList();

	const Value& uList = root["account"];

	for( size_t index = 0; index < uList.Size(); index++) {

		User newUser(
			uList[index]["id"].GetString(),
			uList[index]["pswd"].GetString(),
			User::userLevelConverter(uList[index]["userlevel"].GetString())
		);

		userList->insertUser( newUser );
	}

	return userList;
}

bool AccountLoader::addUser(User& newUser) {

	Value& userListJson = root["account"];

	//똑같은건 저장이 불가능합니다
	for( SizeType counter = 0; counter < userListJson.Size(); counter++ ) {
		if( newUser.getID().compare( userListJson[counter]["id"].GetString()) == 0 )
			return false;
	}

	//새로 입력할 데이터를 Json value에 저장
	Value content(kObjectType);
	{

		Value ID;
		Value pswd;
		Value userLevel;

		ID.SetString( StringRef(newUser.getID().c_str()) );
		pswd.SetString( StringRef(newUser.getPassword().c_str()) );
		userLevel.SetString( StringRef(
					User::userLevelDeConverter( newUser.getUserLevel()).c_str()));

		content.AddMember("id", ID, root.GetAllocator() );
		content.AddMember("pswd", pswd, root.GetAllocator());
		content.AddMember("userlevel", userLevel, root.GetAllocator());

	}

	//json에 content를 저장
	userListJson.PushBack(content, root.GetAllocator() );
	return true;

}

bool AccountLoader::deleteUser(const string _deleteUserName) {

	Value& uList = root["account"];


	for( Value::ConstValueIterator iter = uList.Begin();
			iter != uList.End();
			iter++ ) {
		if( _deleteUserName.compare( (*iter)["id"].GetString() ) == 0 ) {

			uList.Erase(iter);
			return true;
		}
	}

	return false;

}

bool AccountLoader::setUserPswd( const string _userName, const string _newPaswd ) {

	Value& uList = root["account"];

	for( Value::ValueIterator iter = uList.Begin();
			iter != uList.End();
			iter++ ) {

		if( _userName.compare( (*iter)["id"].GetString() ) == 0 ) {

			string changeID = (*iter)["id"].GetString();
			string changePswd = _newPaswd;
			UserLevel changeLevel = User::userLevelConverter((*iter)["userlevel"].GetString());

			//수정한 부분을 다새 생성
			User changeUser( changeID, changePswd, changeLevel );

			//기존에 있는 데이터 삭제하고 변경된 데이터로 갱신
			uList.Erase(iter);
			addUser(changeUser);

			return true;
		}
	}
	return false;
}

bool AccountLoader::setUserLevel( const string _userName, const UserLevel _userLevel ) {

	Value& uList = root["account"];

	for( Value::ValueIterator iter = uList.Begin();
			iter != uList.End(); iter++ ) {

		if( _userName.compare( (*iter)["id"].GetString() ) == 0 ) { 
			
			string changeID = (*iter)["id"].GetString();
			string changePswd = (*iter)["pswd"].GetString();
			UserLevel changeLevel = _userLevel;

			User changeUser( changeID, changePswd, changeLevel);

			uList.Erase(iter);
			addUser(changeUser);

			return true;
		}

	}

	return false;

}

