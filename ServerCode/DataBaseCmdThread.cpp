#include "../lib/threadAdapter/AdapterThreadUtility.hpp"
#include "../lib/logbase/LogBase.hpp"
#include "../lib/database/DataBase.hpp"
#include "../lib/packet/Packet.hpp"

#include <thread>
#include <string>

using namespace std;
using namespace ThreadAdapter;

void DataBaseCmdThread(
    SendCmdPacket* requestPacket,
    SKVS_DataBase::DataBase* DB
) {
            
}