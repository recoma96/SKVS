#include "../lib/packet/Packet.hpp"
#include "../lib/user/LoginedUser.hpp"
#include "../lib/CommandFilter.hpp"
#include "../lib/user/LoginedUserList.hpp"
#include "../lib/SockWrapper/ServerSocketManager.hpp"
#include "../lib/SockWrapper/NetworkingManager.hpp"
#include "../lib/SockWrapper/SocketManager.hpp"
#include "../lib/threadAdapter/AdapterThreadBridge.hpp"
#include "../lib/packet/SerialController.hpp"
#include "../lib/CommandList.hpp"
#include "../lib/loader/SystemLoader.hpp"
#include "../lib/loader/AccountLoader.hpp"
#include "../lib/Tokenizer.hpp"

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
using namespace PacketSerialData;
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

        //데이터받기
        if( recvData(socket, &recvBufSize, sizeof(int)) <= 0 ) {
            
            //클라이언트를 종료 시킬 경우 이 스레드가 남으므로
            //차후에 개선할 예정
            if(*isDisConnected != true) {
                logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, "Server DisConnected");
                cerr << logPacket->getStatement() << endl;
                adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);
            }

            *isDisConnected = true;
            continue;
        }

        
        PacketType recvType; //클라이언트로부터 받는 패킷 타입

        if( recvData(socket, &recvType, sizeof(PacketType)) <= 0) {

            logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, "Server DisConnected");
            cerr << logPacket->getStatement() << endl;
            adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);

            *isDisConnected = true;
            continue;
        }

        recvBuf = new char[recvBufSize]; //데이터받을 버퍼
        if( recvData(socket, recvBuf, recvBufSize) <= 0) {

            logPacket = new LogPacket(user->getID(), socket->getIP(), 0, 0, "Server DisConnected");
            cerr << logPacket->getStatement() << endl;
            adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);

            *isDisConnected = true;
            continue;
        }

         //데이터 역직렬화
        SendCmdPacket* recvPacket = nullptr;

        //SendCmd나 log가 들어오면 안됨
        switch( recvType ) {

            case PACKETTYPE_SENDCMD:
            {
                recvPacket = returnToPacket<SendCmdPacket>(recvBuf);

                //소켓 번호를 서버측의 소켓 번호로 변경
                recvPacket->setSock(socket->getDiscripter());
            }
            break;  
            default:
                delete[] recvBuf;
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
                //adapterBridgeQueue.lock()->pushInQueue(recvPacket, DBAdapterSerial_input);
            break;
            case TASKMILESTONE_SETUSERS: //유저 세팅
            {

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
                    }
                    //아이디, 패스워드 범위값 확인
                    else if(!( recvPacket->getCmdArray()[1].length() >= MIN_ID_LENGTH && 
                                recvPacket->getCmdArray()[1].length() <= MAX_ID_LENGTH)) {
                        msg = "ID size must between 4 and 16";
                        logMsg = "useradd failed";
                    } else if(!( recvPacket->getCmdArray()[2].length() >= MIN_PSWD_LENGTH && 
                                recvPacket->getCmdArray()[2].length() <= MAX_PSWD_LENGTH)) {
                        msg = "Password size must between 8 and 16";
                        logMsg = "useradd failed";
                    
                    //아이디 문자 유효성 확인
                    } else if(!tok::IsAllowedCharacter(recvPacket->getCmdArray()[1], R"(~!@#$%^&*()_+-=[];'./,>{}:"<?")")) {
                        msg = R"(~!@#$%^&*()_+-=[];'./,>{}:"<?")"; 
                        msg += "is not allowd in username";
                        logMsg = "useradd failed";
                    } else {
                    //유저 레벨 유효성 확인
                        UserLevel newUserLevel;

                        try {
                            newUserLevel = User::userLevelConverter(recvPacket->getCmdArray()[3]);
                        } catch(DataConvertException& e) {
                            msg = e.getErrorMsg();
                            logMsg = "tuseradd failed";
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
                            } else {
                                writeUserInfoMutex.unlock();
                                logMsg = "useradd failed";
                                msg = "This user is aleady exist";
                            }

                        }
                    }        

                } else if(recvPacket->getCmdArray()[0].compare(User_Setting::userDel) == 0) {
                    //userdel [user]

                    //인자값 확인
                    if(recvPacket->getCmdArray().size() != 2) {
                        msg = "userdel [user]";
                        logMsg = "userdel failed";
                    }

                    //자기 자신은 삭제 못함
                    else if( recvPacket->getCmdArray()[1].compare(user->getID()) == 0) {
                        msg = "자신감을 잃지 말아요 ㅠㅠ";
                        logMsg = "userdel failed";

                    //삭제대상의 유저가 로그인되어있는 경우
                    } else if(loginedUserList->searchLoginedUser(recvPacket->getCmdArray()[1])) {

                        msg = "Target User is logined.";
                        logMsg = "userdel failed";

                    }  else {
                        
                        writeUserInfoMutex.lock();
                        bool result = userList->deleteUser(recvPacket->getCmdArray()[1]);
                        

                        if(!result) {
                            writeUserInfoMutex.unlock();
                            msg = "this user does not exist";
                            logMsg = "userdel Failed";

                        } else {
                            
                            accountLoader->deleteUser(recvPacket->getCmdArray()[1]);
                            accountLoader->updateFile();
                            writeUserInfoMutex.unlock();

                            msg = "userdel complete";
                            logMsg = "userdel Complete";
                            
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
                sendPacketQueue.lock()->push( new SignalPacket(user->getID(),
                                        socket->getIP(),
                                        recvPacket->getCmdNum(),
                                        socket->getDiscripter(),
                                        SIGNALTYPE_RECVEND   
                ));

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
                                        SIGNALTYPE_RECVEND   
                ));
            }
            delete recvPacket;
            break;
        }

    }
}