#include "Set.hpp"

#include <memory>
#include <string>
#include <set>
#include <list>

using namespace std;
using namespace structure;


Set::Set(const string _key,  const StructType _structType, const DataType _setDataType) : 
    MultiDataStructureLayer(_key, _structType ){

    this->structureDataType= _setDataType;

}

Set::~Set() {
    this->valueSet.clear();
}

//from MultiDataStructureLayer
const d_size_t Set::getStructSize(void) {

    d_size_t resultSize = 0;

    for(set<shared_ptr<DataElement>>::iterator iter = this->valueSet.begin(); 
        iter != this->valueSet.end(); iter++ ) {
        
        resultSize += (*iter)->getSize();
    }

    return resultSize;
}

//from MultiDatStructureLayer
//데이터 검색
const list<DataElement> Set::searchRange(ScanDataCondition& _condition) {

    list<DataElement> resultList;

    //컨디션 객체가 비어있을 경우 전부다 검색
    if(_condition.isEmpty()) {

        for(set<shared_ptr<DataElement>>::iterator iter = this->valueSet.begin();
            iter != this->valueSet.end(); iter++ ) {            
            resultList.push_back(**iter);
        }

    } else {

        //범위가 숫자형일 경우
        if(_condition.conditionToken[0]->dcf == DCF_RANGE ) {
            
            //데이터타입이 틀리면 비어있는 리스트 리턴
            if( this->getStructureDataType() == DATATYPE_STRING) {
                return resultList;
            } else {
                IsInNumberRange isInNumberRange;
                for(set<shared_ptr<DataElement>>::iterator iter = this->valueSet.begin();
                    iter != this->valueSet.end(); iter++ ) {
                    
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
                for(set<shared_ptr<DataElement>>::iterator iter = this->valueSet.begin();
                    iter != this->valueSet.end(); iter++ ) {
                    
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
const bool Set::deleteRange(ScanDataCondition& _condition) {

    //비어있는 경우 false 리턴
    if( _condition.isEmpty() ) return false;
    else {
        if(_condition.conditionToken[0]->dcf == DCF_RANGE ) {

            if(this->getStructureDataType() == DATATYPE_STRING) {
                return false;
            } else {
                IsInNumberRange isInNumberRange;
                for(set<shared_ptr<DataElement>>::iterator iter = this->valueSet.begin(); 
                        iter != this->valueSet.end(); iter++ ) {

                    if(isInNumberRange( _condition, (**iter))) {
                        iter = this->valueSet.erase(iter);

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
                for(set<shared_ptr<DataElement>>::iterator iter = this->valueSet.begin(); 
                        iter != this->valueSet.end(); iter++ ) {

                    if(isInStringrRange( _condition, (**iter))) {
                        iter = this->valueSet.erase(iter);

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



void Set::display(void) {
    for(set<shared_ptr<DataElement>>::iterator iter = this->valueSet.begin();
        iter != this->valueSet.end(); iter++ ) {
        
        cout << (*iter)->getDataToString() << endl;
    }
}