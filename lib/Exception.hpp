/***
 *  @file : Exception.hpp
 * 	@date 2019/08/11
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 예외처리를 담당합니다
 * 
 ***/

#ifndef EXCEPTION_HPP
# define EXCEPTION_HPP

#include <string>
#include <iostream>

using namespace std;

class Exception {
private:
	string errorMsg;
public:
	Exception(string _errorMsg):errorMsg(_errorMsg) { }
	string getErrorMsg(void) {
		return errorMsg;
	}
	void printErrorMsg(void) {
		cout << errorMsg << endl;
	}
};

//1. DataException
class DataException : public Exception {
public:
	DataException(string _errorMsg) : Exception(_errorMsg) { } 
	
};

class DataVaildException : public DataException {
public:
	DataVaildException(string _errorMsg) : DataException(_errorMsg) { }	
};

class DataConvertException : public DataException{

public:
	DataConvertException(string _errorMsg ) : DataException(_errorMsg) { } 

};

class DataTypeException : public DataException {
public:
	DataTypeException(string _errorMsg) : DataException(_errorMsg) { }
};

class DataConditionException : public DataException {
public:
	DataConditionException(string _errorMsg) : DataException(_errorMsg) { }
};

//링크문제
class DataLinkException : public DataException {
public:
	DataLinkException(string _errorMsg) : DataException(_errorMsg) { }
};



//PacketException
class PacketException : public Exception {
public:
	PacketException(string _errorMsg) : Exception(_errorMsg) { }
};

class UserException : public Exception {
public:
	UserException(string _errorMsg) : Exception( _errorMsg) { }
};

class UserLengthException : public UserException {
public:
	UserLengthException(string _errorMsg) : UserException(_errorMsg) { }
};

class FileException : public Exception {
public:
	FileException(string _errorMsg) : Exception(_errorMsg) { }

};
#endif



