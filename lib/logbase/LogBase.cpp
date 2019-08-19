#include "LogBase.hpp"
#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

LogBase::LogBase(const string _dirRoot, shared_ptr<ThreadAdapter::AdapterThreadUtility>& _adapterQueue) {
    dirRoot = _dirRoot;
    adapterQueue = _adapterQueue;
}

//fileRoot의 폴더가 존재하지 않으면 false 반환
// log_file : fileRoot/20XX_XX_XX.log
const bool LogBase::writeLogToFile(LogPacket* _logPacket) {

    //디렉토리 유무 확인
    DIR* dp = nullptr;
    //없을 경우 false 반환
    if( (dp=opendir(dirRoot.c_str())) == nullptr) {
        return false;
    }

    //디렉토리 닫기
    closedir(dp);

    //오늘 날짜 갖고오기
    string todayLogFile = CalTime::makeLogDataFileName();

    string totalFileRoot;
    totalFileRoot += dirRoot;
    totalFileRoot += todayLogFile;

    //로그 작성
    ofstream logFile(totalFileRoot, ios::out | ios::app);
    logFile << _logPacket->getStatement() << endl;
    logFile.close();
    
    //delete _logPacket;
    return true;
}

const bool LogBase::checkAdapter(void) {
    shared_ptr<ThreadAdapter::AdapterThreadUtility> adapterChecker = adapterQueue.lock();
    if( adapterChecker) return true;
    else return false;
}