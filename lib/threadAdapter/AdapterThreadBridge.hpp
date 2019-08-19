/***
 *  @file : AdapterThreadBridge.hpp
 * 	@date 2019/08/19
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 쓰레드 사이의 통로를 중계합니다.
 * 
 ***/
#include <vector>
#include <queue>
#include <deque>
#include <mutex>
#include"../packet/Packet.hpp"

using namespace std;
namespace ThreadAdapter {

    class BridgeQueue : public std::enable_shared_from_this<BridgeQueue> {
    public:
        queue<Packet*, deque<Packet*>> pipe;
        mutex pipeMutex; //상호배제에 사용
        mutex pipeBinarySemaphore; //생산자 -> 소비자 로 진행하게 하는 뮤텍스

        shared_ptr<BridgeQueue> getSharedPtr() { return shared_from_this(); }
    };

    class AdapterThreadBridge {
    private:
        vector<shared_ptr<BridgeQueue>> bridgeQueueContainer;
    public:
        inline void insertQueue(void) noexcept {
            bridgeQueueContainer.push_back(make_shared<BridgeQueue>());
        }
        inline bool deleteQueue(const unsigned int _idx) {
            if( _idx >= bridgeQueueContainer.size() ) return false;

            vector<shared_ptr<BridgeQueue>>::iterator deleteIter = 
                bridgeQueueContainer.begin();
            
            unsigned int counter = 0;
            while( counter != _idx) {
                deleteIter++;
                counter++;
            }

            //삭제
            bridgeQueueContainer.erase(deleteIter);
            return true;
        }

        //입출력
        inline bool pushInQueue(Packet* _packet, const unsigned int _idx) noexcept {
            if( _idx >= bridgeQueueContainer.size() ) return false;

            vector<shared_ptr<BridgeQueue>>::iterator selectIter = 
                bridgeQueueContainer.begin();
            
            unsigned int counter = 0;
            while( counter != _idx) {
                selectIter++;
                counter++;
            }

            (*selectIter)->pipeBinarySemaphore.lock();
            (*selectIter)->pipeMutex.lock();
            (*selectIter)->pipe.push(_packet);
            (*selectIter)->pipeMutex.unlock();

            return true;
        }

        //비어있는 경우,
        inline Packet* popInQueue(const unsigned int _idx) noexcept {
            if( _idx >= bridgeQueueContainer.size() ) return nullptr;
            
            vector<shared_ptr<BridgeQueue>>::iterator selectIter = 
                bridgeQueueContainer.begin();
            
            unsigned int counter = 0;
            while( counter != _idx) {
                selectIter++;
                counter++;
            }

            cout << "shared number : " << selectIter->use_count() << endl;
            //비어있는 경우 블록상태
            //테스트 필요
            while( (*selectIter)->pipe.empty() ) { 
                
            }   

            //입력이 들어오면 블록상태에서 빠져나감
            (*selectIter)->pipeMutex.lock();
            Packet* returnPacket = (*selectIter)->pipe.front();
            (*selectIter)->pipe.pop();
            (*selectIter)->pipeMutex.unlock();
            (*selectIter)->pipeBinarySemaphore.unlock();

            return returnPacket;
        }

    };
}