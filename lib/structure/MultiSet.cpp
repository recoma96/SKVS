#include "MultiSet.hpp"
#include "../Exception.hpp"

bool structure::MultiSet::insertValue(const string _value) {

	//해당 데이터가 데이터타입에 맞는 지 확인
	//Number와 Float만 판단
	CheckDataType checkDataType;

	if( this->valueDataType == DATATYPE_NUMBER ) {
		if( !checkDataType( _value, DATATYPE_NUMBER ))
			throw DataTypeException("your input type and Set type is not same.");
	} else if( this->valueDataType == DATATYPE_FLOAT ) {
		if( !checkDataType(_value, DATATYPE_FLOAT ))
			throw DataTypeException("your input type and Set type is not same,");
	}
	//데이터 삽입
	DataElement* inputElement = new DataElement(_value, this->valueDataType, STRUCTTYPE_ELEMENT);

	this->valueSet.insert(inputElement);
	this->updateLength();

	return true;
}

const unsigned int structure::MultiSet::coutValue(const string _value) {

	unsigned int counter = 0;
	for( set<DataElement*>::iterator iter = this->valueSet.begin();
			iter != this->valueSet.end(); iter++ ) {

		if( (*iter)->getDataToString().compare( _value ) == 0 )
			counter++;
	}

	return counter;


}