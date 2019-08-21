#include <queue>
#include <deque>
#include <mutex>

#include "../lib/packet/Packet.hpp"

extern bool removeSerialNum(vector<int>& serialList, int removeNum);

void CmdThread(int cmdNum, 
                queue<Packet*, deque<Packet*>>* packetQueue,
                vector<int>* cmdSerialList, 
                string cmdInterface, 
                mutex* packetQueueMutex) {
    //cmdInterface는 사용자 입장에서 멀티스레드형식으로 수행될 경우
    //입력 인터페이스가 안보이므로 cmdThread가 명령수행이 끝나면 cmdIterface를 출력합니다.


    removeSerialNum(*cmdSerialList, cmdNum);

}