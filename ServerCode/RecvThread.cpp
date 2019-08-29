#include "../lib/packet/Packet.hpp"
#include "../lib/user/LoginedUser.hpp"
#include "../lib/CommandFilter.hpp"
#include "../lib/user/LoginedUserList.hpp"
#include "../lib/SockWrapper/ServerSocketManager.hpp"
#include "../lib/SockWrapper/NetworkingManager.hpp"
#include "../lib/SockWrapper/SocketManager.hpp"
#include "../lib/threadAdapter/AdapterThreadBridge.hpp"
#include "../lib/CommandList.hpp"
#include "../lib/loader/SystemLoader.hpp"
#include "../lib/loader/AccountLoader.hpp"
#include "../lib/Tokenizer.hpp"
#include "../lib/packet/SkvsProtocol.hpp"

#include <string>
#include <deque>
#include <queue>
#include <mutex>
#include <vector>
#include <chrono>
#include <thread>
#include <string.h>

using namespace std;
using namespace SockWrapperForCplusplus;
using namespace SkvsProtocol;
using namespace CommandList;

extern unsigned int LogAdapterSerial_input;
extern unsigned int DBAdapterSerial_input;

extern mutex writeUserInfoMutex; //유저 정보 수정할 때 사용
extern bool shutdownSignal; //종료 시그널
extern AccountLoader* accountLoader; //사용자 정보 변경

void RecvThread(Socket* socket,
                UserList* userList,
                LoginedUserList* loginedUserList,
                LoginedUser* user,
                CommandFilter* cmdFilter,
                shared_ptr<queue<Packet*, deque<Packet*>>> _sendPacketQueue,
                weak_ptr<ThreadAdapter::AdapterThreadBridge> _adapterBridgeQueue,
                bool* isDisConnected) {
    
    //다른 스레드로 패킷을 이동할 때 사용함
    weak_ptr<ThreadAdapter::AdapterThreadBridge> adapterBridgeQueue = _adapterBridgeQueue;

    //SendThread로 패킷을 보냄
    weak_ptr<queue<Packet*, deque<Packet*>>> sendPacketQueue = _sendPacketQueue;

    while(!shutdownSignal && !(*isDisConnected)) {

        int recvBufSize = 0;
        char* recvBuf = nullptr;
        LogPacket* logPacket = nullptr;

        //응답 시간 제한
        if( recvData(socket, &recvBufSize, sizeof(int)) <= 0 ) {

            if(*isDisConnected != true) {
                logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, "Client DisConnected");
                cerr << logPacket->getStatement() << endl;
                adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);
            }

            *isDisConnected = true;
            return;
        }
        
        PacketType recvType; //클라이언트로부터 받는 패킷 타입

        //여기서부터 시간제한 걸기
        //1분이 지나도 응답이 없을 경우 원상태로 복구
        int cancelCounter = 0;
        
        while(true) {

            //입력 못받음
            if( recvData(socket, &recvType, sizeof(PacketType), MSG_DONTWAIT) <= 0) { //입력 못받음

                if(cancelCounter == 10000) { //제한시간 초과

                    logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, "Client DisConnected");
                    cerr << logPacket->getStatement() << endl;
                    adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);

                    *isDisConnected = true;
                    return;

                } else {
                    cancelCounter++;
                    this_thread::sleep_for(chrono::milliseconds(1));
                    continue;
                }
            } else { //바듬
                cancelCounter = 0;
                break;
            }
        }

        //패킷 타입 유효성 검사
        int packetTypeToInt = recvType;

        if( packetTypeToInt < 0 || packetTypeToInt > PACKETTYPE_LOG) {
            logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0,
                    "This Packet Type is Invaild. It seems Server Attack.");
            cerr << logPacket->getStatement() << endl;

            adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);

            *isDisConnected = true;
            continue;
        }


        recvBuf = new char[recvBufSize+1]; //데이터받을 버퍼
        recvBuf[recvBufSize] = '\0';
        
        //recvBuf = {0};
        while(true) {
            if( recvData(socket, recvBuf, recvBufSize, MSG_DONTWAIT) <= 0) {
                if(cancelCounter == 10000) {

                    logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, "Client DisConnected");
                    cerr << logPacket->getStatement() << endl;
                    adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);

                    *isDisConnected = true;
                    return;
                } else {
                    cancelCounter++;
                    this_thread::sleep_for(chrono::milliseconds(1));
                    continue;
                }
            } else { //바듬
                cancelCounter = 0;
                break;
            }
        }
        
         //데이터 역직렬화
        SendCmdPacket* recvPacket = nullptr;

        //SendCmd나 log가 들어오면 안됨
        switch( recvType ) {

            case PACKETTYPE_SENDCMD:
            {
                recvPacket = returnToPacket<SendCmdPacket>(recvBuf);

                //잘못된 데이터
                if( recvPacket == nullptr) {
                    //데이터 파기

                    //서버공격으로 판단하고 연결 끊기
                    logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, "Unknown Packet From This Client");
                    cerr << logPacket->getStatement() << endl;
                    adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);

                    logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, "It seems server attack so. disconnect witch this client");
                    cerr << logPacket->getStatement() << endl;
                    adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);
                    
                    delete[] recvBuf;
                    *isDisConnected = true;
                    continue;
                }

                //소켓 번호를 서버측의 소켓 번호로 변경
                recvPacket->setSock(socket->getDiscripter());
            }
            break;  
            default:
                delete[] recvBuf;

                logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, "Unknown Packet From This Client");
                cerr << logPacket->getStatement() << endl;
                adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);

                logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, "It seems server attack so. disconnect witch this client");
                cerr << logPacket->getStatement() << endl;
                adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);
                    
                delete[] recvBuf;
                *isDisConnected = true;

                continue;
            break;
        }
        delete[] recvBuf;
        TaskMileStone milestone = cmdFilter->getMileStone(*recvPacket);

        string logMsg; //로그로 작성할 메세지
        string msg; //클라이언트에게 보낼 메세지

        //패킷이 들어갸야 할 방향 제시
        switch(milestone) {

            
            case TASKMILESTONE_DATABASE:
                //데이터베이스 입력 패킷 큐로 패킷을 이동
                adapterBridgeQueue.lock()->pushInQueue(recvPacket, DBAdapterSerial_input);
            break;
            case TASKMILESTONE_SETUSERS: //유저 세팅
            {
                bool missionSuccess = false; //명령수행 성공 여부

                //수신을 시작하는 패킷 삽입
                sendPacketQueue.lock()->push( new SignalPacket(user->getID(),
                                        socket->getIP(),
                                        recvPacket->getCmdNum(),
                                        socket->getDiscripter(),
                                        SIGNALTYPE_RECVSTART   
                ));

                if(recvPacket->getCmdArray()[0].compare(User_Setting::userAdd) == 0) {
                    
                    //useradd [new user] [new password] [set level]
                    

                    //인자값 확인
                    if(recvPacket->getCmdArray().size() != 4) {
                        msg = "setpswd [new user] [new pswd] [set level]";
                        logMsg = "useradd failed";
                        missionSuccess = false;
                    }
                    //아이디, 패스워드 범위값 확인
                    else if(!( recvPacket->getCmdArray()[1].length() >= MIN_ID_LENGTH && 
                                recvPacket->getCmdArray()[1].length() <= MAX_ID_LENGTH)) {
                        msg = "ID size must between 4 and 16";
                        logMsg = "useradd failed";
                        missionSuccess = false;
                    } else if(!( recvPacket->getCmdArray()[2].length() >= MIN_PSWD_LENGTH && 
                                recvPacket->getCmdArray()[2].length() <= MAX_PSWD_LENGTH)) {
                        msg = "Password size must between 8 and 16";
                        logMsg = "useradd failed";
                        missionSuccess = false;
                    
                    //아이디 문자 유효성 확인
                    } else if(!tok::IsAllowedCharacter(recvPacket->getCmdArray()[1], R"(~!@#$%^&*()_+-=[];'./,>{}:"<?")")) {
                        msg = R"(~!@#$%^&*()_+-=[];'./,>{}:"<?")"; 
                        msg += "is not allowd in username";
                        logMsg = "useradd failed";
                        missionSuccess = false;
                    } else {
                    //유저 레벨 유효성 확인
                        UserLevel newUserLevel;

                        try {
                            newUserLevel = User::userLevelConverter(recvPacket->getCmdArray()[3]);
                        } catch(DataConvertException& e) {
                            msg = e.getErrorMsg();
                            logMsg = "tuseradd failed";
                            missionSuccess = false;
                        }

                        //exception으로 인한 msg에 데이터 삽입 발생
                        //msg length가 0인 시점에서 다음 명령 수행
                        if( msg.length() == 0) {

                            //유저 클래스 작성
                            User newUser(recvPacket->getCmdArray()[1],
                                    recvPacket->getCmdArray()[2],
                                    newUserLevel
                            );

                            //userList에 추가
                            writeUserInfoMutex.lock();
                            userList->insertUser(newUser);
                    

                            //json에도 추가
                            bool resultToAddUser = accountLoader->addUser(newUser);

                            if(resultToAddUser) {
                                logMsg = "add user -> ID " + newUser.getID();
                                msg = "complete to add user.";
                                accountLoader->updateFile();
                                writeUserInfoMutex.unlock();
                                missionSuccess = true;
                            } else {
                                writeUserInfoMutex.unlock();
                                logMsg = "useradd failed";
                                msg = "This user is aleady exist";
                                missionSuccess = false;
                            }

                        }
                    }        

                } else if(recvPacket->getCmdArray()[0].compare(User_Setting::userDel) == 0) {
                    //userdel [user]
                    
                    //인자값 확인
                    if(recvPacket->getCmdArray().size() != 2) {
                        msg = "userdel [user]";
                        logMsg = "userdel failed";
                        missionSuccess = false;
                    }

                    //자기 자신은 삭제 못함
                    else if( recvPacket->getCmdArray()[1].compare(user->getID()) == 0) {
                        msg = "you can't remove yourself";
                        logMsg = "userdel failed";
                        missionSuccess = false;

                    //삭제대상의 유저가 로그인되어있는 경우
                    } else if(loginedUserList->searchLoginedUser(recvPacket->getCmdArray()[1])) {

                        msg = "Target User is logined.";
                        logMsg = "userdel failed";
                        missionSuccess = false;

                    }  else {
                        
                        writeUserInfoMutex.lock();
                        bool result = userList->deleteUser(recvPacket->getCmdArray()[1]);
                        

                        if(!result) {
                            writeUserInfoMutex.unlock();
                            msg = "this user does not exist";
                            logMsg = "userdel Failed";
                            missionSuccess = false;

                        } else {
                            
                            accountLoader->deleteUser(recvPacket->getCmdArray()[1]);
                            accountLoader->updateFile();
                            writeUserInfoMutex.unlock();

                            msg = "userdel complete";
                            logMsg = "userdel Complete";
                            missionSuccess = true;
                            
                        }
                    
                    }
                }
                delete recvPacket;

                //패킷 전송
                sendPacketQueue.lock()->push( new RecvMsgPacket(
                    user->getID(),
                    socket->getIP(),
                    recvPacket->getCmdNum(),
                    socket->getDiscripter(),
                    msg
                ));

                //마지막을 알리는 패킷 전송
                if(missionSuccess) {
                    sendPacketQueue.lock()->push( new SignalPacket(user->getID(),
                                        socket->getIP(),
                                        recvPacket->getCmdNum(),
                                        socket->getDiscripter(),
                                        SIGNALTYPE_RECVEND   
                    ));
                } else {
                    sendPacketQueue.lock()->push( new SignalPacket(user->getID(),
                                        socket->getIP(),
                                        recvPacket->getCmdNum(),
                                        socket->getDiscripter(),
                                        SIGNALTYPE_ERROR   
                    ));
                }

                //로그 기록
                logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, logMsg );
                adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);

            }
            break;
            case TASKMILESTONE_SYSTEM: //시스템컨트롤
            {

                if(recvPacket->getCmdArray()[0].compare(System_Control::quit) == 0) {
                    sendPacketQueue.lock()->push(new SignalPacket(user->getID(),
                                                        socket->getIP(),
                                                        recvPacket->getCmdNum(),
                                                        socket->getDiscripter(),
                                                        SIGNALTYPE_SHUTDOWN
                    ));

                    //*isDisConnected = true;
                    delete recvPacket;
                    return;

                } else if(recvPacket->getCmdArray()[0].compare(System_Control::shutdown) == 0) {
                    cout << "shutdown" << endl;
                    shutdownSignal = true;
                    return;
                }
            }
            break;
            default: //error noauth
            {
                //delete recvPacket;
                string errorMsg;
                string logMsg;

                if(milestone == TASKMILESTONE_ERR) {
                    errorMsg = "Command Not Found";
                }
                else {
                    errorMsg = "Authority denied";
                    logMsg = "This IP try to root command [ID " + user->getID() + "]";
                    logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, logMsg);
                    adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);
                }

                //수신을 시작하는 패킷 삽입
                sendPacketQueue.lock()->push( new SignalPacket(user->getID(),
                                        socket->getIP(),
                                        recvPacket->getCmdNum(),
                                        socket->getDiscripter(),
                                        SIGNALTYPE_RECVSTART   
                ));

                //에러메세지를 보내는 패킷 삽입
                sendPacketQueue.lock()->push( new RecvMsgPacket(user->getID(), 
                                         socket->getIP(),
                                         recvPacket->getCmdNum(),
                                         socket->getDiscripter(),
                                         errorMsg
                ));
                //시그널 패킷 삽입
                sendPacketQueue.lock()->push( new SignalPacket(user->getID(),
                                        socket->getIP(),
                                        recvPacket->getCmdNum(),
                                        socket->getDiscripter(),
                                        SIGNALTYPE_ERROR   
                ));
            }
            delete recvPacket;
            break;
        }

    }
}