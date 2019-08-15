/***
 *  @file : StaticHashMap.hpp
 * 	@date 2019/08/15
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : StaticHashMap
 * 
 ***/

#ifndef STATICHASHMAP_HPP
# define  STATICHASHMAP_HPP

#include "StructureUtility.hpp"
#include "HashMapLayer.hpp"

#include <string>
#include <map>
#include <list>

using namespace std;
namespace structure {

    class StaticHashMap : public HashMapLayer, public StaticStructureUtility {
    public:
        StaticHashMap(const string _key, const DataType _mapDataType);

        //from HashMapLayer
        const bool insertKeyValue(const string _newKey, const string _newValue);

        //키 수정이 안됨
        //const bool updateKey( const string _targetKey, const string _newKey);
        const bool updateValue(const string _targetKey, const string _newValue);

        //from StaticStructureUtility
        const list<DataElement> searchRange(ScanDataCondition& _condition);
        const bool deleteRange(ScanDataCondition& _condition);
        
    };
}


#endif