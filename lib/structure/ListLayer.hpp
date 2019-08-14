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
#include "../Tokenizer.hpp"

#include <list>
#include <memory>
#include <string>

namespace structure {

    //인덱스 범위 판정
    struct IsInIndex {
        inline const bool operator() (vector<string> toked, const struct_len_t length) {

            //string<vector> toked = tok::tokenizer(_indexCondition);

            if( toked.size() != 2 ) return false;

            CheckDataType checkDataType;
            if(!checkDataType(toked[0], DATATYPE_NUMBER) && !checkDataType(toked[1], DATATYPE_NUMBER))
                return false;

            const int minIdx = atoi(toked[0].c_str());
            const int maxIdx = atoi(toked[1].c_str());

            //범위 계산
            if( !(minIdx >= 0 && maxIdx < length))
                return false;


            if( !( minIdx <= maxIdx ) )
                return false;

            return true;
        }
    };

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