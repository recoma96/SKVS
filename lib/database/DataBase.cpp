#include "DataBase.hpp"
#include "../structure/Basic.hpp"
#include "../structure/OneSet.hpp"
#include "../structure/MultiSet.hpp"
#include "../structure/StaticList.hpp"
#include "../structure/DynamicList.hpp"
#include "../structure/StaticHashMap.hpp"
#include "../structure/DynamicHashMap.hpp"

#include "../structure/TypePrinter.hpp"
#include "../Tokenizer.hpp"
#include "../CommandList.hpp"

#include <string>
#include <list>
#include <map>
#include <vector>

using namespace std;
using namespace CommandList;
using namespace structure;

//데이터 컨테이너 삽입
void SKVS_DataBase::DataBase::create(SendCmdPacket& _requestPacket) {
    
}


void SKVS_DataBase::DataBase::runCmd(SendCmdPacket& _requestPacket) {

    if( _requestPacket.getCmdArray()[0].compare(DB_Command::Create) == 0)
        create(_requestPacket);
    else { //명령어를 잘못 입력되는 경우인데
        //이부분은 이초에 CommandFilter에서 걸러지는 부분이므로
        //이 구간에 들어올 경우 System Error 발생
        string errorMsg = "System Error : invaild Command try to access database system";
        //로그데이터 등록
        LogPacket* logPacket = new LogPacket(
            _requestPacket.getUserName(),
            _requestPacket.getIP(),
            _requestPacket.getCmdNum(),
            _requestPacket.getSock(),
            errorMsg
        );

        //로그데이터 출력
        this->queueAdapter.lock()->pushInOutputQueue(logPacket);
        cout << logPacket->getStatement() << endl;

        //클라이언트의 해당 커멘드 스레드에 대한 종료 시그널 보내기

        SignalPacket* terminatePacket = new SignalPacket(
            _requestPacket.getUserName(),
            _requestPacket.getIP(),
            _requestPacket.getCmdNum(),
            _requestPacket.getSock(),
            SIGNALTYPE_ERROR
        );
        this->queueAdapter.lock()->pushInOutputQueue(terminatePacket);
    }
}