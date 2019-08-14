/***
 *  @file : ListLayer.hpp
 * 	@date 2019/08/14
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 리스트 계열의 최상위 추상화 클래스(인터페이스) 입니다.
 * 
 ***/

#ifndef LISTLAYER_HPP
# define LISTLAYER_HPP

#include "List.hpp"

#include <list>
#include <memory>
#include <string>

namespace structure {

    class ListLayer : public List {
    public:
        explicit ListLayer( const string _data, const StructType _structType ) : 
            List(_data, _structType) { }

        virtual const bool insertValue(const string _value) = 0;
        virtual const list<DataElement> searchIndex(const string _indexCondition) = 0;
        virtual const bool deleteIndex(const string _indexCondition) = 0;
        virtual const bool updateDataByIndex( const unsigned int _targetIndex, const string _newValue ) = 0;
        virtual const bool updateData( const string _targetValue, const string _value) = 0;
    };
}

#endif