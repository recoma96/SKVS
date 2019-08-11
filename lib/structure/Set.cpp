#include "Set.hpp"

#include <string>
#include <set>
#include <list>

using namespace std;
using namespace structure;

structure::Set::Set( const string _keyData, const DataType _valueDataType,
		const StructType _structType ) : 
	MultiDataStructure( _keyData, DATATYPE_STRING, _structType ) {

	this->valueDataType = _valueDataType;
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
//!! deleteRange에서 이 함수를 사용하기 위해 list의 Element는 원소 자체가 아닌 포인터
//결과데이터의 복사본 출력은 DataBase Class에서 구현
list<DataElement*> Set::searchRange(ScanDataCondition& _condition) {
	
	list<DataElement*> resultList;

	if( this->valueSet.empty() ) return resultList;


	//ConditionClass의 내용이 비었음 -> 전체범위
	if( _condition.isEmpty() ) {
		for( set<DataElement*>::iterator iter = this->valueSet.begin();
				iter != this->valueSet.end(); iter++ ){
			resultList.push_back( (*iter) );
		}
		return resultList;
	} else {

		//number,float와 string으로 탐색알고리즘 스타일이 바뀜
		//MultiDataStructure.hpp

		//1. string
		if( this->valueDataType == DATATYPE_STRING ) {
			
			//condition첫부분이 string이 아니면 DataTypeException 반환
			if( _condition.conditionToken[0]->dcf != DCF_STRING ) 
				throw DataTypeException("This Set type is string but condition style is not string.");
			else {
				//순환하면서 탐색
				IsInStringRange isInStringRange;

				for( set<DataElement*>::iterator iter = this->valueSet.begin();
						iter != this->valueSet.end(); iter++ ) {
					
					if(isInStringRange(_condition, **iter))
						resultList.push_back(*iter);
				}
			}
		} else { // == DATATYPE_NUMBER, DATATYPE_FLOAT
			
			if(_condition.conditionToken[0]->dcf == DCF_STRING )
				throw DataTypeException("This Set type is numberical but, condition style is string.");
			else {
				
				IsInNumberRange isInNumberRange;

				for(set<DataElement*>::iterator iter = this->valueSet.begin();
						iter != this->valueSet.end(); iter++ ) {

					if( isInNumberRange(_condition, **iter))
						resultList.push_back(*iter);
				}
			}
		}

	}
		
	return resultList;
}

//조건에 따른 데이터 삭제
bool Set::deleteRange(ScanDataCondition& _condition) {

	//데이터 탐색
	list<DataElement*> deleteList = searchRange(_condition);
	if( deleteList.empty() ) return false; //삭제 실패

	//차례대로 제거
	for( list<DataElement*>::iterator iter = deleteList.begin();
			iter != deleteList.end(); iter++ ) {
		this->valueSet.erase(*iter);
	}

	//마지막에 데이터 갯수 업데이트
	this->updateLength();

	return true;
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

