/***
 *  @file : Set.hpp
 * 	@date 2019/08/14
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : OneSet
 * 
 ***/

#ifndef ONESET_HPP
# define ONESET_HPP

#include "SetLayer.hpp"
#include <string>

using namespace std;
namespace structure {

    class OneSet : public SetLayer {
    public:
        OneSet(const string _key, const DataType _setDataType) : 
            SetLayer(_key, STRUCTTYPE_ONESET, _setDataType) { }
        ~OneSet() { }


        //throw exception -> DataTypeException;
        const bool insertValue(const string _value);
    };
}


#endif