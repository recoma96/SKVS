#include "DataElement.hpp"
#include "../Exception.hpp"

#include <cstdlib>

using namespace std;


structure::DataElement::DataElement(const string _data, const DataType _dataType, 
		const StructType _structType ) {
	
	CheckDataType checkDataType;
	CheckStructType checkStructType;

	if( !(checkDataType( _data, _dataType ) && checkStructType( _dataType, _structType ) ) )
		throw new DataVaildException("Data and DataType or StructType is not matched.");

	this->data = _data;
	this->dataType = _dataType;
	this->structType = _structType;

	this->size = (d_size_t)(_data.length());

}

//메모리풀에서의 데이터생성용
void structure::DataElement::setDataElement(
											const string _data,
											const DataType _dataType,
											const StructType _structType ) {

	CheckDataType checkDataType;
	CheckStructType checkStructType;

	if( !(checkDataType( _data,  _dataType) && checkStructType( _dataType, _structType) ) )
		throw new DataVaildException("Data and DataType or StructType is not matched,");

	this->data = _data;
	this->dataType = _dataType;
	this->structType = _structType;

	this->size = (d_size_t)(_data.length());

}

bool structure::DataElement::operator==( const DataElement& _targetElement ) const noexcept {

	if( this->dataType == _targetElement.getDataType() && 
			this->data.compare( _targetElement.getDataToString() ) == 0 )
		return true;
	else return false;
		
}

template <typename T>
T structure::DataElement::getData(void) const noexcept {

	switch( this->dataType ) {

		case DATATYPE_STRING:
			return this->data;
		case DATATYPE_NUMBER:
			return (number_t)(atoi(this->data.c_str()));
		default:
			return (float_t)(atof(this->data.c_str()));
	}
}

string structure::DataElement::getDataToString(void) const noexcept {
	return this->data;
}
structure::DataType structure::DataElement::getDataType(void) const noexcept {
	return this->dataType;
}
structure::StructType structure::DataElement::getStructType(void) const noexcept {
	return this->structType;
}
d_size_t structure::DataElement::getSize(void) const noexcept {
	return this->size;
}

void structure::DataElement::setData( string _data, DataType _dataType ) {
	
	CheckDataType checkDataType;
	CheckStructType checkStructType;

	if( !(checkDataType( _data, _dataType ) && checkStructType( _dataType, this->structType) ) )
		throw new DataVaildException("Data and DataType or StructType is not matched.");
	else {

		this->dataType = _dataType;
		this->data = _data;
		this->size = (d_size_t)(data.length());

	}

}


