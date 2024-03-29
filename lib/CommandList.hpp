/***
 *  @file : CommandList.hpp
 * 	@date 2019/08/18
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 명령어 리스트 입니다.
 * 
 ***/


#include <string>

using namespace std;

namespace CommandList {

    namespace DB_Command {

        const string Create = "create";
        const string Drop = "drop";
        const string Get = "get";
        const string Insert = "insert";
        const string Set="set";
        const string Link="link";
        const string UnLink="unlink";
        const string Delete="delete";
        const string Sort="sort";

        const string GetSize="getsize";
        const string GetKey="getkey";
        const string List="list";
    }

    namespace User_Setting {

        const string userAdd = "useradd"; //ex useradd newUser, newPswd
        const string userDel = "userdel";     
    }

    namespace System_Control {
        const string quit = "quit";
        const string shutdown = "shutdown";
    }


}