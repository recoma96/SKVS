/***
 *  @file : CommandList.hpp
 * 	@date 2019/08/18
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 서버가 클라이언트로부터 명령어를 받고 패킷이 어디로 가야 하는 지를 가르쳐 주는 클래스입니다.
 * 
 ***/

#ifndef COMMANDFILTER_HPP
# define COMMANDFILTER_HPP

#include <map>
#include <string>
#include "user/UserList.hpp"
#include "packet/Packet.hpp"
using namespace std;

enum TaskMileStone { //패킷이 어디로 가야 할 지를 정해주는 인식표입니다

	TASKMILESTONE_DATABASE,  //데이터베이스로 이동합니다.
	//TASKMILESTONE_SNAPSHOT, //데이터 베이스의 snapshot으로 이동합니다.:
	TASKMILESTONE_SETUSERS, //유저를 새팅하는 Control Center로 이동합니다	:
	//TASKMILESTONE_LOG, //로그쓰레드로 이동합니다.
	//TASKMILESTONE_BROADCAST, //메세지를 다른 유저에게 뿌려줍니다
	TASKMILESTONE_SYSTEM, //시스템명령입니다
	TASKMILESTONE_ERR, //검색이 안되었을 경우입니다
	TASKMILESTONE_NOAUTH //권한문제

};

class CommandFilter {
private:
	map<string, TaskMileStone> dbCommand;
	map<string, TaskMileStone> clientCommand;
	map<string, TaskMileStone> adminCommand;
	map<string, TaskMileStone> rootCommand;
	UserList* userList;
public:
	CommandFilter(UserList* _userList);
	TaskMileStone getMileStone(SendCmdPacket& _packet);
};

#endif
