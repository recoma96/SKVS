#include "StaticHashMap.hpp"
#include "../Tokenizer.hpp"
#include "../Exception.hpp"

#include <string>
#include <map>
#include <list>

using namespace std;
using namespace structure;

StaticHashMap::StaticHashMap(const string _key, const DataType _mapDataType) : 
    HashMapLayer(_key, STRUCTTYPE_STATICHASHMAP) {
    
    this->structureDataType = _mapDataType;
}

const bool StaticHashMap::insertKeyValue(const string _newKey, const string _newValue) {

    //값의 타입에 대한 적합성 판단
    CheckDataType checkDataType;
    if( !checkDataType(_newValue, this->structureDataType)) return false;

    //키 중복 조회
    map<string, shared_ptr<DataElement>>::iterator findKey = this->dataMap.find(_newKey);
    if( findKey != this->dataMap.end()) return false;

    //키 삽입
    shared_ptr<DataElement> insertData(new DataElement(
                                                _newValue,
                                                this->structureDataType,
                                                STRUCTTYPE_ELEMENT));
    this->dataMap.insert(pair<string, shared_ptr<DataElement>>(_newKey, insertData));
    updateLength();
    return true;
}

/* string 수정이 안됨
const bool StaticHashMap::updateKey( const string _targetKey, const string _newKey) {

    //키 조회
    map<string, shared_ptr<DataElement>>::iterator findKey = this->dataMap.find(_targetKey);
    if( findKey == this->dataMap.end()) return false;

    this->dataMap[_targetKey] = _newKey;
    //string insertKey = _newKey;
    //findKey->first.swap(insertKey);
    return true;
}
*/
const bool StaticHashMap::updateValue(const string _targetKey, const string _newValue) {

    //값의 타입에 대한 적합성 판단
    CheckDataType checkDataType;
    if( !checkDataType(_newValue, this->structureDataType)) return false;

    //키 중복 조회
    map<string, shared_ptr<DataElement>>::iterator findKey = this->dataMap.find(_targetKey);
    if( findKey == this->dataMap.end()) return false;

    findKey->second->setData(_newValue, this->structureDataType);
    return true;

}

//from StaticStructureUtility
const list<DataElement> StaticHashMap::searchRange(ScanDataCondition& _condition) {

    list<DataElement> resultList;

    if( _condition.isEmpty() ) {

        for(map<string, shared_ptr<DataElement>>::iterator iter = this->dataMap.begin();
            iter != this->dataMap.end(); iter++ ) {
            resultList.push_back( (*iter->second));
        }
        return resultList;
    } else {
        //해쉬맵은 String만 취급
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

const bool StaticHashMap::deleteRange(ScanDataCondition& _condition) {

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
                    iter = this->dataMap.erase(iter);
                    if( this->dataMap.empty()) {
                        updateLength();
                        return true;
                    }
                    iter--;
                }
            }
            updateLength();
            return true;
        }
    }
}