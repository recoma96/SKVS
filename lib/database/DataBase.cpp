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
#include "../Exception.hpp"

#include <string>
#include <list>
#include <map>
#include <vector>

using namespace std;
using namespace CommandList;
using namespace structure;

//데이터 루트 찾기
//Ex) key01.key02... (단 key02가 key01의 자식 노드(키)로 연결되어있어야 함)
//throw DataException
DataElement* SKVS_DataBase::DataBase::findDatabyRoot(const string root, string& errorMsg) {

    vector<string> dataRoot = tok::tokenizer(root, '.');

    //벡터가 비었을 경우
    if(dataRoot.empty())
        return nullptr;
    return nullptr;


    DataElement* currentRoot = nullptr;
    //한개 이상 검색 시 탐색 시작
    //처음은 최상위 루트에서 검색
    for( list<DataElement*>::iterator iter = dataBase.begin(); 
        iter != dataBase.end(); iter++ ) {

        if( (*iter)->getDataToString().compare(dataRoot[0]) == 0 ) {
            currentRoot = (*iter);
            break;
        }
        
    }
    if( currentRoot == nullptr )
        return nullptr; //데이터 못찾음
    
    for(int i = 1; i<dataRoot.size(); i++) {
        
        //해당 루트의 데이터타입 검색
        //하위 링크가 존재하는 구조체타입은
        //Dynamic 계열 구조체 밖애 없습니다.

        switch(currentRoot->getStructType()) {
            case STRUCTTYPE_DYNAMICLIST:
                
                try {
                    currentRoot = &(((DynamicList*)(currentRoot))->getChildElementByKey(dataRoot[i]));
                    continue;
                } catch(DataLinkException e) {
                    errorMsg = dataRoot[i] + " is not found in " + dataRoot[i-1];
                    return nullptr;
                }
            break;
            case STRUCTTYPE_DYNAMICHASHMAP:
                try {
                    currentRoot = &(((DynamicHashMap*)(currentRoot))->getChildElementByKey(dataRoot[i]));
                    continue;
                } catch(DataLinkException e) {
                    errorMsg = dataRoot[i] + " is not found in " + dataRoot[i-1];
                    return nullptr;
                }

            break;
            default:
                errorMsg = "This Data Struct Type is not Dynamic";
                return nullptr;
            break;
        }
    }
    return currentRoot;
}


//명령수행중에 명령문이 유효하지 않아 수행하기 불가능한 경우
//예외처리
void SKVS_DataBase::DataBase::exceptError(SendCmdPacket& _requestPacket, string errorMsg) {

    //에러메세지 송출
    this->queueAdapter.lock()->pushInOutputQueue(
        new RecvMsgPacket(
            _requestPacket.getUserName(),
            _requestPacket.getIP(),
            _requestPacket.getCmdNum(),
            _requestPacket.getSock(),
            errorMsg
        )
    );

    //명령수행을 끝내는 패킷 송출
    this->queueAdapter.lock()->pushInOutputQueue(
        new SignalPacket(
            _requestPacket.getUserName(),
            _requestPacket.getIP(),
            _requestPacket.getCmdNum(),
            _requestPacket.getSock(),
            SIGNALTYPE_ERROR
        )
    );       
    return;
}

//데이터 컨테이너 삽입
//최상위에서만 삽입 가능
void SKVS_DataBase::DataBase::create(SendCmdPacket& _requestPacket) {

    string errorMsg;
    string logMsg;

    //시작을 알리는 패킷 전송
    SignalPacket* sigPacket = new SignalPacket(
        _requestPacket,
        SIGNALTYPE_RECVSTART
    );
    this->queueAdapter.lock()->pushInOutputQueue(sigPacket);

    //인자수 적합성 판단.
    if(_requestPacket.getCmdArray().size() < 3) {
        errorMsg = "create [struct-type] [new-key] <data-type>";

        exceptError(_requestPacket, errorMsg); 
        return;
    }

    vector<string> cmdVec = _requestPacket.getCmdArray();

    //첫번 째 인자수 적합성 판단
    if( !(cmdVec[1].compare(printedBasic) == 0|| 
        cmdVec[1].compare(printedOneSet) == 0||
        cmdVec[1].compare(printedMultiSet) == 0||
        cmdVec[1].compare(printedDynamicList) == 0|| 
        cmdVec[1].compare(printedStaticList) == 0|| 
        cmdVec[1].compare(printedDynamicHashMap) == 0|| 
        cmdVec[1].compare(printedStaticHashMap) == 0 )
    ) {
        errorMsg = cmdVec[1] + "is not container";
        exceptError(_requestPacket, errorMsg); 
        return;
    }

    //키 유효성 판단
    if(!tok::IsAllowedCharacter(cmdVec[2], R"(~!@#$%^&*()_+-=[];'./,>{}:"<?")")) {
        errorMsg = R"(Invalid Key-name list = ~!@#$%^&*()_+-=[];'./,>{}:"<?)";
        exceptError(_requestPacket, errorMsg); 
        return;
    }

    //키가 존재하는 지 판단
    for(list<DataElement*>::iterator iter = dataBase.begin(); 
        iter != dataBase.end(); iter++ ) {
        
        if( (*iter)->getDataToString().compare(cmdVec[2]) == 0 ) {
            errorMsg = "This key has already exist";
            exceptError(_requestPacket, errorMsg);
            return;
        }
    }

    //데이터 타입에 따른 키 생성
    StructType insertType = convertStringToType<StructType>(cmdVec[1]);

    switch(insertType) {
        //1. Basic
        case STRUCTTYPE_BASIC:
        {

            //데이터타입 검색
            if( cmdVec.size() != 5 ) {
                errorMsg = "create basic [key] [data-type] [value]";
                exceptError(_requestPacket, errorMsg);
                return;
            } else {
                try {
                    //데이터 타입 검색
                    //데이터 타입이 안맞을 경우 throw Exception
                    DataType insertDataType = convertStringToType<DataType>(cmdVec[3]);
                    
                    //해당 데이터와 타입이 맞는 지 확인
                    CheckDataType checkDataType;
                    
                    if(!checkDataType(cmdVec[4], insertDataType)) {
                        errorMsg = cmdVec[4] + " and " + cmdVec[3] + " is not matched.";
                        exceptError(_requestPacket, errorMsg);
                        return;
                    }

                    //value도 데이터타입에 따른 검사
                    dataBase.push_back(new Basic(cmdVec[2], cmdVec[4], insertDataType));

                    //결과데이터 전송
                    string result = "Basic : " + cmdVec[2] + " -> " + cmdVec[4];
                    this->queueAdapter.lock()->pushInOutputQueue(
                        new RecvMsgPacket(
                            _requestPacket,
                            result
                        )
                    );

                    this->queueAdapter.lock()->pushInOutputQueue(
                        new RecvMsgPacket(
                            _requestPacket,
                            "Create Complete"
                        )
                    );

                    this->queueAdapter.lock()->pushInOutputQueue(
                        new SignalPacket(
                            _requestPacket,
                            SIGNALTYPE_RECVEND
                        )
                    );

                    //로그데이터 생성
                    logMsg = "create key : " + cmdVec[2] + " by Basic";
                    LogPacket* logPacket = new LogPacket(
                            _requestPacket,
                            logMsg
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(logPacket);
                    cout << logPacket->getStatement() << endl;

                    return;

                } catch(DataException& e) {
                    errorMsg = e.getErrorMsg();
                    exceptError(_requestPacket, errorMsg);
                    return;
                }
            }
        }
        break;

        case STRUCTTYPE_ONESET: case STRUCTTYPE_MULTISET:
        {
            //create oneset [key] [datatype]
            if( cmdVec.size() != 4 ) {
                errorMsg = "create oneset [key] [datatype]";
                exceptError(_requestPacket, errorMsg);
                return;
            } else {
                try {
                    //데이터 타입 검색
                    //데이터 타입이 안맞을 경우 throw Exception
                    DataType insertDataType = convertStringToType<DataType>(cmdVec[3]);

                    if(insertType == STRUCTTYPE_ONESET) {
                        dataBase.push_back(new OneSet(cmdVec[2], insertDataType));

                        string result =  "OneSet : " + cmdVec[2] + " | " + "DataType : " + cmdVec[3];
                        this->queueAdapter.lock()->pushInOutputQueue(
                            new RecvMsgPacket(
                                _requestPacket,
                                result
                            )
                        );

                        logMsg = "Create Key " + cmdVec[2] + " by OneSet";
                    } else {

                        dataBase.push_back(new MultiSet(cmdVec[2], insertDataType));

                        string result =  "MultiSet : " + cmdVec[2] + " | " + "DataType : " + cmdVec[3];
                        this->queueAdapter.lock()->pushInOutputQueue(
                            new RecvMsgPacket(
                                _requestPacket,
                                result
                            )
                        );
                        logMsg = "Create Key " + cmdVec[2] + " by MultiSet";

                    }

                    //INPUT COMPLETE
                    this->queueAdapter.lock()->pushInOutputQueue(
                        new RecvMsgPacket(
                            _requestPacket,
                            "Create complete"
                        )
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                        new SignalPacket(
                            _requestPacket,
                            SIGNALTYPE_RECVEND
                        )
                    );

                    //로그 작성
                    LogPacket* logPacket = new LogPacket(
                            _requestPacket,
                            logMsg
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(logPacket);
                    cout << logPacket->getStatement() << endl;
                    return;

                } catch(DataException& e) {
                    errorMsg = e.getErrorMsg();
                    exceptError(_requestPacket, errorMsg);
                    return;
                }
            }
        }
        break;

        case STRUCTTYPE_STATICLIST:
        {
            //create staticlist [key] [datatype]
            if( cmdVec.size() != 4 ) {
                errorMsg =  "create staticlist [key] [datatype]";
                exceptError(_requestPacket, errorMsg);
                return;
            } else {
                try {
                    DataType insertDataType = convertStringToType<DataType>(cmdVec[3]);

                    dataBase.push_back(new StaticList(cmdVec[2], insertDataType));
                    string result = "Static List : " + cmdVec[2] + " | " + "DataType : " + cmdVec[3];
                    logMsg =  "Create Key " + cmdVec[2] + " by StaticList";
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new RecvMsgPacket(
                                _requestPacket,
                                result
                            )
                        );
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new RecvMsgPacket(
                                _requestPacket,
                                "Create complete"
                            )
                        );
                        
                    this->queueAdapter.lock()->pushInOutputQueue(
                        new SignalPacket(
                            _requestPacket,
                            SIGNALTYPE_RECVEND
                        )
                    );
                    LogPacket* logPacket = new LogPacket(
                        _requestPacket,
                        logMsg
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(logPacket);
                    cout << logPacket->getStatement() << endl;

                    return;
                } catch (DataException& e) {
                    errorMsg = e.getErrorMsg();
                    exceptError(_requestPacket, errorMsg);
                    return;
                }
            }
        }
        break;
        case STRUCTTYPE_DYNAMICLIST:
        {
            //create dynamiclist [key]
            if(cmdVec.size() != 3 ) {
                errorMsg  = "create dynamiclist [key] ";
                exceptError(_requestPacket, errorMsg);
                return;
            } else {
                dataBase.push_back(new DynamicList(cmdVec[2]));

                string result = "Dynamic List : " + cmdVec[2];
                logMsg = "Create Key " + cmdVec[2] + " by StaticList";
                this->queueAdapter.lock()->pushInOutputQueue(
                    new RecvMsgPacket(
                        _requestPacket,
                        result
                    )
                );
                this->queueAdapter.lock()->pushInOutputQueue(
                    new RecvMsgPacket(
                        _requestPacket,
                        "Create Complete"
                    )
                );
                this->queueAdapter.lock()->pushInOutputQueue(
                        new SignalPacket(
                            _requestPacket,
                            SIGNALTYPE_RECVEND
                    )
                );
                LogPacket* logPacket = new LogPacket(
                        _requestPacket,
                        logMsg
                    );
                this->queueAdapter.lock()->pushInOutputQueue(logPacket);
                cout << logPacket->getStatement() << endl;
                return;
                
            }
        }
        break;
        case STRUCTTYPE_STATICHASHMAP:
        {
            //create statichashmap [key] [datatype]

            if( cmdVec.size() != 4 ) {
                cout << "create statichashmap [key] [datatype]" << endl;
                return;
            } else {
                try {
                    DataType insertDataType = convertStringToType<DataType>(cmdVec[3]);
                    dataBase.push_back(new StaticHashMap(cmdVec[2], insertDataType));

                    string result = "Static HashMap : " + cmdVec[2] + " | " + "DataType : " + cmdVec[3];
                    logMsg = "Create Key " + cmdVec[2] + " by StaticHashMap";
                    this->queueAdapter.lock()->pushInOutputQueue(
                        new RecvMsgPacket(
                            _requestPacket,
                            result
                        )
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                        new RecvMsgPacket(
                            _requestPacket,
                            "Create complete"
                        )
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                        new SignalPacket(
                            _requestPacket,
                            SIGNALTYPE_RECVEND
                        )
                    );
                    LogPacket* logPacket = new LogPacket(
                        _requestPacket,
                        logMsg
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(logPacket);
                    cout << logPacket->getStatement() << endl;
                    return;
                } catch (DataException& e) {
                    errorMsg = e.getErrorMsg();
                    exceptError(_requestPacket, errorMsg);
                    return;
                }
            }
        }
        break;

        case STRUCTTYPE_DYNAMICHASHMAP:
        {
            //create dynamichashmap [key]
            if(cmdVec.size() != 3) {
                errorMsg =  "create dynamichashmap [key]";
                exceptError(_requestPacket, errorMsg);
                return;
            } else {
                dataBase.push_back(new DynamicHashMap(cmdVec[2]));
                string result = "Dynamic HashMap : " + cmdVec[2];
                logMsg = "Create Key " + cmdVec[2] + " by StaticHashMap";

                this->queueAdapter.lock()->pushInOutputQueue(
                    new RecvMsgPacket(
                        _requestPacket,
                        result
                    )
                );
                this->queueAdapter.lock()->pushInOutputQueue(
                    new RecvMsgPacket(
                        _requestPacket,
                        "Create compelete"
                    )
                );
                this->queueAdapter.lock()->pushInOutputQueue(
                        new SignalPacket(
                            _requestPacket,
                            SIGNALTYPE_RECVEND
                    )
                );
                LogPacket* logPacket = new LogPacket(
                        _requestPacket,
                        logMsg
                    );
                this->queueAdapter.lock()->pushInOutputQueue(logPacket);
                cout << logPacket->getStatement() << endl;                 
                return;
            }
        }
        break;

        default: //나와서는 안되는 케이스
            errorMsg = "System Error : Element is try to inserting!";
            this->queueAdapter.lock()->pushInOutputQueue(
                new RecvMsgPacket(
                    _requestPacket,
                    errorMsg
                )
            );
            this->queueAdapter.lock()->pushInOutputQueue(
                new SignalPacket(
                    _requestPacket,
                    SIGNALTYPE_ERROR
                )
            );
            //로그 작성
            LogPacket* logPacket = new LogPacket(
                        _requestPacket,
                        logMsg
                    );
            this->queueAdapter.lock()->pushInOutputQueue(logPacket);
            cout << logPacket->getStatement() << endl;  
            
            return;
    }

    return;

}
void SKVS_DataBase::DataBase::drop(SendCmdPacket& _requestPacket) {
    vector<string> cmdVec = _requestPacket.getCmdArray();
}

void SKVS_DataBase::DataBase::insert(SendCmdPacket& _requestPacket) {
    vector<string> cmdVec = _requestPacket.getCmdArray();



}

void SKVS_DataBase::DataBase::runCmd(SendCmdPacket& _requestPacket) {

    if( _requestPacket.getCmdArray()[0].compare(DB_Command::Create) == 0)
        create(_requestPacket);
    else if( _requestPacket.getCmdArray()[0].compare(DB_Command::Create) == 0)
    else { //명령어를 잘못 입력되는 경우인데
        //이부분은 이초에 CommandFilter에서 걸러지는 부분이므로
        //이 구간에 들어올 경우 System Error 발생
        string errorMsg = "System Error : invaild Command try to access database system";
        //로그데이터 등록
        LogPacket* logPacket = new LogPacket(
            _requestPacket,
            errorMsg
        );

        //로그데이터 출력
        this->queueAdapter.lock()->pushInOutputQueue(logPacket);
        cout << logPacket->getStatement() << endl;
        //클라이언트의 해당 커멘드 스레드에 대한 종료 시그널 보내기

        SignalPacket* terminatePacket = new SignalPacket(
            _requestPacket,
            SIGNALTYPE_ERROR
        );
        this->queueAdapter.lock()->pushInOutputQueue(terminatePacket);
    }
}