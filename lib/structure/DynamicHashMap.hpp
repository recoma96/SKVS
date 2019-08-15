/***
 *  @file : DynamicHashMap.hpp
 * 	@date 2019/08/15
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : DynamicHashMap
 ***/

#ifndef DYNAMICHASHMAP_HPP
# define DYNAMICHASHMAP_HPP

#include "HashMapLayer.hpp"
#include "StructureUtility.hpp"

#include <map>
#include <list>
#include <string>

using namespace std;

namespace structure {
    class DynamicHashMap : public HashMapLayer, public DynamicStructureUtility {
    public:
        DynamicHashMap(const string _data);
        ~DynamicHashMap();
    
        //from HashMapLayer
        const bool insertKeyValue(const string _newKey, const string _newValue);
        const bool updateValue(const string _targetKey, const string _newValue);

        //from DynamicStructureUtility
        //StructureUtility
        const list<DataElement> searchRange(ScanDataCondition& _condition);
        const bool deleteRange(ScanDataCondition& _condition);

        //Dynamic
        const bool linkChildElement(const string _targetKey, MultiDataStructure* _targetElement);
        const bool unlinkChildElement(const string _targetKey);

        MultiDataStructure& getChildElementByKey(const string _key);

    };
}

#endif
