/***
 *  @file : SocketException.hpp
 * 	@version : 0.0.1
 * 	@date 2019/18/09
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 해당 Socket Function 사용에 있어서 생기는 오류를 false 대신 Exception Class를 사용
 *              메모리 동적 할당에 의한 힙 메모리의 단편화를 방지하기 위해 
 *              최소한 이 라이브러리 내에는 포인터를 사용하지 않습니다.
 *  @license : MIT-License
 * 
 ***/

#ifndef SOCKETEXCEPTION_HPP
# define SOCKETEXCEPTION_HPP

namespace SockWrapperForCplusplus {

    class SocketException {
    private:
        string errorMsg;
    public:
        SocketException(const string _errMsg) : errorMsg(_errMsg) { }
        const string getErrorMsg(void) { return errorMsg; }

    };

    //보내거나 받아서 읽을 데이터의 길이 인자값이 0으로 설정되었을 경우 Exception 송출
    class SocketDataSizeZeroException : public SocketException {
    public:
        SocketDataSizeZeroException(const string _errorMsg) : SocketException(_errorMsg) { }
    };
}

#endif
