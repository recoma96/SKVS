/***
 *  @file : HashMapLayer.hpp
 * 	@date 2019/08/15
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : HashMap 추상화 클래스
 * 
 ***/

#ifndef HASHMAPLAYER_HPP
# define HASHMAPLAYER_HPP

#include "HashMap.hpp"

namespace structure {
    class HashMapLayer : public HashMap {
    public:
        HashMapLayer(const string _data, const StructType _structType ) :
            HashMap(_data, _structType ) { }
        
        virtual const bool insertKeyValue(const string _newKey, const string _newValue) = 0;
        //virtual const bool updateKey( const string _targetKey, const string _newKey) = 0;
        virtual const bool updateValue(const string _targetKey, const string _newValue) = 0;

    };
}

#endif