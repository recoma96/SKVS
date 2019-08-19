/***
 *  @file : AdapterThreadUtility.hpp
 * 	@date 2019/08/19
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 특정 스레드의 이식성을 향상시키기 위해 추가된 클래스입니다.
 * 
 ***/

#ifndef ADAPTERTHREADUTILITY_HPP
# define ADAPTERTHREADUTILITY_HPP

#include <queue>
#include <deque>
#include <mutex>
#include"../packet/Packet.hpp"

namespace ThreadAdapter {
    class AdapterThreadUtility {
    private:
        queue<Packet*, deque<Packet*>> inputQueue;
        queue<Packet*, deque<Packet*>> outputQueue;
        mutex inputMutex;
        mutex outputMutex;
    public:
        inline void pushInInputQueue(Packet* _insertPacket) {
            inputMutex.lock();
            inputQueue.push(_insertPacket);
            inputMutex.unlock();
        }
        inline Packet* popInOutputQueue(void) {
            if( inputQueue.empty()) return nullptr;

            inputMutex.lock();
            Packet* returnPacket = inputQueue.front();
            inputQueue.pop();
            inputMutex.unlock();
            return returnPacket;
        }
        inline void pushInOutputQueue(Packet* _outputPacket) {
            outputMutex.lock();
            outputQueue.push(_outputPacket);
            outputMutex.unlock();
        }
        inline Packet* popInOuputQueue(void) {

            outputMutex.lock();
            Packet* returnPacket = outputQueue.front();
            outputQueue.pop();
            outputMutex.unlock();
            return returnPacket;
        }
        inline const bool isInputQueueEmpty(void) {
            return inputQueue.empty();
        }
        inline const bool isOutputQueueEmpty(void) {
            return outputQueue.empty();
        }
    };
}


#endif