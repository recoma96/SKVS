#include "MultiSet.hpp"

#include <string>
#include <set>

using namespace std;
using namespace structure;

const bool MultiSet::insertValue(const string _value) {

    //데이터타입에 따른 데이터 적합성 판정
    CheckDataType checkDataType;
    
    if(!checkDataType(_value, this->structureDataType))
        throw DataTypeException("This DataType is not machted.");

    shared_ptr<DataElement> insertData(new DataElement(
                                                _value, 
                                                this->structureDataType,
                                                STRUCTTYPE_ELEMENT
                                                ));
    //데이터 삽입
    this->valueSet.insert( insertData );
    updateLength();

    return true;

}