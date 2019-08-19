/***
 *  @file : CommandList.hpp
 * 	@date 2019/08/19
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 로그데이터를 관리합니다.
 * 
 ***/
#include <string>
#include "../threadAdapter/AdapterThreadUtility.hpp"
#include "../packet/Packet.hpp"

using namespace std;
using namespace ThreadAdapter;

//AdpaterThread를 필요로 하는 클래스 입니다.
// log_file : fileRoot/20XX_XX_XX.log
class LogBase {
private:
    string dirRoot;
    weak_ptr<AdapterThreadUtility> adapterQueue;
public:
    LogBase(const string _fileRoot, shared_ptr<ThreadAdapter::AdapterThreadUtility>& _adapterQueue);
    const bool writeLogToFile(LogPacket* _logPacket);

    //어댑터 연결 여부 확인
    const bool checkAdapter(void);

};