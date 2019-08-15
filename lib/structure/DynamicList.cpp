#include "DynamicList.hpp"
#include "../Exception.hpp"
#include "../Tokenizer.hpp"

#include <list>
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace structure;

DynamicList::DynamicList(const string _key) : ListLayer(_key, STRUCTTYPE_DYNAMICLIST) {
    
}


DynamicList::~DynamicList() {

    //링크를 가지고있지 않을 경우 -> nullptr
    //가지고있는 경우 non nullptr

    //링크리스트를 순회해서 연결된 부분 끊어버리기 
    for(map<shared_ptr<DataElement>, MultiDataStructure*>::iterator deleteIter = this->linkMap.begin();
            deleteIter != this->linkMap.end(); deleteIter++ ) {
        if( deleteIter->second != nullptr ) {
            //하위에서 상위 링크 끊기
            deleteIter->second->unlinkParentElement();
        }
    }
    this->linkMap.clear();

}

list<shared_ptr<DataElement>>::iterator DynamicList::operator[](const unsigned int _idx) {
    
    //시간상 알고리즘을 최적화하지 못했습니다.
    //차후에 수정할 예정입니다.
    
    //범위를 벗어난 인덱스는 end()를 리턴합니다.
    if( _idx >= this->length ) return this->valueList.end();

    list<shared_ptr<DataElement>>::iterator resultIter = this->valueList.begin();
    advance(resultIter, _idx);

    return resultIter;

}

const bool DynamicList::insertValue(const string _value) {  

    //해당 값의 유일성을 판단.
    for(list<shared_ptr<DataElement>>::iterator iter = this->valueList.begin();
        iter != this->valueList.end(); iter++ ) {
        if( (*iter)->getDataToString().compare(_value) == 0 ) return false;
    }

    //데이터 삽입

    shared_ptr<DataElement> insertData(new DataElement(
                                                    _value,
                                                    DATATYPE_STRING,
                                                    STRUCTTYPE_ELEMENT
                                                    ));

    //리스트 추가
    this->valueList.push_back(insertData);
    //링크리스트에서도 삽입
    this->linkMap.insert(pair<shared_ptr<DataElement>, MultiDataStructure*>(insertData->getSharedPtr(), nullptr));

    updateLength();
    return true;
}

const list<DataElement> DynamicList::searchIndex(const string _indexCondition) {

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

const bool DynamicList::deleteIndex(const string _indexCondition) {

    vector<string> toked = tok::tokenizer(_indexCondition);
    IsInIndex isInIndex;

    if( !isInIndex( toked, this->length))
        throw DataConditionException("This Index Condition is vailed.");
    
    int minIdx = atoi(toked[0].c_str());
    int maxIdx = atoi(toked[1].c_str());

    list<shared_ptr<DataElement>>::iterator idxIter = (*this)[minIdx];
    int counter = minIdx;

    while(counter <= maxIdx) {
        
        //순회할대마다 링크검색
        map<shared_ptr<DataElement>, MultiDataStructure*>::iterator link = 
            this->linkMap.find(*idxIter);
        
        link->second->unlinkParentElement();
        this->linkMap.erase(link);

        idxIter = this->valueList.erase(idxIter);
        counter++;
    }
    updateLength();

    return true;
}

const bool DynamicList::updateDataByIndex( const unsigned int _targetIndex, const string _newValue ) {

    //범위 판정
    if( _targetIndex >= this->length) return false;

    //데이터 입력
    list<shared_ptr<DataElement>>::iterator targetIter = (*this)[_targetIndex];
    (*targetIter)->setData(_newValue, DATATYPE_STRING );
    return true;
}

const bool DynamicList::updateData(const string _targetValue, const string _value) {

    
    //수정 할 데이터 찾기
    for(list<shared_ptr<DataElement>>::iterator targetIter = this->valueList.begin();
        targetIter != this->valueList.end(); targetIter++ ) {
        if( (*targetIter)->getDataToString().compare(_value) == 0) {
            (*targetIter)->setData(_value, DATATYPE_STRING);
            return true;
        }
    }
    return false;
}

const list<DataElement> DynamicList::searchRange(ScanDataCondition& _condition) {

    list<DataElement> resultList;
    //컨디션 객체가 비어있을 경우 전부다 검색

    if(_condition.isEmpty()) {

        for(list<shared_ptr<DataElement>>::iterator iter = this-> valueList.begin();
            iter != this-> valueList.end(); iter++ ) {            
            resultList.push_back(**iter);
        }
        return resultList;

    } else {

        //condition은 String만 취급
        if(_condition.conditionToken[0]->dcf == DCF_RANGE || 
           _condition.conditionToken[0]->dcf == DCF_LOGIC ) return resultList;
        else {

            IsInStringRange isInStringRange;
            for(list<shared_ptr<DataElement>>::iterator iter = this->valueList.begin();
                    iter != this->valueList.end(); iter++ ) {
                    
                    if(isInStringRange( _condition, (**iter))) {
                        resultList.push_back(**iter);
                }
            }
            return resultList;
        }
    }

}


//데이터 삭제
const bool DynamicList::deleteRange(ScanDataCondition& _condition) {

    if(_condition.isEmpty()) return false;
    else {
        if(_condition.conditionToken[0]->dcf == DCF_RANGE || 
           _condition.conditionToken[0]->dcf == DCF_LOGIC ) return false;
        else {
            IsInNumberRange isInNumberRange;
                for(list<shared_ptr<DataElement>>::iterator iter = this->valueList.begin(); 
                        iter != this->valueList.end(); iter++ ) {

                    if(isInNumberRange( _condition, (**iter))) {

                        //링크 검색
                        map<shared_ptr<DataElement>, MultiDataStructure*>::iterator link = 
                            this->linkMap.find(*iter);

                        if( link->second != nullptr ) {
                            link->second->unlinkParentElement();
                            link->second = nullptr;
                        }

                        iter = this->valueList.erase(iter);

                        //다음 위치를 가리키므로 데이터를 건너띄는것을 
                        //방지하기 위해 1칸 앞으로 땡기기
                        iter--;
                }

            }
            updateLength();
            return true;
        }
    }
}

//from DynamicStructureUtility
const bool DynamicList::linkChildElement(const string _targetKey, MultiDataStructure* _targetElement) {

    //false : 해당 키에 링크가 존재

    //targetElement에 연결이 되어있는 지에 대한 검사
    if( _targetElement->isLinkedByParent() )
        throw DataLinkException("This target element has already parent Element");

    //키 탐색
    for(list<shared_ptr<DataElement>>::iterator iter = this->valueList.begin(); 
        iter != this->valueList.end(); iter++ ) {
        
        if( (*iter)->getDataToString().compare(_targetKey) == 0) {
            
            //링크 탐색
            map<shared_ptr<DataElement>, MultiDataStructure*>::iterator link = 
                this->linkMap.find(*iter);
            
            //이미 링크가 있는 경우
            if( link->second != nullptr ) return false;

            //targetElement로부터 먼저 데이터 링크
            _targetElement->setParentElement(*iter);

            //상위Element에서도 연결
            link->second = _targetElement;
            return true;

        }
    }

    //키 못찾음
    throw DataLinkException("Key is not found.");
}


const bool DynamicList::unlinkChildElement(const string _targetKey) {
    //false 링크없음

    //키 탐색
    for(list<shared_ptr<DataElement>>::iterator iter = this->valueList.begin(); 
        iter != this->valueList.end(); iter++ ) {
        
        if( (*iter)->getDataToString().compare(_targetKey) == 0) {
            
            //링크 탐색
            map<shared_ptr<DataElement>, MultiDataStructure*>::iterator link = 
                this->linkMap.find(*iter);

            if( link->second == nullptr) return false;
            //상대쪽에서 링크 해제
            link->second->unlinkParentElement();
            link->second = nullptr;

            return true;

        }
    }

    //키 못찾음
    throw DataLinkException("Key is not found.");
}

MultiDataStructure& DynamicList::getChildElementByKey(const string _key) {

    //데이터 탐색
    //키 탐색
    for(list<shared_ptr<DataElement>>::iterator iter = this->valueList.begin(); 
        iter != this->valueList.end(); iter++ ) {
        
        if( (*iter)->getDataToString().compare(_key) == 0) {
            
            //링크 탐색
            map<shared_ptr<DataElement>, MultiDataStructure*>::iterator link = 
                this->linkMap.find(*iter);

            if(link->second == nullptr)
                throw DataLinkException("This key has not Link");
            return *(link->second);
        }
    }

    //키 못찾음
    throw DataLinkException("Key is not found.");
}

MultiDataStructure& DynamicList::getChildElementbyIndex(const unsigned int _idx) {

    int idx = _idx;
    //인덱스 탐색
    if( _idx >= length )
        throw DataLinkException("This Index over the range");

    //데이터 검색
    list<shared_ptr<DataElement>>::iterator foundIter = (*this)[idx];

    //링크 검색
    map<shared_ptr<DataElement>, MultiDataStructure*>::iterator link = 
                this->linkMap.find(*foundIter);

    if(link->second == nullptr ) throw DataLinkException("This key has not Link");
    else return *(link->second);

}