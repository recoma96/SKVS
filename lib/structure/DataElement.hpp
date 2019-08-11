#ifndef DATAELEMENT_HPP
# define DATAELEMENT_HPP

#include <string>


typedef unsigned int d_size_t; //데이터 용량(길이)

typedef long number_t; //숫자
typedef double float_t; //실수

using namespace std;

namespace structure {
	
	enum DataType {
		
		DATATYPE_STRING,
		DATATYPE_NUMBER,
		DATATYPE_FLOAT

	};

	enum StructType {
		
		STRUCTTYPE_ELEMENT,
		STRUCTTYPE_BASIC,
		STRUCTTYPE_ONESET,
		STRUCTTYPE_MULTISET,
		STRUCTTYPE_DYNAMICLIST,
		STRUCTTYPE_STATICLIST,
		STRUCTTYPE_DYNAMICHASHMAP,
		STRUCTTYPE_STATICHASHMAP

	};

	//데이터타입 유효성 체크 객체함수
	
	//1. 데이터 - 데이터타입 매칭 객체함수
	struct CheckDataType {

		bool operator() (string _data, DataType _dataType ) noexcept {

			if( _dataType == DATATYPE_STRING) return true;
			else if( _dataType == DATATYPE_NUMBER ) {

				if( atoi( _data.c_str() ) == 0 && _data.compare("0") != 0 ) return false;
				else return true;

			} else if( _dataType == DATATYPE_FLOAT ) {
				if( atof( _data.c_str() ) == 0 && _data.compare("0") != 0 ) return false;
				else return true;
			}

			return false;

		}
	};

	//2. StructType Checker
	struct CheckStructType {

		bool operator() ( DataType _dataType, StructType _structType ) noexcept {

			if( _structType == STRUCTTYPE_ELEMENT ) return true;
			else if ( _structType != STRUCTTYPE_ELEMENT ) {

				if( _dataType == DATATYPE_STRING) return true;
				else return false;
			}

		}

	};


	class DataElement {

	private:
		string data;
		DataType dataType;
		StructType structType;
		d_size_t size;
	public:
		explicit DataElement( const string _data, const DataType _dataType, const StructType _structType );
		//throw DataVaildException
		
		//메모리풀 생성용
		DataElement(void) { }
		void setDataElement( const string _data, const DataType _dataType, const StructType _structType );
		
		bool operator==( const DataElement& _targetElement ) const noexcept;

		template <typename T=string>
		T getData(void) const noexcept;
		string getDataToString(void) const noexcept;
		DataType getDataType(void) const noexcept;
		StructType getStructType(void) const noexcept;
		d_size_t getSize(void) const noexcept;

		void setData(string _data, DataType _dataType);
		//throw DataVaildException

	};

}

#endif
