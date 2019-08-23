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



//복사본
structure::DataElement& structure::DataElement::operator=( DataElement& _destElement) noexcept {

	this->data = _destElement.getDataToString();
	this->dataType = _destElement.getDataType();
	this->structType = _destElement.getStructType();
	this->size = static_cast<d_size_t>(this->data.length());
	
	return *this;
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


template <>
string structure::DataElement::getData<string>(void) const noexcept {
	if( this->dataType == DATATYPE_STRING) return this->data;
	else return 0;
}
template <>
number_t structure::DataElement::getData<number_t>(void) const noexcept {
	if( this->dataType == DATATYPE_NUMBER) return (number_t)(atoi(this->data.c_str()));
	else return 0;
}
template <>
float_t structure::DataElement::getData<float_t>(void) const noexcept {
	if( this->dataType == DATATYPE_FLOAT) return (float_t)(atof(this->data.c_str()));
	else return 0;
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