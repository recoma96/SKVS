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
DataElement* SKVS_DataBase::DataBase::findDataByRoot(const string root, string& errorMsg) {

    vector<string> dataRoot = tok::tokenizer(root, '.');

    //벡터가 비었을 경우
    if(dataRoot.empty())
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

    if( currentRoot == nullptr ) {
        errorMsg = "Data Not Found";
        return nullptr; //데이터 못찾음
    }
    
    for(int i = 1; i<dataRoot.size(); i++) {
        
        //해당 루트의 데이터타입 검색
        //하위 링크가 존재하는 구조체타입은
        //Dynamic 계열 구조체 밖애 없습니다.

        switch(currentRoot->getStructType()) {
            case STRUCTTYPE_DYNAMICLIST:
                
                try {
                    currentRoot = &(((DynamicList*)(currentRoot))->getChildElementByKey(dataRoot[i]));
                    continue;
                } catch(DataLinkException& e) {
                    errorMsg = dataRoot[i] + " is not found in " + dataRoot[i-1];
                    return nullptr;
                }
            break;
            case STRUCTTYPE_DYNAMICHASHMAP:
                try {
                    currentRoot = &(((DynamicHashMap*)(currentRoot))->getChildElementByKey(dataRoot[i]));
                    continue;
                } catch(DataLinkException& e) {
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
                    Basic* insertData = new Basic(cmdVec[2], cmdVec[4], insertDataType);
                    dataBase.push_back(insertData);

                    //뮤텍스 삽입
                    mutexMap.insert(pair<DataElement*, shared_ptr<mutex>>(insertData, make_shared<mutex>()));


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
                        OneSet* insertData = new OneSet(cmdVec[2], insertDataType);
                        dataBase.push_back(insertData);
                        mutexMap.insert(pair<DataElement*, shared_ptr<mutex>>(insertData, make_shared<mutex>()));

                        string result =  "OneSet : " + cmdVec[2] + " | " + "DataType : " + cmdVec[3];
                        this->queueAdapter.lock()->pushInOutputQueue(
                            new RecvMsgPacket(
                                _requestPacket,
                                result
                            )
                        );

                        logMsg = "Create Key " + cmdVec[2] + " by OneSet";
                    } else {
                        MultiSet* insertData = new MultiSet(cmdVec[2], insertDataType);
                        dataBase.push_back(insertData);
                        mutexMap.insert(pair<DataElement*, shared_ptr<mutex>>(insertData, make_shared<mutex>()));

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

                    StaticList* insertData = new StaticList(cmdVec[2], insertDataType);
                    dataBase.push_back(insertData);
                    mutexMap.insert(pair<DataElement*, shared_ptr<mutex>>(insertData, make_shared<mutex>()));

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
                DynamicList* insertData = new DynamicList(cmdVec[2]);
                dataBase.push_back(insertData);
                mutexMap.insert(pair<DataElement*, shared_ptr<mutex>>(insertData, make_shared<mutex>()));

                string result = "Dynamic List : " + cmdVec[2];
                logMsg = "Create Key " + cmdVec[2] + " by DynamicList";
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
                
                return;
                
            }
        }
        break;
        case STRUCTTYPE_STATICHASHMAP:
        {
            //create statichashmap [key] [datatype]

            if( cmdVec.size() != 4 ) {
                string errorMsg = "create statichashmap [key] [datatype]";
                exceptError(_requestPacket, errorMsg);
                return;
            } else {
                try {
                    DataType insertDataType = convertStringToType<DataType>(cmdVec[3]);

                    StaticHashMap* insertData = new StaticHashMap(cmdVec[2], insertDataType);
                    dataBase.push_back(insertData);
                    mutexMap.insert(pair<DataElement*, shared_ptr<mutex>>(insertData, make_shared<mutex>()));

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

                DynamicHashMap* insertData = new DynamicHashMap(cmdVec[2]);
                dataBase.push_back(insertData);
                mutexMap.insert(pair<DataElement*, shared_ptr<mutex>>(insertData, make_shared<mutex>()));

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
              
            return;
    }
    return;
}

//개체 삭제 <최상위만 가능>
void SKVS_DataBase::DataBase::drop(SendCmdPacket& _requestPacket) {

    string result; //결과메세지
    string logMsg; //로그메세지

    //recvstart
    this->queueAdapter.lock()->pushInOutputQueue(
        new SignalPacket(_requestPacket, SIGNALTYPE_RECVSTART )
    );

    vector<string> cmdVec = _requestPacket.getCmdArray();
    // drop [key]
    if( cmdVec.size() != 2) {
        result= "drop [key]";
        exceptError(_requestPacket, result);
    }

    //drop은 부모데이터가 없는 최상위 데이터만 삭제 가능합니다.

    //키 유효성 판단
    if(!tok::IsAllowedCharacter(cmdVec[1], R"(~!@#$%^&*()_+-=[];'./,>{}:"<?")")) {
        result =  R"(Invalid Key-name list = ~!@#$%^&*()_+-=[];'./,>{}:"<?)";
        exceptError(_requestPacket, result);
        return;
    }

    //키 탐색
    for(list<DataElement*>::iterator iter = dataBase.begin(); 
        iter != dataBase.end(); iter++ ) {
        
        //데이터를 찾았으면 부모데이터가 존재하는 지 확인
        //존재하면 에러 송출
        if( (*iter)->getDataToString().compare(cmdVec[1]) == 0 ) {
            
            //그러나 Basic은 부모데이터가 존재하지 않으므로 바로삭제
            if( (*iter)->getStructType() == STRUCTTYPE_BASIC) {
                
                //뮤텍스 설정
                DataElement* mutexLoc = *iter; //뮤텍스를 검색하기 위한 선언
                mutexMap.find(mutexLoc)->second->lock();

                Basic* deleteData = (Basic*)(*iter);           
                delete deleteData;
                dataBase.erase(iter);

                mutexMap.find(mutexLoc)->second->unlock();

                //뮤텍스 삭제
                mutexMap.erase(mutexLoc);

                result = "key : " + cmdVec[1] + " is deleted.";
                logMsg = "drop key " + cmdVec[1];

                //패킷 송출
                this->queueAdapter.lock()->pushInOutputQueue(
                    new RecvMsgPacket( _requestPacket, result)
                );
                this->queueAdapter.lock()->pushInOutputQueue(
                    new SignalPacket( _requestPacket, SIGNALTYPE_RECVEND)
                );
                this->queueAdapter.lock()->pushInOutputQueue(
                    new LogPacket( _requestPacket, logMsg)
                );
                return;

            } else {
                
                MultiDataStructure* parentChecker = (MultiDataStructure*)(*iter);
                if( parentChecker->isLinkedByParent() ) {
                    result = "This Key have Parent Key.";
                    exceptError(_requestPacket, result);
                    return;
                } else {
                    
                    DataElement* mutexLoc = *iter; //뮤텍스를 검색하기 위한 선언

                    mutexMap.find(mutexLoc)->second->lock();

                    //구조체 타입에 따른 소멸자 호출
                    switch(parentChecker->getStructType()) {

                        case STRUCTTYPE_ONESET:
                            delete (OneSet*)parentChecker;
                        break;
                        case STRUCTTYPE_MULTISET:
                            delete (MultiSet*)parentChecker;
                        break;
                        case STRUCTTYPE_STATICLIST:
                            delete (StaticList*)parentChecker;
                        break;
                        case STRUCTTYPE_DYNAMICLIST:
                            delete (DynamicList*)parentChecker;
                        break;
                        case STRUCTTYPE_STATICHASHMAP:
                            delete (StaticHashMap*)parentChecker;
                        break;
                        case STRUCTTYPE_DYNAMICHASHMAP:
                            delete (DynamicHashMap*)parentChecker;
                        break;
                        default:
                            result = "System Error : This struct is not multi data structure";
                            mutexMap.find(mutexLoc)->second->unlock();
                            mutexMap.erase(mutexLoc);
                            
                            exceptError(_requestPacket, result);
                            return;
                    }
                    //삭제
                    dataBase.erase(iter);

                    mutexMap.find(mutexLoc)->second->unlock();
                    mutexMap.erase(mutexLoc);
                    result = "key : " + cmdVec[1] + " is deleted.";
                    logMsg = "drop key " + cmdVec[1];

                    //패킷 송출
                    this->queueAdapter.lock()->pushInOutputQueue(
                        new RecvMsgPacket( _requestPacket, result)
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                        new SignalPacket( _requestPacket, SIGNALTYPE_RECVEND)
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                        new LogPacket( _requestPacket, logMsg)
                    );
                        return;
                }
            }
        }
    }

    //데이터못찾음
    result=  "Data is not found ";
    exceptError(_requestPacket, result);
    return;
    
}

void SKVS_DataBase::DataBase::insert(SendCmdPacket& _requestPacket) {

    this->queueAdapter.lock()->pushInOutputQueue(
        new SignalPacket(_requestPacket, SIGNALTYPE_RECVSTART )
    );

    vector<string> cmdVec = _requestPacket.getCmdArray();
    string result;
    string logMsg;

    // Only HashMap -> insert [key] [new-key] [new-value]
    // Others -> insert [key] [value]
    // Basic Can't insert Data

    //인자값 측정
    if( cmdVec.size() != 3 && cmdVec.size() != 4 ) {
        result = "insert [key] [new-value]\ninsert [key] [new-key] [new-value] <- Only HashMap";
        exceptError(_requestPacket, result);
        return;
    }

    //키 유효성 판단
    //검색 대상 키 유효성 판단
    if(!tok::IsAllowedCharacter(cmdVec[1], R"(~!@#$%^&*()_+-=[];'/,>{}:"<?")")) {
        result = R"(Invalid Key-name list = ~!@#$%^&*()_+-=[];'/,>{}:"<?)";
        exceptError(_requestPacket, result);
        return;
    }

    //검색 대상 새로 입력할 데이터(또는 해시맵 키) 유효성 판단
    //얘는 온점도 못붙임
    if(!tok::IsAllowedCharacter(cmdVec[2], R"(~!@#$%^&*()_+-=[];'/,.>{}:"<?")")) {
        result = R"(Invalid Key-name list = ~!@#$%^&*()_+-=[];'/,>{}:."<?)";
        exceptError(_requestPacket, result);
        return;
    }

    //데이터 찾기
    DataElement* foundData = findDataByRoot(cmdVec[1], result);

    if(foundData == nullptr) {
        exceptError(_requestPacket, result);
        return;
    }
    //StructType 
    //데이터 타입검색
    switch(foundData->getStructType()) {
        case STRUCTTYPE_BASIC:
            result = "Basic Can't insert Data";
            exceptError(_requestPacket, result);
            return;
        break;

        case STRUCTTYPE_STATICHASHMAP:
        case STRUCTTYPE_DYNAMICHASHMAP:
        {
            //인자값 갯수 판정
            if( cmdVec.size() != 4) {
                result = "insert [key] [new-key] [new-value] <- Only HashMap";
                exceptError(_requestPacket, result);
                return;
            }

            if(!tok::IsAllowedCharacter(cmdVec[3], R"(~!@#$%^&*()_+-=[];'/,.>{}:"<?")")) {
                result =  R"(Invalid Key-name list = ~!@#$%^&*()_+-=[];'/,>{}:."<?)";
                exceptError(_requestPacket, result);
                return;
            }

            //데이터 입력
            if(foundData->getStructType() == STRUCTTYPE_STATICHASHMAP) {

                mutexMap.find(foundData)->second->lock(); //뮤텍스 설정

                if(!((StaticHashMap*)(foundData))->insertKeyValue(cmdVec[2], cmdVec[3])) {

                    mutexMap.find(foundData)->second->unlock(); //뮤텍스 해제

                    result = "failed to insert data";
                    exceptError(_requestPacket, result);
                    return;


                } else {
                    mutexMap.find(foundData)->second->unlock(); //뮤텍스 해제
                    result = "complete";
                    logMsg = "insert " + cmdVec[2] + " -> " + cmdVec[3] + " in " + cmdVec[1];

                    this->queueAdapter.lock()->pushInOutputQueue(
                        new RecvMsgPacket(_requestPacket, result)
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                        new SignalPacket(_requestPacket,SIGNALTYPE_RECVEND)
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                        new LogPacket(_requestPacket, logMsg)
                    );
                    return;

                }
                return;
            } else {
                mutexMap.find(foundData)->second->lock(); //뮤텍스 설정

                if(!((DynamicHashMap*)(foundData))->insertKeyValue(cmdVec[2], cmdVec[3])) {

                    mutexMap.find(foundData)->second->unlock(); //뮤텍스 해제

                    result = "failed to insert data";
                    exceptError(_requestPacket, result);
                    return;

                } else {
                    mutexMap.find(foundData)->second->unlock(); //뮤텍스 해제
                    result = "complete";
                    logMsg = "insert " + cmdVec[2] + " -> " + cmdVec[3] + " in " + cmdVec[1];

                    this->queueAdapter.lock()->pushInOutputQueue(
                        new RecvMsgPacket(_requestPacket, result)
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                        new SignalPacket(_requestPacket,SIGNALTYPE_RECVEND)
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                        new LogPacket(_requestPacket, logMsg)
                    );
                    return;
                }
                return;
            }
            return;
        }
        break;

        case STRUCTTYPE_ONESET:
        case STRUCTTYPE_MULTISET:
        case STRUCTTYPE_STATICLIST:
        case STRUCTTYPE_DYNAMICLIST:
        {
            //인자값 설정
            if( cmdVec.size() != 3) {
                result = "insert [key] [new-key] ";
                exceptError(_requestPacket, result);
                return;
            }
            if(foundData->getStructType() == STRUCTTYPE_ONESET) {
                //데이터타입 불일치 시 Throw DataTypeException
                try {

                    mutexMap.find(foundData)->second->lock(); //뮤텍스 설정

                    if(!((OneSet*)(foundData))->insertValue(cmdVec[2])) {

                        mutexMap.find(foundData)->second->unlock(); //뮤텍스 해제
                        result = "This Value is Aleady Exist" ;
                        exceptError(_requestPacket, result);
                        return;

                    } else {
                        mutexMap.find(foundData)->second->unlock(); //뮤텍스 해제

                        result= "insert compelete";
                        logMsg = "insert " + cmdVec[2] + " in " + cmdVec[1];
                        this->queueAdapter.lock()->pushInOutputQueue(
                            new RecvMsgPacket(_requestPacket, result)
                        );
                        this->queueAdapter.lock()->pushInOutputQueue(
                            new SignalPacket(_requestPacket,SIGNALTYPE_RECVEND)
                        );
                        this->queueAdapter.lock()->pushInOutputQueue(
                            new LogPacket(_requestPacket, logMsg)
                        );
                        return;
                    }
                } catch (DataTypeException& e) {

                    mutexMap.find(foundData)->second->unlock(); //뮤텍스 해제
                    result = e.getErrorMsg();
                    exceptError(_requestPacket, result);
                    return;
                }
                return;

            } else if(foundData->getStructType() == STRUCTTYPE_MULTISET) {

                try {
                    mutexMap.find(foundData)->second->lock(); //뮤텍스 설정

                    if(!((MultiSet*)(foundData))->insertValue(cmdVec[2])) {

                        mutexMap.find(foundData)->second->unlock(); //뮤텍스 설정
                        result = "This Value is Aleady Exist";
                        exceptError(_requestPacket, result);
                        return;
                    } else {
                        mutexMap.find(foundData)->second->unlock(); //뮤텍스 설정

                        result= "insert compelete";
                        logMsg = "insert " + cmdVec[2] + " in " + cmdVec[1];
                        this->queueAdapter.lock()->pushInOutputQueue(
                            new RecvMsgPacket(_requestPacket, result)
                        );
                        this->queueAdapter.lock()->pushInOutputQueue(
                            new SignalPacket(_requestPacket,SIGNALTYPE_RECVEND)
                        );
                        this->queueAdapter.lock()->pushInOutputQueue(
                            new LogPacket(_requestPacket, logMsg)
                        );
                        return;
                    }
                } catch(DataTypeException& e) {

                    mutexMap.find(foundData)->second->unlock(); //뮤텍스 해제
                    result = e.getErrorMsg();
                    exceptError(_requestPacket, result);
                    return;
                }
                return;
            } else if(foundData->getStructType() == STRUCTTYPE_STATICLIST) {
                 try {
                    
                    mutexMap.find(foundData)->second->lock(); //뮤텍스 설정
                    
                    if(!((StaticList*)(foundData))->insertValue(cmdVec[2])) {

                        mutexMap.find(foundData)->second->unlock(); //뮤텍스 설정
                        result =  "This Value is Aleady Exist";
                        exceptError(_requestPacket, result);
                        return;

                    } else {
                        mutexMap.find(foundData)->second->unlock(); //뮤텍스 설정

                        result= "insert compelete";
                        logMsg = "insert " + cmdVec[2] + " in " + cmdVec[1];
                        this->queueAdapter.lock()->pushInOutputQueue(
                            new RecvMsgPacket(_requestPacket, result)
                        );
                        this->queueAdapter.lock()->pushInOutputQueue(
                            new SignalPacket(_requestPacket,SIGNALTYPE_RECVEND)
                        );
                        this->queueAdapter.lock()->pushInOutputQueue(
                            new LogPacket(_requestPacket, logMsg)
                        );
                        return;
                    }
                 } catch(DataTypeException& e) {

                     mutexMap.find(foundData)->second->unlock(); //뮤텍스 해제
                    result = e.getErrorMsg();
                    exceptError(_requestPacket, result);
                    return;
                    
                 }
                 return;
            } else { //DynamicList
                try {
                    mutexMap.find(foundData)->second->lock(); //뮤텍스 설정

                    if(!((DynamicList*)(foundData))->insertValue(cmdVec[2])) {

                        mutexMap.find(foundData)->second->unlock(); //뮤텍스 설정
                        result =  "This Value is Aleady Exist";
                        exceptError(_requestPacket, result);
                        return;

                    } else {
                        mutexMap.find(foundData)->second->unlock(); 
                        result= "insert compelete";
                        logMsg = "insert " + cmdVec[2] + " in " + cmdVec[1];
                        this->queueAdapter.lock()->pushInOutputQueue(
                            new RecvMsgPacket(_requestPacket, result)
                        );
                        this->queueAdapter.lock()->pushInOutputQueue(
                            new SignalPacket(_requestPacket,SIGNALTYPE_RECVEND)
                        );
                        this->queueAdapter.lock()->pushInOutputQueue(
                            new LogPacket(_requestPacket, logMsg)
                        );
                        return;
                    }
                } catch(DataTypeException& e) {

                    mutexMap.find(foundData)->second->unlock(); //뮤텍스 해제
                    result = e.getErrorMsg();
                    exceptError(_requestPacket, result);
                    return;
                    
                }
            }
        }
        break;

        default:
            result = "System Error! : Element Data is on Super Location";
            exceptError(_requestPacket, result);
            return;
        break;
    }
}

//데이터 탐색
void SKVS_DataBase::DataBase::get(SendCmdPacket& _requestPacket) {

    this->queueAdapter.lock()->pushInOutputQueue(
        new SignalPacket(_requestPacket, SIGNALTYPE_RECVSTART )
    );

    vector<string> cmdVec = _requestPacket.getCmdArray();
    // Nomal : get [key] [condition] (Ex over 10 and eunder 20 )
    // List Also can : get <-i> [key] [indexRange] (Ex 1-3)

    //1. vector 갯수 확인 (최소 3개 이상)
    //1-5. 키 탐색
    //2. -i 플래그 확인과 동시에 타입이 List인 지 확인
    //              확인이 될 경우 그 상태에서 바로 출력
    //3. 인덱스 3번 부터 끝까지 합치기
    //4. 실행

    string result;
    string logMsg;

    if( cmdVec.size() < 2 ) {
        result =  "get [key] <-i> <condition> ";
        exceptError(_requestPacket, result);
        return;
    }

    DataElement* foundData = nullptr;
    //인덱스 범위 검색
    if( cmdVec[1].compare("-i") == 0 ) {

        //반드시 4개여야 함
        if( cmdVec.size() != 4 ) {
            result = "get [key] [-i] [minIndex-maxIndex]";
            exceptError(_requestPacket, result);
            return;
        }
        //탐색 알고리즘 <맨 위 참조>

        //검색 대상 키 유효성 판단
        if(!tok::IsAllowedCharacter(cmdVec[2], R"(~!@#$%^&*()_+=[-];'/,>{}:"<?")")) {

            result = R"(Invalid Key-name list = ~!@#$%^&*()_+-=[];'/,>{}:"<?)";
            exceptError(_requestPacket, result);

            return;
        }

        //.으로 되어있는 경우, .트로 토큰화를 나누고
        // .갯수에 따라 하위 데이터로 이동

        foundData = findDataByRoot(cmdVec[2], result);

        //데이터 검색

        //데이터 못찾음
        if( foundData == nullptr ) {
            exceptError(_requestPacket, result);
            return;
        }

        //타입이 List 계열인 지 확인
        StructType dataType = foundData->getStructType();
        if( dataType != STRUCTTYPE_STATICLIST && dataType != STRUCTTYPE_DYNAMICLIST ) {

            result = "This Data StructType is not list. but you try to search to Index Range.";

            this->queueAdapter.lock()->pushInOutputQueue(
                new RecvMsgPacket(_requestPacket, result)
            );
            this->queueAdapter.lock()->pushInOutputQueue(
                new SignalPacket(_requestPacket, SIGNALTYPE_RECVEND)
            );
            return;
        }

        //인덱스 하나만 검색할 경우
        vector<string> checkHipen = tok::tokenizer(cmdVec[3], '-');
        if(checkHipen.size() == 1) {
            cmdVec[3].clear();
            cmdVec[3] += checkHipen[0];
            cmdVec[3] += "-";
            cmdVec[3] += checkHipen[0];
        }

        //Static과 Dynamic 계열 판정
        if( dataType == STRUCTTYPE_STATICLIST) {

            //출력
            try {
                mutexMap.find(foundData)->second->lock();
                //Condition 오류 발생 시 Exception 호출
                list<DataElement> resultList = ((StaticList*)(foundData))->searchIndex(cmdVec[3]);
                mutexMap.find(foundData)->second->unlock();

                //출력 인터페이스
                string iFace = cmdVec[2] + " | " + "Static List" + "\n" + 
                                "total : " + to_string(resultList.size()) + "\n" +
                                "-------------------------------------\n";

                //인터페이스 출력               
                this->queueAdapter.lock()->pushInOutputQueue(
                    new RecvMsgPacket(_requestPacket, iFace)
                );
                for(list<DataElement>::iterator iter  = resultList.begin(); 
                    iter != resultList.end(); iter++ ) { 
                        
                    //데이터 하나씩 전송
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new RecvDataPacket(_requestPacket, *iter )
                    );
                }
                //끝내기           
                this->queueAdapter.lock()->pushInOutputQueue(
                    new SignalPacket(_requestPacket, SIGNALTYPE_RECVEND)
                );
                return; //end

            } catch(DataConditionException& e) {
                
                result = e.getErrorMsg();
                this->queueAdapter.lock()->pushInOutputQueue(
                    new RecvMsgPacket(_requestPacket, result)
                );
                this->queueAdapter.lock()->pushInOutputQueue(
                    new SignalPacket(_requestPacket, SIGNALTYPE_ERROR)
                );
                
                return;
            }
        //DynamicList
        } else {
            try {
                mutexMap.find(foundData)->second->lock();
                list<DataElement> resultList = ((DynamicList*)(foundData))->searchIndex(cmdVec[3]);
                mutexMap.find(foundData)->second->unlock();

                //출력 인터페이스
                string iFace = cmdVec[2] + " | " + "Dynamic List" + "\n" + 
                                "total : " + to_string(resultList.size()) + "\n" +
                                "-------------------------------------\n";

                //인터페이스 출력               
                this->queueAdapter.lock()->pushInOutputQueue(
                    new RecvMsgPacket(_requestPacket, iFace)
                );
                for(list<DataElement>::iterator iter  = resultList.begin(); 
                    iter != resultList.end(); iter++ ) { 
                        
                    //데이터 하나씩 전송
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new RecvDataPacket(_requestPacket, *iter )
                    );
                }
                //끝내기           
                this->queueAdapter.lock()->pushInOutputQueue(
                    new SignalPacket(_requestPacket, SIGNALTYPE_RECVEND)
                );
                return; //end

            } catch(DataConditionException& e) {
                
                result = e.getErrorMsg();
                this->queueAdapter.lock()->pushInOutputQueue(
                    new RecvMsgPacket(_requestPacket, result)
                );
                this->queueAdapter.lock()->pushInOutputQueue(
                    new SignalPacket(_requestPacket, SIGNALTYPE_ERROR)
                );
                
                return;
            }
        }

    } else { //Condition 범위 검색
        // get [key] [condition]
        
        //검색 대상 키 유효성 판단
        if(!tok::IsAllowedCharacter(cmdVec[1], R"(~!@#$%^&*()_+=[-];'/,>{}:"<?")")) {

            result = R"(Invalid Key-name list = ~!@#$%^&*()_+-=[];'/,>{}:"<?)";
            exceptError(_requestPacket, result);

            return;
        }

        //키 검색
        foundData = findDataByRoot(cmdVec[1], result);

        if( foundData == nullptr ) {
            exceptError(_requestPacket, result);
            return;
        }

        //Conditon 토큰으로 나눠진거 다시 붙이기
        string conditionStr = "";
        if( cmdVec.size() >= 3 ) {
            conditionStr = cmdVec[2];
            for(int i = 3; i < cmdVec.size(); i++) {
                conditionStr += " ";
                conditionStr += cmdVec[i];
            }
        }

        //Condition 객체 생성
        ScanDataCondition* condition = nullptr;
        try {
            condition = new ScanDataCondition(conditionStr);
        } catch(DataException& e) {

            result = e.getErrorMsg();
            exceptError(_requestPacket, result);
            return;
        }

        if( !condition->isEmpty() && foundData->getStructType() == STRUCTTYPE_BASIC ) {
            
            result = "Basic can't search with condition";
            exceptError(_requestPacket, result);
            delete condition;
            return;
        }

        StructType targetStructType = foundData->getStructType();

        //출력 인터페이스
        string firstIFace = cmdVec[1] + " | " + convertStructTypeToString(targetStructType);
        this->queueAdapter.lock()->pushInOutputQueue(
            new RecvMsgPacket(_requestPacket, firstIFace)
        );
        //list<DataElement>* resultList = nullptr; //결과데이터를 담을 리스트
        // operator= 알고리즘에 문제가 생긴 상태로 사용 불가

        switch(targetStructType) {
            case STRUCTTYPE_BASIC: //Basic은 따로 처리
            {   
                
                mutexMap.find(foundData)->second->lock();
                Basic* foundBasic = (Basic*)(foundData);
                mutexMap.find(foundData)->second->unlock();

                firstIFace = "-------------------------------------";

                this->queueAdapter.lock()->pushInOutputQueue(
                    new RecvMsgPacket(_requestPacket, firstIFace)
                );

                //데이터 출력
                DataElement value = foundBasic->getValue();

                this->queueAdapter.lock()->pushInOutputQueue(
                    new RecvDataPacket(_requestPacket, value)
                );

                this->queueAdapter.lock()->pushInOutputQueue(
                    new SignalPacket(_requestPacket, SIGNALTYPE_RECVEND)
                );
                delete condition;         
                return;
            }
            break;

            case STRUCTTYPE_ONESET:
            case STRUCTTYPE_MULTISET:
            {
                mutexMap.find(foundData)->second->lock();
                list<DataElement> resultList = ((Set*)(foundData))->searchRange(*condition);
                mutexMap.find(foundData)->second->unlock();
                //resultList = &tempList;
                
                firstIFace = "Total Size : " + to_string(resultList.size()) + "\n" + "-------------------------------------";
 
                this->queueAdapter.lock()->pushInOutputQueue(
                    new RecvMsgPacket(_requestPacket, firstIFace)
                );
                
                for(list<DataElement>::iterator iter = resultList.begin(); 
                        iter != resultList.end(); iter++ ) {

                    this->queueAdapter.lock()->pushInOutputQueue(
                        new RecvDataPacket(_requestPacket, *iter)
                    );
                }
                
            }
            break;
            case STRUCTTYPE_STATICLIST:
            {
                mutexMap.find(foundData)->second->lock();
                list<DataElement> resultList = ((StaticList*)(foundData))->searchRange(*condition);
                mutexMap.find(foundData)->second->unlock();

                firstIFace = "Total Size : " + to_string(resultList.size()) + "\n" + "-------------------------------------";
 
                this->queueAdapter.lock()->pushInOutputQueue(
                    new RecvMsgPacket(_requestPacket, firstIFace)
                );
                
                for(list<DataElement>::iterator iter = resultList.begin(); 
                        iter != resultList.end(); iter++ ) {

                    this->queueAdapter.lock()->pushInOutputQueue(
                        new RecvDataPacket(_requestPacket, *iter)
                    );
                }
            }
            break;

            case STRUCTTYPE_DYNAMICLIST:
            {
                mutexMap.find(foundData)->second->lock();
                list<DataElement> resultList = ((DynamicList*)(foundData))->searchRange(*condition);
                mutexMap.find(foundData)->second->unlock();

                firstIFace = "Total Size : " + to_string(resultList.size()) + "\n" + "-------------------------------------";
 
                this->queueAdapter.lock()->pushInOutputQueue(
                    new RecvMsgPacket(_requestPacket, firstIFace)
                );
                
                for(list<DataElement>::iterator iter = resultList.begin(); 
                        iter != resultList.end(); iter++ ) {
                    
                    this->queueAdapter.lock()->pushInOutputQueue(
                        new RecvDataPacket(_requestPacket, *iter)
                    );
                }
            }
            break;

            case STRUCTTYPE_STATICHASHMAP:
            {
                mutexMap.find(foundData)->second->lock();
                list<DataElement> resultList = ((StaticHashMap*)(foundData))->searchRange(*condition);
                mutexMap.find(foundData)->second->unlock();

                firstIFace = "Total Size : " + to_string(resultList.size()) + "\n" + "-------------------------------------";
 
                this->queueAdapter.lock()->pushInOutputQueue(
                    new RecvMsgPacket(_requestPacket, firstIFace)
                );
                
                for(list<DataElement>::iterator iter = resultList.begin(); 
                        iter != resultList.end(); iter++ ) {

                    this->queueAdapter.lock()->pushInOutputQueue(
                        new RecvDataPacket(_requestPacket, *iter)
                    );
                }
            }            
            break;

            case STRUCTTYPE_DYNAMICHASHMAP:
            {
                mutexMap.find(foundData)->second->lock();
                list<DataElement> resultList = ((DynamicHashMap*)(foundData))->searchRange(*condition);
                mutexMap.find(foundData)->second->unlock();

                firstIFace = "Total Size : " + to_string(resultList.size()) + "\n" + "-------------------------------------";
 
                this->queueAdapter.lock()->pushInOutputQueue(
                    new RecvMsgPacket(_requestPacket, firstIFace)
                );
                
                for(list<DataElement>::iterator iter = resultList.begin(); 
                        iter != resultList.end(); iter++ ) {

                    this->queueAdapter.lock()->pushInOutputQueue(
                        new RecvDataPacket(_requestPacket, *iter)
                    );
                }
            }
            break;

            default:
                result = "System Error! : Get->basic is selected!";
                exceptError(_requestPacket, result);
                return;
            break;

        }
        //결과데이터 프린트
        /* operator=문제로 사용 불가
        cout << "Total Size : " << resultList->size() << endl;
        cout << "-------------------------------------" << endl;
        if( resultList->empty() ) {
            cout << "empty" << endl;
        } else {
            for(list<DataElement>::iterator iter = resultList->begin(); 
                iter != resultList->end(); iter++ ) {
                cout << iter->getDataToString() << "\t\t" << 
                    iter->getDataType() << "\t\t" << 
                    iter->getStructType() << "\t\t" << endl;
            }
        }
        */

       //마무리
       this->queueAdapter.lock()->pushInOutputQueue(
            new SignalPacket(_requestPacket, SIGNALTYPE_RECVEND)
        );
        
        //로그 작성
        if( cmdVec[1].compare("-i") == 0) {
            logMsg = "search key " + cmdVec[2] + " with index";
        } else {
            logMsg = "search key " + cmdVec[1];
        }

        this->queueAdapter.lock()->pushInOutputQueue(
            new LogPacket(_requestPacket, logMsg)
        );

        delete condition;
        return;
    }

}

//값 수정
void SKVS_DataBase::DataBase::set(SendCmdPacket& _requestPacket) {

    this->queueAdapter.lock()->pushInOutputQueue(
        new SignalPacket(_requestPacket, SIGNALTYPE_RECVSTART )
    );

    string result;
    string logMsg;

    vector<string> cmdVec = _requestPacket.getCmdArray();

        //리스트와 나머지 구조체로 분류
    if( cmdVec.size() != 4 && cmdVec.size() != 5 ) {
        result = "set <-i> [key] [value or index] [new value]";
        exceptError(_requestPacket, result);
        return;
    }

    DataElement* foundData = nullptr;

    // -i 여부 판단
    if( cmdVec[1].compare("-i") == 0) {
        
        //set [-i] [key] [index] [new value]
        //인자값 판단.

        if(cmdVec.size() != 5) {
            result = "set [-i] [key] [index] [new value]";
            exceptError(_requestPacket, result);
            return;
        }
        
        //키 판정
        if(!tok::IsAllowedCharacter(cmdVec[2], R"(~!@#$%^&.*()_+-=[];'/,>{}:"<?)")) {
            result = R"(Invalid Key-name list = ~!@#$%^&*()_+-=[];'/,>{}:"<?)";
            exceptError(_requestPacket, result);
            return;
        }

        //값 판정
        if(!tok::IsAllowedCharacter(cmdVec[4], R"(~!@#$%^&.*()_+-.=[];'/,>{}:"<?)")) {
            result =  R"(Invalid Key-name list = ~!@#$%^&*()_+-=[.];'/,>{}:"<?)";
            exceptError(_requestPacket, result);
            return;
        }

    
        //검색
        foundData = findDataByRoot(cmdVec[2], result);
        if( foundData == nullptr) {
            exceptError(_requestPacket, result);
            return;
        }
        //인덱스 판정
        CheckDataType checkDataType;

        //해당 구조체가 리스트인 지 판정
        if( foundData->getStructType() != STRUCTTYPE_STATICLIST && 
            foundData->getStructType() != STRUCTTYPE_DYNAMICLIST) {
            result = "This is Not list, but you try to set data by index";
            exceptError(_requestPacket, result);
            return;
        }


        if( !checkDataType(cmdVec[3], DATATYPE_NUMBER)) {
            result = "This index must be number";
            exceptError(_requestPacket, result);
            return;
        }

        int idx = atoi(cmdVec[3].c_str());
        if( idx < 0) {
            result = "index must not be minus";
            exceptError(_requestPacket, result);
            return;
        }
        //값 수정하기
        if( foundData->getStructType() == STRUCTTYPE_STATICLIST) {

            mutexMap.find(foundData)->second->lock();

            if(!((StaticList*)(foundData))->updateDataByIndex(idx, cmdVec[4]) ) {

                mutexMap.find(foundData)->second->unlock();
                result =  "Failed to update";
                exceptError(_requestPacket, result);
            } else {
                mutexMap.find(foundData)->second->unlock();
                result = "complete";
                logMsg = "In key " +  cmdVec[2] + ", Data in index " + to_string(idx) + " is change to " + cmdVec[4];

                this->queueAdapter.lock()->pushInOutputQueue(
                        new RecvMsgPacket(_requestPacket, result)
                );
                this->queueAdapter.lock()->pushInOutputQueue(
                        new SignalPacket(_requestPacket, SIGNALTYPE_RECVEND)
                );
                this->queueAdapter.lock()->pushInOutputQueue(
                        new LogPacket(_requestPacket, logMsg)
                );
            }
            return;
        } else if(foundData->getStructType()==STRUCTTYPE_DYNAMICLIST) {

            mutexMap.find(foundData)->second->lock();

            if(!((DynamicList*)(foundData))->updateDataByIndex(idx, cmdVec[4])) {

                mutexMap.find(foundData)->second->unlock();

                result =  "Failed to update";
                exceptError(_requestPacket, result);
            } else {
                mutexMap.find(foundData)->second->unlock();

                result = "complete";
                logMsg = "In key " +  cmdVec[2] + ", Data in index " + to_string(idx) + " is change to " + cmdVec[4];

                this->queueAdapter.lock()->pushInOutputQueue(
                        new RecvMsgPacket(_requestPacket, result)
                );
                this->queueAdapter.lock()->pushInOutputQueue(
                        new SignalPacket(_requestPacket, SIGNALTYPE_RECVEND)
                );
                this->queueAdapter.lock()->pushInOutputQueue(
                        new LogPacket(_requestPacket, logMsg)
                );
            }
            return;
        }
    } else {

        //set [key] [value(or key in hashmap)] [new value]
        if(cmdVec.size() != 4) {
            result =  "set [key] [value(or key in hashmap)] [new value]";
            exceptError(_requestPacket, result);
            return;
        }

        //값에 대한 유효성 판정
        if(!tok::IsAllowedCharacter(cmdVec[2], R"(~!@#$%^&.*()_+-=[];'/,>{}:"<?)")) {
            result =  R"(Invalid Key-name list = ~!@#$%^&*()_+-=[];.'/,>{}:"<?)";
            exceptError(_requestPacket, result);
            return;
        }
        if(!tok::IsAllowedCharacter(cmdVec[3], R"(~!@#$%^&.*()_+-=[];'/,>{}:"<?)")) {
            result = R"(Invalid Key-name list = ~!@#$%^&*()_+-=[];.'/,>{}:"<?)";
            exceptError(_requestPacket, result);
            return;
        }

        //데이터 검색
        foundData = findDataByRoot(cmdVec[1], result);
            if( foundData == nullptr) {
            exceptError(_requestPacket, result);
            return;
        }

        //데이터 수정 시작
        switch(foundData->getStructType()) {
            case STRUCTTYPE_STATICLIST:
                mutexMap.find(foundData)->second->lock();

                if(((StaticList*)(foundData))->updateData(cmdVec[2], cmdVec[3])) {

                    mutexMap.find(foundData)->second->unlock();

                    result = "complete";
                    logMsg = "In key " +  cmdVec[1] + ", value " + cmdVec[2] + " is change to " + cmdVec[3];

                    this->queueAdapter.lock()->pushInOutputQueue(
                            new RecvMsgPacket(_requestPacket, result)
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new SignalPacket(_requestPacket, SIGNALTYPE_RECVEND)
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new LogPacket(_requestPacket, logMsg)
                    );
                } else {
                    mutexMap.find(foundData)->second->unlock();
                    result = "faild to update value";
                    exceptError(_requestPacket, result);
                }
                return;
            break;
            case STRUCTTYPE_DYNAMICLIST:

                mutexMap.find(foundData)->second->lock();

                if(((DynamicList*)(foundData))->updateData(cmdVec[2], cmdVec[3])) {
                    mutexMap.find(foundData)->second->unlock();
                    result = "complete";
                    logMsg = "In key " +  cmdVec[1] + ", value " + cmdVec[2] + " is change to " + cmdVec[3];

                    this->queueAdapter.lock()->pushInOutputQueue(
                            new RecvMsgPacket(_requestPacket, result)
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new SignalPacket(_requestPacket, SIGNALTYPE_RECVEND)
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new LogPacket(_requestPacket, logMsg)
                    );
                } else {
                    mutexMap.find(foundData)->second->unlock();
                    result = "faild to update value";
                    exceptError(_requestPacket, result);
                }
                return;
            break;
            case STRUCTTYPE_STATICHASHMAP:
                mutexMap.find(foundData)->second->lock();
                if(((StaticHashMap*)(foundData))->updateValue(cmdVec[2], cmdVec[3])) {
                    mutexMap.find(foundData)->second->unlock();
                    result = "complete";
                    logMsg = "In key " +  cmdVec[1] + ", value " + cmdVec[2] + " is change to " + cmdVec[3];

                    this->queueAdapter.lock()->pushInOutputQueue(
                            new RecvMsgPacket(_requestPacket, result)
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new SignalPacket(_requestPacket, SIGNALTYPE_RECVEND)
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new LogPacket(_requestPacket, logMsg)
                    );
                } else {
                    mutexMap.find(foundData)->second->unlock();
                    result = "faild to update value";
                    exceptError(_requestPacket, result);
                }
                return;
            break;
            case STRUCTTYPE_DYNAMICHASHMAP:
                mutexMap.find(foundData)->second->lock();
                if(((DynamicHashMap*)(foundData))->updateValue(cmdVec[2], cmdVec[3])) {
                    mutexMap.find(foundData)->second->unlock();
                    result = "complete";
                    logMsg = "In key " +  cmdVec[1] + ", value " + cmdVec[2] + " is change to " + cmdVec[3];

                    this->queueAdapter.lock()->pushInOutputQueue(
                            new RecvMsgPacket(_requestPacket, result)
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new SignalPacket(_requestPacket, SIGNALTYPE_RECVEND)
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new LogPacket(_requestPacket, logMsg)
                    );
                } else {
                    mutexMap.find(foundData)->second->unlock();
                    result = "faild to update value";
                    exceptError(_requestPacket, result);
                }
                return;
            break;

            //이구간은 실행불가능
            case STRUCTTYPE_ONESET:
            case STRUCTTYPE_MULTISET:
                result = "Set can't update data";
                exceptError(_requestPacket, result);
            break;

            case STRUCTTYPE_BASIC:
                result = "Set can't update data";
                exceptError(_requestPacket, result);
            break;
        }
    } 
}

//데이터 삭제
void SKVS_DataBase::DataBase::_delete(SendCmdPacket& _requestPacket) {
    this->queueAdapter.lock()->pushInOutputQueue(
        new SignalPacket(_requestPacket, SIGNALTYPE_RECVSTART )
    );

    string result;
    string logMsg;

    vector<string> cmdVec = _requestPacket.getCmdArray();

    //delete [key] [condition]
    //delete <-i> [key] [index-condition]

    if(cmdVec.size() < 3) {
        result = "delete <-i> [key] [index-range]";
        exceptError(_requestPacket, result);
        return;
    }
    DataElement* foundData = nullptr;
    if(cmdVec[1].compare("-i") == 0) {

        foundData = findDataByRoot(cmdVec[2], result);
        if(foundData == nullptr) {
            exceptError(_requestPacket, result);
            return;
        }

        //타입이 List 계열인 지 확인
        StructType dataType = foundData->getStructType();
        if( dataType != STRUCTTYPE_STATICLIST && dataType != STRUCTTYPE_DYNAMICLIST ) {
            result = "This Data StructType is not list. but you try to search to Index Range.";
            exceptError(_requestPacket, result);
            return;
        }

        //인덱스 하나만 삭제할 경우
        vector<string> checkHipen = tok::tokenizer(cmdVec[3], '-');
        if(checkHipen.size() == 1) {
            cmdVec[3].clear();
            cmdVec[3] += checkHipen[0];
            cmdVec[3] += "-";
            cmdVec[3] += checkHipen[0];
        }

        //삭제 시작
        if( dataType == STRUCTTYPE_STATICLIST) {
            try {
                //얘는 리턴값이 true밖에 없음
                mutexMap.find(foundData)->second->lock();
                ((StaticList*)(foundData))->deleteIndex(cmdVec[3]);
                mutexMap.find(foundData)->second->unlock();

                result = "compelete";
                logMsg = "Delete range : " + cmdVec[3] + " In key " + cmdVec[2];
                this->queueAdapter.lock()->pushInOutputQueue(
                        new RecvMsgPacket(_requestPacket, result)
                );
                this->queueAdapter.lock()->pushInOutputQueue(
                        new SignalPacket(_requestPacket,SIGNALTYPE_RECVEND)
                );
                this->queueAdapter.lock()->pushInOutputQueue(
                        new LogPacket(_requestPacket, logMsg)
                );
            } catch(DataConditionException& e) {

                mutexMap.find(foundData)->second->unlock();
                result = e.getErrorMsg();
                exceptError(_requestPacket, result);
            }
            return;
        } else { // STRUCTTYPE_DYANMICLIST
            try {
                mutexMap.find(foundData)->second->lock();
                ((DynamicList*)(foundData))->deleteIndex(cmdVec[3]);
                mutexMap.find(foundData)->second->unlock();
                
                result = "compelete";
                logMsg = "Delete range : " + cmdVec[3] + " In key " + cmdVec[2];
                this->queueAdapter.lock()->pushInOutputQueue(
                        new RecvMsgPacket(_requestPacket, result)
                );
                this->queueAdapter.lock()->pushInOutputQueue(
                        new SignalPacket(_requestPacket,SIGNALTYPE_RECVEND)
                );
                this->queueAdapter.lock()->pushInOutputQueue(
                        new LogPacket(_requestPacket, logMsg)
                );

            } catch(DataConditionException& e) {

                mutexMap.find(foundData)->second->unlock();
                result = e.getErrorMsg();
                exceptError(_requestPacket, result);
            }
            return;
        }
    } else {

        foundData = findDataByRoot(cmdVec[1], result);

        if(foundData == nullptr) {
            exceptError(_requestPacket, result);
            return;
        }

        //Basic은 delete 불가능
        if( foundData->getStructType() == STRUCTTYPE_BASIC) {   
            result = "Basic can't run delete, only drop";
            exceptError(_requestPacket, result);
            return;
        }

        ScanDataCondition* condition = nullptr;

        string conditionStr = "";
        if( cmdVec.size() >= 3 ) {
            conditionStr = cmdVec[2];
            for(int i = 3; i < cmdVec.size(); i++) {
                conditionStr += " ";
                conditionStr += cmdVec[i];
            }
        }

        try {
            condition = new ScanDataCondition(conditionStr);
        } catch (DataException& e) {
            result = e.getErrorMsg();
            exceptError(_requestPacket, result);
            return;
        }

        //구조체에 따른 컨디션 수행
        switch(foundData->getStructType()) {
            case STRUCTTYPE_ONESET:
            case STRUCTTYPE_MULTISET:

                mutexMap.find(foundData)->second->lock();
                if(((Set*)(foundData))->deleteRange(*condition)) {
                    mutexMap.find(foundData)->second->unlock();

                    result = "compelete";
                    logMsg = "Delete range : " + conditionStr+ " In key " + cmdVec[1];
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new RecvMsgPacket(_requestPacket, result)
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new SignalPacket(_requestPacket,SIGNALTYPE_RECVEND)
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new LogPacket(_requestPacket, logMsg)
                    );

                } else {

                    mutexMap.find(foundData)->second->unlock();
                    result = "failed to delete";
                    exceptError(_requestPacket, result);
                }
                delete condition;
                return;
            break;

            case STRUCTTYPE_STATICLIST:

                mutexMap.find(foundData)->second->lock();
                if(((StaticList*)(foundData))->deleteRange(*condition)) {
                    mutexMap.find(foundData)->second->unlock();
                    
                    result = "compelete";
                    logMsg = "Delete range : " + conditionStr + " In key " + cmdVec[1];
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new RecvMsgPacket(_requestPacket, result)
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new SignalPacket(_requestPacket,SIGNALTYPE_RECVEND)
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new LogPacket(_requestPacket, logMsg)
                    );

                }
                else {
                    mutexMap.find(foundData)->second->unlock();
                    result = "failed to delete";
                    exceptError(_requestPacket, result);
                    
                }
                delete condition;
                return;
            break;

            case STRUCTTYPE_DYNAMICLIST:

                mutexMap.find(foundData)->second->lock();
                if(((DynamicList*)(foundData))->deleteRange(*condition)) {
                    mutexMap.find(foundData)->second->unlock();
                    
                    result = "compelete";
                    logMsg = "Delete range : " + conditionStr + " In key " + cmdVec[1];
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new RecvMsgPacket(_requestPacket, result)
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new SignalPacket(_requestPacket,SIGNALTYPE_RECVEND)
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new LogPacket(_requestPacket, logMsg)
                    );
                }
                else {
                    mutexMap.find(foundData)->second->unlock();
                    result = "failed to delete";
                    exceptError(_requestPacket, result);
                    
                }
                delete condition;
                return;
            break;

            case STRUCTTYPE_STATICHASHMAP:

                mutexMap.find(foundData)->second->lock();
                if(((StaticHashMap*)(foundData))->deleteRange(*condition)) {
                    mutexMap.find(foundData)->second->unlock();
                    
                    result = "compelete";
                    logMsg = "Delete range : " + conditionStr + " In key " + cmdVec[1];
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new RecvMsgPacket(_requestPacket, result)
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new SignalPacket(_requestPacket,SIGNALTYPE_RECVEND)
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new LogPacket(_requestPacket, logMsg)
                    );
                }
                else {
                    mutexMap.find(foundData)->second->unlock();
                    result = "failed to delete";
                    exceptError(_requestPacket, result);
                    
                }
                delete condition;
                return;
            break;

            case STRUCTTYPE_DYNAMICHASHMAP:
                //cout << "test" << endl;

                mutexMap.find(foundData)->second->lock();
                if(((DynamicHashMap*)(foundData))->deleteRange(*condition)) {
                    mutexMap.find(foundData)->second->unlock();
                           
                    result = "compelete";
                    logMsg = "Delete range : " + conditionStr + " In key " + cmdVec[1];
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new RecvMsgPacket(_requestPacket, result)
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new SignalPacket(_requestPacket,SIGNALTYPE_RECVEND)
                    );
                    this->queueAdapter.lock()->pushInOutputQueue(
                            new LogPacket(_requestPacket, logMsg)
                    );
                }
                else {
                    mutexMap.find(foundData)->second->unlock();
                    result = "failed to delete";
                    exceptError(_requestPacket, result);
                }
                
                delete condition;
                return;
            break;

            default:
                result = "System Error: invalid structure try to delete";
                exceptError(_requestPacket, result);
                delete condition;
                return;
            break;
        }
    }

}

void SKVS_DataBase::DataBase::link(SendCmdPacket& _requestPacket) {

    this->queueAdapter.lock()->pushInOutputQueue(
        new SignalPacket(_requestPacket, SIGNALTYPE_RECVSTART )
    );

    string result;
    string logMsg;

    vector<string> cmdVec = _requestPacket.getCmdArray();  

    //link [key] [value key] [target_key]
    //길이 판정
    if(cmdVec.size() != 4 && cmdVec.size() != 5) {
        result = "link [key] [value key] [target_key]";
        exceptError(_requestPacket, result);
        return;
    }

    string dataErr01; //부모가 될 데이터 결과
    string dataErr02; //자식이 될 데이터 결과
    //데이터 찾기
    DataElement* foundData = findDataByRoot(cmdVec[1], dataErr01);
    DataElement* foundChildData = findDataByRoot(cmdVec[3], dataErr02);

    //해당 대이터 검색
    if( foundData == nullptr) {
        exceptError(_requestPacket,dataErr01);
        return;
    }

    //해당 키가 Dynamic인지 판정
    if( foundData->getStructType() != STRUCTTYPE_DYNAMICLIST && 
        foundData->getStructType() != STRUCTTYPE_DYNAMICHASHMAP) {
        
        result = "This key is not dynamic";
        exceptError(_requestPacket,result);
        return;
    }

    //하위로 지정할 데이터 검색
    if(foundChildData == nullptr) {
        dataErr02 += "in child data";
        exceptError(_requestPacket,dataErr02);
        return;
    }

    //Basic은 링크 불가
    if(foundChildData->getStructType() == STRUCTTYPE_BASIC) {
        result = "Basic can't use as child data";
        exceptError(_requestPacket,dataErr02);
        return;
    }

    MultiDataStructure* targetData = (MultiDataStructure*)(foundChildData);

    //키 검색
    if( foundData->getStructType() == STRUCTTYPE_DYNAMICLIST ) {
        
        try {

            mutexMap.find(foundData)->second->lock();
            mutexMap.find(foundChildData)->second->lock();

            if(!((DynamicList*)(foundData))->linkChildElement(cmdVec[2], targetData)) {

                mutexMap.find(foundChildData)->second->unlock();
                mutexMap.find(foundData)->second->unlock();

                result = "This Value have aleady link";
                exceptError(_requestPacket,dataErr02);


            } else {
                
                mutexMap.find(foundChildData)->second->unlock();
                mutexMap.find(foundData)->second->unlock();

                result = "compelete";
                logMsg = cmdVec[2] + " connects to " +  cmdVec[3] + " as Parent Data";
                this->queueAdapter.lock()->pushInOutputQueue(
                    new RecvMsgPacket(_requestPacket, result)
                );
                this->queueAdapter.lock()->pushInOutputQueue(
                    new SignalPacket(_requestPacket,SIGNALTYPE_RECVEND)
                );
                this->queueAdapter.lock()->pushInOutputQueue(
                    new LogPacket(_requestPacket, logMsg)
                );
            }
            //child Data가 어딘가에 연결되어있을 경우 
            //DataLinkException 호출
            return;
        } catch( DataLinkException& e) {
            
            mutexMap.find(foundChildData)->second->unlock();
            mutexMap.find(foundData)->second->unlock();

            exceptError(_requestPacket,e.getErrorMsg()); 
            return;
        }
    } else { //dynamichashmap

        try {

            mutexMap.find(foundData)->second->lock();
            mutexMap.find(foundChildData)->second->lock();

            if(!((DynamicHashMap*)(foundData))->linkChildElement(cmdVec[2], targetData)) {
                
                mutexMap.find(foundChildData)->second->unlock();
                mutexMap.find(foundData)->second->unlock();
                

                result = "This Value have aleady link";
                exceptError(_requestPacket,dataErr02);

            } else {
                
                mutexMap.find(foundChildData)->second->unlock();
                mutexMap.find(foundData)->second->unlock();

                result = "compelete";
                logMsg = cmdVec[2] + " connects to " +  cmdVec[3] + " as Parent Data";
                this->queueAdapter.lock()->pushInOutputQueue(
                    new RecvMsgPacket(_requestPacket, result)
                );
                this->queueAdapter.lock()->pushInOutputQueue(
                    new SignalPacket(_requestPacket,SIGNALTYPE_RECVEND)
                );
                this->queueAdapter.lock()->pushInOutputQueue(
                    new LogPacket(_requestPacket, logMsg)
                );

            }
            return;
        } catch(DataException& e) {

            mutexMap.find(foundChildData)->second->unlock();
            mutexMap.find(foundData)->second->unlock();

            exceptError(_requestPacket,e.getErrorMsg()); 

            return;
        }

    }
}

void SKVS_DataBase::DataBase::unlink(SendCmdPacket& _requestPacket) {

    this->queueAdapter.lock()->pushInOutputQueue(
        new SignalPacket(_requestPacket, SIGNALTYPE_RECVSTART )
    );

    string result;
    string logMsg;

    vector<string> cmdVec = _requestPacket.getCmdArray();  

    if( cmdVec.size() != 3) {
        result = "unlink [key] [value to unlink]";
        exceptError(_requestPacket, result);
        return;
    }

    DataElement* foundData = findDataByRoot(cmdVec[1], result);

    if(foundData == nullptr) {
        exceptError(_requestPacket, result);
        return;
    }

    //해당 키가 Dynamic인지 판정
    if( foundData->getStructType() != STRUCTTYPE_DYNAMICLIST && 
        foundData->getStructType() != STRUCTTYPE_DYNAMICHASHMAP) {
        
        result = "This key is not dynamic";
        exceptError(_requestPacket, result);
        return;
    }

    //데이터 링크 해 ㅋ 제 ㅋ
    if( foundData->getStructType() == STRUCTTYPE_DYNAMICLIST ) {
        try {

            mutexMap.find(foundData)->second->lock();

            if( ((DynamicList*)(foundData))->unlinkChildElement(cmdVec[2])) {
                mutexMap.find(foundData)->second->unlock();

                result = "complete";
                logMsg = cmdVec[1] + " and " + cmdVec[2] + "is disconnected";
                this->queueAdapter.lock()->pushInOutputQueue(
                        new RecvMsgPacket(_requestPacket, result)
                );
                this->queueAdapter.lock()->pushInOutputQueue(
                        new SignalPacket(_requestPacket,SIGNALTYPE_RECVEND)
                );
                this->queueAdapter.lock()->pushInOutputQueue(
                        new LogPacket(_requestPacket, logMsg)
                );
            } else {
                mutexMap.find(foundData)->second->unlock();
                result = "This link is already empty";
                exceptError(_requestPacket, result);
            }
            return;
        //키 못찾으면 예외 호출
        } catch(DataLinkException& e) {
            mutexMap.find(foundData)->second->unlock();
            exceptError(_requestPacket, e.getErrorMsg());
            return;
        }
    } else {
        try {
            mutexMap.find(foundData)->second->lock();

            if( ((DynamicHashMap*)(foundData))->unlinkChildElement(cmdVec[2])) {
                result = "complete";
                logMsg = cmdVec[1] + " and " + cmdVec[2] + "is disconnected";
                this->queueAdapter.lock()->pushInOutputQueue(
                        new RecvMsgPacket(_requestPacket, result)
                );
                this->queueAdapter.lock()->pushInOutputQueue(
                        new SignalPacket(_requestPacket,SIGNALTYPE_RECVEND)
                );
                this->queueAdapter.lock()->pushInOutputQueue(
                        new LogPacket(_requestPacket, logMsg)
                );

            } else {
                mutexMap.find(foundData)->second->unlock();
                result = "This link is already empty";
                exceptError(_requestPacket, result);
            }
            return;
        //키 못찾으면 예외 호출
        } catch(DataLinkException& e) {
            mutexMap.find(foundData)->second->unlock();
            exceptError(_requestPacket, e.getErrorMsg());
            return;
        }
    }
}

void SKVS_DataBase::DataBase::getsize(SendCmdPacket& _requestPacket) {

    this->queueAdapter.lock()->pushInOutputQueue(
        new SignalPacket(_requestPacket, SIGNALTYPE_RECVSTART )
    );

    string result;
    string logMsg;

    vector<string> cmdVec = _requestPacket.getCmdArray();  

        //데이터 찾기
    //getsize [key]
    if(cmdVec.size() != 2 ) {
        result = "getsize [key]";
        exceptError(_requestPacket, result);
        return;
    }
    DataElement* foundData = findDataByRoot(cmdVec[1], result);

    if(foundData == nullptr) {
        exceptError(_requestPacket, result);
        return;
    }
    d_size_t totalSize = 0;

    mutexMap.find(foundData)->second->lock();
    switch(foundData->getStructType()) {

        case STRUCTTYPE_BASIC:
            totalSize = ((Basic*)(foundData))->getValueSize();
        break;

        case STRUCTTYPE_STATICLIST:
        case STRUCTTYPE_DYNAMICLIST:
            totalSize = ((List*)(foundData))->getStructSize();
        break;

        case STRUCTTYPE_ONESET:
        case STRUCTTYPE_MULTISET:
            totalSize = ((Set*)(foundData))->getStructSize();
        break;

        case STRUCTTYPE_STATICHASHMAP:
        case STRUCTTYPE_DYNAMICHASHMAP:
            totalSize = ((HashMap*)(foundData))->getStructSize();
        break;

        default:
            result = "System Erorr! : Element try to check size.";
            exceptError(_requestPacket, result);
            mutexMap.find(foundData)->second->unlock();
            return;

    }
    mutexMap.find(foundData)->second->unlock();

    //결과 데이터 생성
    DataElement totalSizeBlock(to_string(totalSize),DATATYPE_NUMBER, STRUCTTYPE_ELEMENT);
    
    result =  cmdVec[1] + " | Total Size [byte]\n-------------------------";
    logMsg = "calulate total size about key : " + cmdVec[1]; 

    this->queueAdapter.lock()->pushInOutputQueue(
            new RecvMsgPacket(_requestPacket, result)
    );
    this->queueAdapter.lock()->pushInOutputQueue(
            new RecvDataPacket(_requestPacket, totalSizeBlock)
    );
    this->queueAdapter.lock()->pushInOutputQueue(
            new SignalPacket(_requestPacket,SIGNALTYPE_RECVEND)
    );
    this->queueAdapter.lock()->pushInOutputQueue(
            new LogPacket(_requestPacket,logMsg)
    );

    return;
}

void SKVS_DataBase::DataBase::getkey(SendCmdPacket& _requestPacket) {

    this->queueAdapter.lock()->pushInOutputQueue(
        new SignalPacket(_requestPacket, SIGNALTYPE_RECVSTART )
    );

    string result;
    string logMsg;

    vector<string> cmdVec = _requestPacket.getCmdArray();  
    //getkey [key]

    if(cmdVec.size() != 2 ) {
        result = "getkey [key]";
        exceptError(_requestPacket, result);
        return;
    }

    //키 검색
    DataElement* foundData = findDataByRoot(cmdVec[1], result);

    if(foundData == nullptr) {
        exceptError(_requestPacket, result);
        return;
    }

    if(foundData->getStructType() == STRUCTTYPE_STATICHASHMAP ||
        foundData->getStructType() == STRUCTTYPE_DYNAMICHASHMAP) {
        

        vector<string> resultKey = ((HashMap*)(foundData))->getKey();

        result = "HashMap : '" + cmdVec[1] + "' key list\n-------------------------";
        this->queueAdapter.lock()->pushInOutputQueue(
            new RecvMsgPacket(_requestPacket, result)
        );

        for(int i = 0; i < resultKey.size(); i++) {
            DataElement keyElement(resultKey[i],DATATYPE_STRING, STRUCTTYPE_ELEMENT);
            this->queueAdapter.lock()->pushInOutputQueue(
                new RecvDataPacket(_requestPacket, keyElement)
            );
        }
        this->queueAdapter.lock()->pushInOutputQueue(
            new SignalPacket(_requestPacket, SIGNALTYPE_RECVEND)
        );
        return;
    } else {
        result = "This key is not hashmap";
        exceptError(_requestPacket, result);
        return;
    }
}

void SKVS_DataBase::DataBase::_list(SendCmdPacket& _requestPacket) {

    this->queueAdapter.lock()->pushInOutputQueue(
        new SignalPacket(_requestPacket, SIGNALTYPE_RECVSTART )
    );

    this->queueAdapter.lock()->pushInOutputQueue(
        new RecvMsgPacket(_requestPacket, "----------- Data Table List ---------"  )
    );
    for(list<DataElement*>::iterator iter = dataBase.begin();
        iter != dataBase.end(); iter++ ) {
        
        this->queueAdapter.lock()->pushInOutputQueue(
            new RecvDataPacket(_requestPacket, **iter  )
        );
    }
    this->queueAdapter.lock()->pushInOutputQueue(
        new SignalPacket(_requestPacket, SIGNALTYPE_RECVEND  )
    );
    return;
}

void SKVS_DataBase::DataBase::runCmd(SendCmdPacket& _requestPacket) {

    if( _requestPacket.getCmdArray()[0].compare(DB_Command::Create) == 0)
        create(_requestPacket);
    else if( _requestPacket.getCmdArray()[0].compare(DB_Command::Drop) == 0)
        drop(_requestPacket);
    else if( _requestPacket.getCmdArray()[0].compare(DB_Command::Insert) == 0)
        insert(_requestPacket);
    else if(_requestPacket.getCmdArray()[0].compare(DB_Command::Get) == 0)
        get(_requestPacket);
    else if(_requestPacket.getCmdArray()[0].compare(DB_Command::Set) == 0)
        set(_requestPacket);
    else if(_requestPacket.getCmdArray()[0].compare(DB_Command::Delete) == 0)
        _delete(_requestPacket);
    else if(_requestPacket.getCmdArray()[0].compare(DB_Command::Link) == 0)
        link(_requestPacket);
    else if(_requestPacket.getCmdArray()[0].compare(DB_Command::UnLink) == 0)
        unlink(_requestPacket);
    else if(_requestPacket.getCmdArray()[0].compare(DB_Command::GetSize) == 0)
        getsize(_requestPacket);
    else if(_requestPacket.getCmdArray()[0].compare(DB_Command::GetKey) == 0)
        getkey(_requestPacket);
    else if(_requestPacket.getCmdArray()[0].compare(DB_Command::List) == 0)
        _list(_requestPacket);
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
        this->queueAdapter.lock()->pushInOutputQueue(
            new RecvMsgPacket(_requestPacket, errorMsg)
        );
        //클라이언트의 해당 커멘드 스레드에 대한 종료 시그널 보내기
        SignalPacket* terminatePacket = new SignalPacket(
            _requestPacket,
            SIGNALTYPE_ERROR
        );
        this->queueAdapter.lock()->pushInOutputQueue(terminatePacket);
    }
}