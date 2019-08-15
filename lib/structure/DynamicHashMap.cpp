#include "DynamicHashMap.hpp"

#include <map>
#include <list>
#include <string>

using namespace std;
using namespace structure;

DynamicHashMap::DynamicHashMap(const string _data) : HashMapLayer(_data, STRUCTTYPE_DYNAMICHASHMAP) {

}

DynamicHashMap::~DynamicHashMap() {

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

const bool DynamicHashMap::insertKeyValue(const string _newKey, const string _newValue) {

    //해당 키의 유일성 판단
    map<string, shared_ptr<DataElement>>::iterator iter = this->dataMap.find(_newKey);
    if( iter != this->dataMap.end() ) return false;

    shared_ptr<DataElement> insertValueData( new DataElement( 
                                                        _newValue,
                                                        DATATYPE_STRING,
                                                        STRUCTTYPE_ELEMENT
                                                        ));
    
    //데이터 삽입
    this->dataMap.insert(pair<string, shared_ptr<DataElement>>(_newKey, insertValueData));

    //링크리스트에도 삽입
    this->linkMap.insert(pair<shared_ptr<DataElement>, MultiDataStructure*>(insertValueData->getSharedPtr(), nullptr));
    updateLength();
    return true;
    
}


const bool DynamicHashMap::updateValue(const string _targetKey, const string _newValue) {

    //데이터 찾기
    map<string, shared_ptr<DataElement>>::iterator foundIter = this->dataMap.find(_targetKey);
    if( foundIter == this->dataMap.end()) return false;

    //데이터 수정
    foundIter->second->setData(_newValue, DATATYPE_STRING);
    return true;
}


const list<DataElement> DynamicHashMap::searchRange(ScanDataCondition& _condition) {

    list<DataElement> resultList;

    if(_condition.isEmpty()) {

        for(map<string, shared_ptr<DataElement>>::iterator iter = this-> dataMap.begin();
            iter != this-> dataMap.end(); iter++ ) {            
            resultList.push_back(*(iter->second));
        }
        return resultList;
    } else {

        //string만 취급
        if( _condition.conditionToken[0]->dcf != DCF_STRING) {
            return resultList;
        } else {

            IsInStringRange isInStringRange;
            for(map<string, shared_ptr<DataElement>>::iterator iter = this->dataMap.begin();
                    iter != this->dataMap.end(); iter++ ) {

                DataElement compareString(iter->first, DATATYPE_STRING, STRUCTTYPE_ELEMENT);
                if(isInStringRange(_condition, compareString)) {
                    resultList.push_back( *(iter->second) );
                }
            }
            return resultList;
        }
    }
}

const bool DynamicHashMap::deleteRange(ScanDataCondition& _condition) {

    if(_condition.isEmpty()) {
        return false;
    } else {
        if(_condition.conditionToken[0]->dcf != DCF_STRING) {
            return false;
        } else {
            IsInStringRange isInStringRange;
            for(map<string, shared_ptr<DataElement>>::iterator iter = this->dataMap.begin(); 
                iter != this->dataMap.end(); iter++ ) {
                
                DataElement compareString(iter->first, DATATYPE_STRING, STRUCTTYPE_ELEMENT);
                if(isInStringRange(_condition, compareString)) {
                    
                    //연결 링크 존재 여부 확인
                    map<shared_ptr<DataElement>, MultiDataStructure*>::iterator link = 
                        this->linkMap.find(iter->second);
                    
                    if( link->second != nullptr ) {
                        link->second->unlinkParentElement();
                        link->second = nullptr;
                    }
                    iter = this->dataMap.erase(iter);
                    iter--;
                }
            }
            updateLength();
            return true;
        }
    }
}

//link/unlink fundtion
const bool DynamicHashMap::linkChildElement(const string _targetKey, MultiDataStructure* _targetElement) {

    //false 해당 키에 링크가 존재
    if( _targetElement->isLinkedByParent() )
        throw DataLinkException("This target element has already parent Element");
    
    //키 탐색
    map<string, shared_ptr<DataElement>>::iterator foundValue = this->dataMap.find(_targetKey);
    if(foundValue == this->dataMap.end())
        throw DataLinkException("Key is not found.");
    
    //링크 탐색
    map<shared_ptr<DataElement>, MultiDataStructure*>::iterator foundLink 
        = this->linkMap.find(foundValue->second);
    
    //링크 유무 여부
    if( foundLink->second != nullptr ) return false;
    
    //targetElement로부터 연결
    _targetElement->setParentElement(foundValue->second);

    //상위에도 연결
    foundLink->second = _targetElement;
    return true;
}

const bool DynamicHashMap::unlinkChildElement(const string _targetKey) {

    //키 탐색
    map<string, shared_ptr<DataElement>>::iterator foundValue = this->dataMap.find(_targetKey);
    if(foundValue == this->dataMap.end())
        throw DataLinkException("Key is not found.");
    
    //링크 탐색
    map<shared_ptr<DataElement>, MultiDataStructure*>::iterator foundLink = 
        this->linkMap.find(foundValue->second);
    
    if(foundLink->second == nullptr) return false;

    foundLink->second->unlinkParentElement();
    foundLink->second = nullptr;

    return true;

}

MultiDataStructure& DynamicHashMap::getChildElementByKey(const string _key) {
    //키 탐색
    map<string, shared_ptr<DataElement>>::iterator foundValue = this->dataMap.find(_key);
    if(foundValue == this->dataMap.end())
        throw DataLinkException("Key is not found.");
    
    //링크 탐색
    map<shared_ptr<DataElement>, MultiDataStructure*>::iterator foundLink = 
        this->linkMap.find(foundValue->second);
    
    if(foundLink->second == nullptr)
        throw DataLinkException("This key has not Link");
    return *(foundLink->second);
}