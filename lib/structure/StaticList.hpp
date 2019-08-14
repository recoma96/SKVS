/***
 *  @file : StaticList.hpp
 * 	@date 2019/08/14
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : StaticList
 * 
 ***/

#ifndef STATICLIST_HPP
# define STATICLIST_HPP

#include "ListLayer.hpp"
#include "StructureUtility.hpp"

#include <list>
#include <string>
#include <vector>

using namespace std;

namespace structure {

    class StaticList : public ListLayer, public StaticStructureUtility {
    public:
        StaticList(const string _value, const DataType _listDataType);

        //operator -> [] 데이터의 수정/삭제기능을 추가하기 위해
        //리턴값을 iterator로 설정했습니다.

        list<shared_ptr<DataElement>>::iterator operator[](const unsigned int _idx);
        
        //from ListLayer
        const bool insertValue(const string _value);
        const list<DataElement> searchIndex(const string _indexCondition);
        const bool deleteIndex(const string _indexCondition);
        const bool updateDataByIndex( const unsigned int _targetIndex, const string _newValue );
        const bool updateData( const string _targetValue, const string _value);

        //from StaticStructureUtility
        const list<DataElement> searchRange(ScanDataCondition& _condition);
        const bool deleteRange(ScanDataCondition& _condition);
    };
}

#endif