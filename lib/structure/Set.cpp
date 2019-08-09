#include "Set.hpp"

#include <string>
#include <set>
#include <list>

using namespace std;
using namespace structure;

structure::Set::Set( const string _keyData, const DataType _valueDataType,
		const StructType _structType ) : 
	MultiDataStructure( _keyData, DATATYPE_STRING, _structType ) {

	this->dataType = _valueDataType;
	this->length = 0;

}

Set::~Set() noexcept {

	if(!this->valueSet.empty() ) {
		
		for( set<DataElement*>::iterator iter = this->valueSet.begin();
				iter != this->valueSet.end(); iter++ ) {

			DataElement* deleteElement = (*iter);
			this->valueSet.erase(iter);
			delete deleteElement;
		}

	}

}

//데이터 갯수 업데이트
void Set::updateLength(void) noexcept {
	
	this->length = this->valueSet.size();

}


//조건에 따른 Set안의 데이터 수집
list<DataElement*> Set::searchRange(ScanDataCondition& _condition) {
	
	list<DataElement*> resultList;

	if( this->valueSet.empty() ) return resultList;

	bool isSearchByString = false;
	// 범위값으로 탐색하는지 아니면
	// 문자열로 찾을 것인지 판별:
		
	//탐색
	for( set<DataElement*>::iterator iter = this->valueSet.begin();
			iter != this->valueSet.end(); iter++ ) {


	}

	return resultList;
}

//조건에 따른 데이터 삭제
bool Set::deleteRange(ScanDataCondition& _condition) {

	//마지막에 데이터 갯수 업데이트
	this->updateLength();

	return false;
}

//데이터 사이즈 구하기
d_size_t Set::getStructSize(void) noexcept {

	if( this->valueSet.empty() ) return 0;
	else {

		d_size_t resultSize = 0;

		//순회하면서 데이터 길이 구하기
		for( set<DataElement*>::iterator iter = this->valueSet.begin();
				iter != this->valueSet.end(); iter++ ) {
			resultSize += (*iter)->getSize();
		}
	}

}

