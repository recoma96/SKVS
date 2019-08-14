/***
 *  @file : SetLayer.hpp
 * 	@date 2019/08/14
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : Set의 추상화클래스입니다.
 * 
 ***/


#ifndef SETLAYER_HPP
# define SETLAYER_HPP

#include "Set.hpp"

namespace structure {
    class SetLayer : public Set {
    public:
        SetLayer(const string _key,  const StructType _structType, const DataType _setDataType) :
            Set(_key, _structType, _setDataType) { }

        //TODO 여기서부터
        virtual const bool insertValue(const string _value) = 0;
    };
}

#endif