#include "CommandFilter.hpp"
#include "CommandList.hpp"

using namespace CommandList;

CommandFilter::CommandFilter(UserList* _userList) {

	//DB 명령어 삽입
	dbCommand.insert(pair<string,TaskMileStone>(DB_Command::Create, TASKMILESTONE_DATABASE));
	dbCommand.insert(pair<string,TaskMileStone>(DB_Command::Drop, TASKMILESTONE_DATABASE));
	dbCommand.insert(pair<string,TaskMileStone>(DB_Command::Get, TASKMILESTONE_DATABASE));
	dbCommand.insert(pair<string,TaskMileStone>(DB_Command::Insert, TASKMILESTONE_DATABASE));
	dbCommand.insert(pair<string,TaskMileStone>(DB_Command::Set, TASKMILESTONE_DATABASE));
	dbCommand.insert(pair<string,TaskMileStone>(DB_Command::Link, TASKMILESTONE_DATABASE));
	dbCommand.insert(pair<string,TaskMileStone>(DB_Command::UnLink, TASKMILESTONE_DATABASE));
	dbCommand.insert(pair<string,TaskMileStone>(DB_Command::Delete, TASKMILESTONE_DATABASE));
	dbCommand.insert(pair<string,TaskMileStone>(DB_Command::GetSize, TASKMILESTONE_DATABASE));
	dbCommand.insert(pair<string,TaskMileStone>(DB_Command::GetKey, TASKMILESTONE_DATABASE));
	dbCommand.insert(pair<string,TaskMileStone>(DB_Command::List, TASKMILESTONE_DATABASE));


	//공용 명령어 삽입
	clientCommand.insert(pair<string,TaskMileStone>(System_Control::quit, TASKMILESTONE_SYSTEM));

	//루트 전용 명령어 삽입
	rootCommand.insert(pair<string,TaskMileStone>(System_Control::shutdown, TASKMILESTONE_SYSTEM));
	rootCommand.insert(pair<string,TaskMileStone>(User_Setting::userAdd, TASKMILESTONE_SETUSERS));
	rootCommand.insert(pair<string,TaskMileStone>(User_Setting::userDel, TASKMILESTONE_SETUSERS));

	this->userList = _userList;
	
}
TaskMileStone CommandFilter::getMileStone(SendCmdPacket& _packet) {

	string firstCmd = (_packet.getCmdArray())[0];
	string userName = _packet.getUserName();
	UserLevel userLevel;

	//유저 권한 검색하기
	User userData = userList->getCopiedUserData(userName);
	userLevel = userData.getUserLevel();

	map<string, TaskMileStone>::iterator iter;
	
	//명령어 검색
	if( (iter = dbCommand.find(firstCmd)) != dbCommand.end())
		return iter->second;
	else if( (iter = clientCommand.find(firstCmd)) != clientCommand.end())
		return iter->second;
	else if( (iter = rootCommand.find(firstCmd)) != rootCommand.end()) {
		if( userLevel != USERLEVEL_ROOT)
			return TASKMILESTONE_NOAUTH;
		else
			return iter->second;
	}
	else return TASKMILESTONE_ERR;

}

