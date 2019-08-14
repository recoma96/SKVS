/***
 *  @file : Set.hpp
 * 	@date 2019/08/14
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : MultiSet.hpp
 * 
 ***/

#ifndef MULTISET_HPP
# define MULTISET_HPP

#include "SetLayer.hpp"
#include <string>

using namespace std;
namespace structure {

    class MultiSet : public SetLayer {
    public:
        MultiSet(const string _key, const DataType _setDataType) : 
            SetLayer(_key, STRUCTTYPE_MULTISET, _setDataType) { }
        ~MultiSet() { }


        //throw exception -> DataTypeException;
        const bool insertValue(const string _value);
    };
}


#endif