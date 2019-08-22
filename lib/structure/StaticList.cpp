#include "StaticList.hpp"
#include "../Exception.hpp"
#include "../Tokenizer.hpp"

using namespace std;
using namespace structure;

#include <list>
#include <string>
#include <vector>

StaticList::StaticList(const string _key, const DataType _listDataType) : 
    ListLayer(_key, STRUCTTYPE_STATICLIST) {
    
    this->structureDataType = _listDataType;
}

list<shared_ptr<DataElement>>::iterator StaticList::operator[](const unsigned int _idx) {

    //시간상 알고리즘을 최적화하지 못했습니다.
    //차후에 수정할 예정입니다.
    
    //범위를 벗어난 인덱스는 end()를 리턴합니다.
    if( _idx >= this->length ) return this->valueList.end();

    list<shared_ptr<DataElement>>::iterator resultIter = this->valueList.begin();
    advance(resultIter, _idx);

    return resultIter;
}

//from ListLayer
const bool StaticList::insertValue(const string _value) {
    CheckDataType checkDataType;

    if(!checkDataType(_value, this->structureDataType))
        throw DataTypeException("This DataType is not machted.");

    //StaticList는 데이터 유일성을 판단하지 않음

    shared_ptr<DataElement> insertData(new DataElement(
                                                _value,
                                                this->structureDataType,
                                                STRUCTTYPE_ELEMENT
                                                ));
    this->valueList.push_back(insertData);
    updateLength();
    return true;
}

const list<DataElement> StaticList::searchIndex(const string _indexCondition) {

    vector<string> toked = tok::tokenizer(_indexCondition, '-');
    IsInIndex isInIndex;

    if( !isInIndex( toked, this->length))
        throw DataConditionException("This Index Condition is vailed.");
    
    int minIdx = atoi(toked[0].c_str());
    int maxIdx = atoi(toked[1].c_str());
    
    //탐색 실행
    list<DataElement> resultList;

    list<shared_ptr<DataElement>>::iterator idxIter = (*this)[minIdx];
    int counter = minIdx;

    while( counter <= maxIdx ) {
        resultList.push_back( (**idxIter));
        counter++;
        idxIter++;
    }
    return resultList;
}

const bool StaticList::deleteIndex(const string _indexCondition) {

    vector<string> toked = tok::tokenizer(_indexCondition, '-');
    IsInIndex isInIndex;

    if( !isInIndex( toked, this->length))
        throw DataConditionException("This Index Condition is vailed.");
    
    int minIdx = atoi(toked[0].c_str());
    int maxIdx = atoi(toked[1].c_str());

    list<shared_ptr<DataElement>>::iterator idxIter = (*this)[minIdx];
    int counter = minIdx;

    while(counter <= maxIdx) {
        idxIter = this->valueList.erase(idxIter);
        counter++;
    }
    updateLength();

    return true;
}

const bool StaticList::updateDataByIndex( const unsigned int _targetIndex, const string _newValue ) {

    //범위 판정
    if( _targetIndex >= this->length) return false;

    //데이터타입 적합성 판정
    CheckDataType checkDataType;

    if(!checkDataType(_newValue, this->structureDataType))
        return false;

    //데이터 입력
    list<shared_ptr<DataElement>>::iterator targetIter = (*this)[_targetIndex];
    (*targetIter)->setData(_newValue, this->structureDataType );
    return true;
}

const bool StaticList::updateData(const string _targetValue, const string _value) {

    //데이터 타입 적합성 판정
    CheckDataType checkDataType;

    if(!checkDataType(_value, this->structureDataType))
        return false;
    
    //수정 할 데이터 찾기
    for(list<shared_ptr<DataElement>>::iterator targetIter = this->valueList.begin();
        targetIter != this->valueList.end(); targetIter++ ) {
        if( (*targetIter)->getDataToString().compare(_targetValue) == 0) {
            (*targetIter)->setData(_value, this->structureDataType);
            return true;
        }
    }
    return false;
}

const list<DataElement> StaticList::searchRange(ScanDataCondition& _condition) {

    list<DataElement> resultList;

    //컨디션 객체가 비어있을 경우 전부다 검색
    if(_condition.isEmpty()) {

        for(list<shared_ptr<DataElement>>::iterator iter = this-> valueList.begin();
            iter != this-> valueList.end(); iter++ ) {            
            resultList.push_back(**iter);
        }
        return resultList;

    } else {

        //범위가 숫자형일 경우
        if(_condition.conditionToken[0]->dcf == DCF_RANGE ) {
            
            //데이터타입이 틀리면 비어있는 리스트 리턴
            if( this->getStructureDataType() == DATATYPE_STRING) {
                return resultList;
            } else {
                IsInNumberRange isInNumberRange;
                for(list<shared_ptr<DataElement>>::iterator iter = this-> valueList.begin();
                    iter != this-> valueList.end(); iter++ ) {
                    
                    if(isInNumberRange( _condition, (**iter))) {
                        resultList.push_back(**iter);
                    }
                }

                return resultList;
            }
        } else if(_condition.conditionToken[0]->dcf == DCF_STRING ) {

            if( this->getStructureDataType() != DATATYPE_STRING) {
                return resultList;
            } else {
                IsInStringRange isInStringrRange;
                for(list<shared_ptr<DataElement>>::iterator iter = this-> valueList.begin();
                    iter != this-> valueList.end(); iter++ ) {
                    
                    if(isInStringrRange( _condition, (**iter))) {
                        resultList.push_back(**iter);
                    }
                }

                return resultList;
            }
        } else {
            return resultList;
        }
    }

}


//데이터 삭제
const bool StaticList::deleteRange(ScanDataCondition& _condition) {

    //비어있는 경우 false 리턴
    if( _condition.isEmpty() ) return false;
    else {
        if(_condition.conditionToken[0]->dcf == DCF_RANGE ) {

            if(this->getStructureDataType() == DATATYPE_STRING) {
                return false;
            } else {
                IsInNumberRange isInNumberRange;
                for(list<shared_ptr<DataElement>>::iterator iter = this->valueList.begin(); 
                        iter != this->valueList.end(); iter++ ) {

                    if(isInNumberRange( _condition, (**iter))) {
                        iter = this->valueList.erase(iter);
                        if(this->valueList.empty()) {
                            updateLength();
                            return true;
                        }
                        //다음 위치를 가리키므로 데이터를 건너띄는것을 
                        //방지하기 위해 1칸 앞으로 땡기기
                        iter--;
                    }

                }
                updateLength();
                return true;
            }

        } else if(_condition.conditionToken[0]->dcf == DCF_STRING) {

            if(this->getStructureDataType() != DATATYPE_STRING) {
                return false;
            } else {

                IsInStringRange isInStringrRange;
                for(list<shared_ptr<DataElement>>::iterator iter = this->valueList.begin(); 
                        iter != this->valueList.end(); iter++ ) {

                    if(isInStringrRange( _condition, (**iter))) {
                        iter = this->valueList.erase(iter);
                        if(this->valueList.empty()) {
                            updateLength();
                            return true;
                        }
                        //다음 위치를 가리키므로 데이터를 건너띄는것을 
                        //방지하기 위해 1칸 앞으로 땡기기
                        iter--;
                    }
                }
                updateLength();
                return true;
            }
        } else {
            return false;
        }
    }
    return false;
}
