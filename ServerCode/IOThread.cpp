#include "../lib/packet/Packet.hpp"
#include "../lib/user/LoginedUserList.hpp"
#include "../lib/CommandFilter.hpp"
#include "../lib/SockWrapper/ServerSocketManager.hpp"
#include "../lib/SockWrapper/NetworkingManager.hpp"

#include "../lib/Tokenizer.hpp"

#include <string>
#include <list>
#include <map>
#include <deque>
#include <queue>
#include <mutex>

using namespace std;
using namespace SockWrapperForCplusplus;

extern void IOThread(UserList* userList, LoginedUserList* loginedUserList, Socket* sock,
              CommandFilter* cmdFilter, map<int, queue<Packet*, deque<Packet*>>>* packetBridge, mutex* bridgeMutex   ) {
    
    //해당 클라이언트로부터 데이터 수신

}