/***
 *  @file : TypePrinter.hpp
 * 	@date 2019/08/11
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 숫자로 되어있는 Type을 string으로 컨버팅합니다.
 * 
 ***/


#ifndef TYPEPRINTER_HPP
# define TYPEPRINTER_HPP

#include <string>
#include "DataElement.hpp"
#include "../Exception.hpp"

using namespace std;

namespace structure {

	//DataType
	const string printedString = "string";
	const string printedNumber = "number";
	const string printedFloat = "float";

	//StructType
	const string printedElement = "element";
	const string printedBasic = "basic";
	const string printedOneSet = "oneset";
	const string printedMultiSet = "multiset";
	const string printedDynamicList = "dynamiclist";
	const string printedStaticList = "staticlist";
	const string printedDynamicHashMap = "dynamichashmap";
	const string printedStaticHashMap = "statichashmap";


	//convert datatype
	inline const string convertDataTypeToString(const DataType _dataType ) {

		switch( _dataType ) {
			case DATATYPE_STRING:
				return printedString;
			case DATATYPE_NUMBER:
				return printedNumber;
			default:
				return printedFloat;
		}
	}

	//convert structtype
	inline const string convertStructTypeToString(const StructType _structType ) {

		switch( _structType ) {
			
			case STRUCTTYPE_ELEMENT:
				return printedElement;
			case STRUCTTYPE_BASIC:
				return printedBasic;
			case STRUCTTYPE_ONESET:
				return printedOneSet;
			case STRUCTTYPE_MULTISET:
				return printedMultiSet;
			case STRUCTTYPE_DYNAMICLIST:
				return printedDynamicList;
			case STRUCTTYPE_STATICLIST:
				return printedStaticList;
			case STRUCTTYPE_DYNAMICHASHMAP:
				return printedDynamicHashMap;
			default:
				return printedStaticHashMap;
		}

	}
	
	template<typename T>
	inline const T convertStringToType(const string _str );

	//template example
	template <>
	inline const DataType convertStringToType<DataType>(const string _str) {
		//1.Data
		if( _str.compare(printedString) == 0 ) return DATATYPE_STRING;
		else if( _str.compare(printedNumber) == 0 ) return DATATYPE_NUMBER;
		else if( _str.compare(printedFloat) == 0 ) return DATATYPE_FLOAT;
		else
			throw DataConvertException("this string is not matched with data or struct type.");
	}
	template <>
	inline const StructType convertStringToType<StructType>(const string _str) {
		//2. Structure
		if( _str.compare(printedElement) == 0 ) return STRUCTTYPE_ELEMENT;
		else if( _str.compare(printedBasic) == 0 ) return STRUCTTYPE_BASIC;
		else if( _str.compare(printedOneSet) == 0 ) return STRUCTTYPE_ONESET;
		else if( _str.compare(printedMultiSet) == 0 ) return STRUCTTYPE_MULTISET;
		else if( _str.compare(printedDynamicList) == 0 ) return STRUCTTYPE_DYNAMICLIST;
		else if( _str.compare(printedStaticList) == 0 ) return STRUCTTYPE_STATICLIST;
		else if( _str.compare(printedDynamicHashMap) == 0 ) return STRUCTTYPE_DYNAMICHASHMAP;
		else if( _str.compare(printedStaticHashMap) == 0 ) return STRUCTTYPE_STATICHASHMAP;
		else
			throw DataConvertException("this string is not matched with data or struct type.");
	}

}

#endif
