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
	clientCommand.insert(pair<string,TaskMileStone>(User_Setting::setPswd, TASKMILESTONE_SETUSERS));
	clientCommand.insert(pair<string,TaskMileStone>(DB_Command::DisplayUsage, TASKMILESTONE_DATABASE));
	clientCommand.insert(pair<string,TaskMileStone>(System_Control::quit, TASKMILESTONE_SYSTEM));

	//어드민 전용 명령어 삽입
	adminCommand.insert(pair<string,TaskMileStone>(User_Setting::userAdd, TASKMILESTONE_SETUSERS));
	adminCommand.insert(pair<string,TaskMileStone>(User_Setting::userAuthSet, TASKMILESTONE_SETUSERS));

	//루트 전용 명령어 삽입
	/*
	rootCommand.insert(pair<string,TaskMileStone>("snapshot-save", TASKMILESTONE_SNAPSHOT));
	rootCommand.insert(pair<string,TaskMileStone>("snapshot-auto-set", TASKMILESTONE_SNAPSHOT));
	rootCommand.insert(pair<string,TaskMileStone>("snapshot-time-set", TASKMILESTONE_SNAPSHOT));
	rootCommand.insert(pair<string,TaskMileStone>("snapshot-load-set", TASKMILESTONE_SNAPSHOT));
	rootCommand.insert(pair<string,TaskMileStone>("snapshot-init-log", TASKMILESTONE_LOG));
	*/
	rootCommand.insert(pair<string,TaskMileStone>(System_Control::shutdown, TASKMILESTONE_SYSTEM));

	this->userList = _userList;
	
}
TaskMileStone CommandFilter::getMileStone(SendCmdPacket& _packet) {

	string firstCmd = (_packet.getCmdArray())[0];
	string userName = _packet.getUserName();
	UserLevel userLevel;

	//유저 권한 검색하기
	User userData = userList->getCopiedUserData(userName);
	userLevel = userData.getUserLevel();

	//명령어 검색
	//데이터베이스 명령어 검색
	for(map<string, TaskMileStone>::iterator iter = dbCommand.begin();
			iter != dbCommand.end(); iter++ ) {

		if( firstCmd.compare(iter->first) == 0 )
			return iter->second;
	}

	//Client계열 검색
	for( map<string, TaskMileStone>::iterator iter = clientCommand.begin();
			iter != clientCommand.end(); iter++ ) {

		if( firstCmd.compare(iter->first) == 0 )
			return iter->second;

	}

	//Admin 계열 검색
	for( map<string, TaskMileStone>::iterator iter = adminCommand.begin();
			iter != adminCommand.end(); iter++ ) {

		if( firstCmd.compare(iter->first) == 0 ) {

			if( userLevel == USERLEVEL_CLIENT )
				return TASKMILESTONE_NOAUTH;
			else
				return iter->second;
		}
			
	}

	//Root 계열 검색
	for( map<string, TaskMileStone>::iterator iter = rootCommand.begin();
			iter != rootCommand.end(); iter++ ) {
		if( firstCmd.compare(iter->first) == 0 ) {

			if( userLevel != USERLEVEL_ROOT)
				return TASKMILESTONE_NOAUTH;
			else
				return iter->second;

		}

	}


	//명령어가 잘못됨
	return TASKMILESTONE_ERR;

}

