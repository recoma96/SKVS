#include "../lib/packet/Packet.hpp"
#include "../lib/user/LoginedUserList.hpp"
#include "../lib/CommandFilter.hpp"
#include "../lib/SockWrapper/ServerSocketManager.hpp"
#include "../lib/SockWrapper/NetworkingManager.hpp"
#include "../lib/SockWrapper/SocketManager.hpp"
#include "../lib/Tokenizer.hpp"
#include "../lib/threadAdapter/AdapterThreadBridge.hpp"


#include "../lib/Tokenizer.hpp"

#include <string>
#include <list>
#include <map>
#include <deque>
#include <queue>
#include <mutex>
#include <chrono>
#include <thread>

using namespace std;
using namespace SockWrapperForCplusplus;

extern unsigned int LogAdapterSerial_input;
extern unsigned int DBAdapterSerial_input;

extern bool shutdownSignal; //종료 시그널

//Recv Thread
extern void RecvThread(Socket* socket,
                UserList* userList,
                LoginedUserList* loginedUserList,
                LoginedUser* user,
                CommandFilter* cmdFilter,
                shared_ptr<queue<Packet*, deque<Packet*>>> _sendPacketQueue,
                weak_ptr<ThreadAdapter::AdapterThreadBridge> _adapterBridgeQueue,
                bool* isDisConnected);

//Send Thread
extern void SendThread(Socket* socket,
                LoginedUser* user,
                shared_ptr<queue<Packet*, deque<Packet*>>> _sendPacketQueue,
                weak_ptr<ThreadAdapter::AdapterThreadBridge> _adapterBridgeQueue,
                bool* isDisConnected );

void IOThread(   UserList* userList, 
                        LoginedUserList* loginedUserList, 
                        Socket* sock,
                        CommandFilter* cmdFilter, 
                        map< int, weak_ptr<queue<Packet*, deque<Packet*>>> >* packetBridge, 
                        mutex* bridgeMutex,   
                        shared_ptr<ThreadAdapter::AdapterThreadBridge> _adpaterBridgeQueue) {
    
    //해당 클라이언트로부터 데이터 수신

    char loginBuf[48] = {0};
    bool isLoginInfoRight = false;

    //브릿지 큐
    weak_ptr<ThreadAdapter::AdapterThreadBridge> adapterBridgeQueue = _adpaterBridgeQueue;
    LogPacket* logPacket = nullptr;


    if( recvData(sock, loginBuf, sizeof(char)*48) <= 0 ) {

        string errorMsg("Connected defused from client - ");
        errorMsg += sock->getIP();

        logPacket = new LogPacket("Server", "Server", 0, 0, errorMsg);
        cerr << logPacket->getStatement() << endl;
        adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);

        closeSocket(sock);
        delete sock;
        return;
    }

    string loginStr(loginBuf); //변환
    //토큰화
    vector<string> loginVec = tok::tokenizer(loginStr, '-');
    //0 -> id, 1 -> pswd

    //아이디, 패스워드 판별    
    User user;
    try {
        user = userList->getCopiedUserData(loginVec[0]);

        if(user.getPassword().compare(loginVec[1]) == 0)
            isLoginInfoRight = true;
        else isLoginInfoRight = false;

    } catch(UserException e) {
        cerr << e.getErrorMsg() << endl;
        isLoginInfoRight = false;
    }

    //인증 결과 데이터 전송
    if( sendData(sock, &isLoginInfoRight, sizeof(bool)) <= 0) {

        string errorMsg("Failed To Connect client - ");
        errorMsg += sock->getIP();
        
        logPacket = new LogPacket("Server", "Server", 0, 0, errorMsg);
        cerr << logPacket->getStatement() << endl;
        adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);

        closeSocket(sock);
        delete sock;
        return;
    }
    //인증이 틀린 경우
    if(!isLoginInfoRight) {
        //실페 로그패킷 전송

        string errorMsg("This IP is failed to Login - ");
        errorMsg += sock->getIP();
        
        logPacket = new LogPacket("Server", "Server", 0, 0, errorMsg);
        cerr << logPacket->getStatement() << endl;
        adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);


        closeSocket(sock);
        delete sock;
        return;
    }  else {

        UserLevel sendLevel = user.getUserLevel();
        if( sendData(sock, &sendLevel, sizeof(UserLevel)) <= 0) {
            
            string errorMsg("Failed To Connect client - ");
            errorMsg += sock->getIP();
        
            logPacket = new LogPacket("Server", "Server", 0, 0, errorMsg);
            cerr << logPacket->getStatement() << endl;
            adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);

            closeSocket(sock);
            delete sock;
            return;
        }
    }

    //로그인 유저 리스트 생성
    LoginedUser loginedUser(user.getID(), 
                            user.getPassword(), 
                            user.getUserLevel(),
                            sock->getIP(),
                            sock->getDiscripter()
    );

    //패킷브릿지 추가, 로그인유저리스트 추가
    bridgeMutex->lock();
    loginedUserList->insertLoginedUser(loginedUser);
    
    //단일 패킷브릿지 생성
    shared_ptr<queue<Packet*, deque<Packet*>>> userPacketBridge = 
        make_shared<queue<Packet*, deque<Packet*>>>();

    //패킷 브릿지 생성
    packetBridge->insert(pair<int, weak_ptr<queue<Packet*, deque<Packet*>>> >
                                (sock->getDiscripter(), userPacketBridge ));

    bridgeMutex->unlock();

    //User Logined!
    string alertMsg("This User is Logined. User : ");
    alertMsg += user.getID();
        
    logPacket = new LogPacket(user.getID(), sock->getIP(), 0, 0, alertMsg);
    cout << logPacket->getStatement() << endl;
    adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);

    //클라이언트와의 연결이 끊어졌는 지에 대한 여부
    bool isDisConnected = false;

    //Send/Recv Thread 생성
    thread recvThread(RecvThread,
                      sock,
                      userList,
                      loginedUserList,
                      &loginedUser,
                      cmdFilter,
                      userPacketBridge,
                      adapterBridgeQueue,
                      &isDisConnected
    );
    recvThread.detach();
    //SendTrhead
    thread sendThread(SendThread,
                      sock,
                      &loginedUser,
                      userPacketBridge,
                      adapterBridgeQueue,
                      &isDisConnected

    );
    
    
    //while진입
    while(!shutdownSignal && !isDisConnected ) {
        this_thread::sleep_for(chrono::milliseconds(1));
    }

    //클라이언트로부터 종료를 시도할 경우
    //sendThread가 클라이언트에게 종료시그널을 보낼 때 까지 기다린다.
    sendThread.join();
    //recvThread.join();
    
    if(isDisConnected) {
        //패킷 브릿지 삭제
        packetBridge->erase(sock->getDiscripter());
        //로그인 유저 리스트에서 삭제
        if(!loginedUserList->deleteLoginedUser(loginedUser))
            cout << "delete Error" << endl;

        logPacket = new LogPacket(user.getID(), sock->getIP(), 0, 0, "This User is loged out");
        cout << logPacket->getStatement() << endl;
        adapterBridgeQueue.lock()->pushInQueue(logPacket, LogAdapterSerial_input);
    }
}