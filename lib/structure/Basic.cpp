#include "Basic.hpp"

#include <string>

using namespace std;
using namespace structure;

Basic::Basic( const string _keyData, const string _valueData, const DataType _valueDataType ) :
	Structure( _keyData, DATATYPE_STRING, STRUCTTYPE_BASIC) {

	this->value = new DataElement( _valueData, _valueDataType, STRUCTTYPE_ELEMENT);
	//데이터와 데이터 타입아 안맞을 경우
	//throw DataVaildException

}

Basic::~Basic() {
	delete value;
}

template <typename T>
T Basic::getValue(void) const noexcept {  return value->getData<T>(); }
string Basic::getValueToString(void) const noexcept { return value->getDataToString(); }
DataType Basic::getValueDataType(void) const noexcept { return value->getDataType(); }
StructType Basic::getValueStructType(void) const noexcept { return value->getStructType(); }
d_size_t Basic::getValueSize(void) const noexcept { return value->getSize(); }

//throw DataVaildException
void Basic::setValue(string _value, DataType _valueType) const noexcept { value->setData( _value, _valueType); }


