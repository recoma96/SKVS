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

class DataConditionException : public DataException {
public:
	DataConditionException(string _errorMsg) : DataException(_errorMsg) { }
};
#endif
