/***
 *  @file : DataBaseCmd.hpp
 * 	@date 2019/08/19
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 데이터베이스 사용자 클래스
 * 
 ***/

#ifndef DATABASECMD_HPP
# define DATABASECMD_HPP

#include "../packet/Packet.hpp"
#include "../threadAdapter/AdapterThreadUtility.hpp"
#include "DataBase.hpp"

namespace SKVS_DataBase {
    class DataBaseCmd {
    private:
        //클라이언트로부터 전달받은 request packet
        SendCmdPacket* reqPacket;
        weak_ptr<DataBase> connectedDataBase;
    public:
        explicit DataBaseCmd(SendCmdPacket* _reqPacket, shared_ptr<DataBase>& _db) {
            reqPacket = _reqPacket;
            connectedDataBase = _db;
            //reqPacket => null일경우 Exception 송출예정
        }
        inline SendCmdPacket& getPacket(void) noexcept { return *reqPacket; }

        inline weak_ptr<DataBase> useDataBase(void) { 
            shared_ptr<DataBase> dataBasePointerSender = connectedDataBase.lock();
            return dataBasePointerSender->getWeekPtr(); 
        }
        
        inline bool checkDataBase(void) { 
            shared_ptr<DataBase> databaseChecker = connectedDataBase.lock();
            if(databaseChecker) return true;
            else false;
        }
    };
}

#endif