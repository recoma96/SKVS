/***
 *  @file : DynamicList.hpp
 * 	@date 2019/08/14
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : DynamicList
 * 
 ***/
#ifndef DYNAMICLIST_HPP
# define DYNAMICLIST_HPP

#include "ListLayer.hpp"
#include "StructureUtility.hpp"

#include <list>
#include <map>
#include <string>
#include <vector>

using namespace std;

namespace structure {

    class DynamicList : public ListLayer, public DynamicStructureUtility {
    public:
        DynamicList(const string _value);
        ~DynamicList();

        // < 해당 인덱스 데이터, 그 데이터에 대한 하위 데이터 포인터 >
        // list에서 탐색하고 value/link map에서 추출
        list<shared_ptr<DataElement>>::iterator operator[](const unsigned int _idx);

        //from ListLayer
        const bool insertValue(const string _value);
        const list<DataElement> searchIndex(const string _indexCondition);
        const bool deleteIndex(const string _indexCondition);
        const bool updateDataByIndex( const unsigned int _targetIndex, const string _newValue );
        const bool updateData( const string _targetValue, const string _value);

        //from DynamicStructureUtility
        //StructureUtility
        const list<DataElement> searchRange(ScanDataCondition& _condition);
        const bool deleteRange(ScanDataCondition& _condition);

        //Dynamic
        const bool linkChildElement(const string _targetKey, MultiDataStructure* _targetElement);
        const bool unlinkChildElement(const string _targetKey);

        //상위객체이므로 포인터를 이용해서 반환을 하고
        //다른 함수로 다운캐스팅을 해서 사용해야 합니다.

        //throws DataLinkException
        MultiDataStructure& getChildElementByKey(const string _key);
        MultiDataStructure& getChildElementbyIndex(const unsigned int _idx);

    };
}

#endif