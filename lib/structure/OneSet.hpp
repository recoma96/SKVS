/***
 *  @file : OneSet.hpp
 * 	@version : 0.0.1
 * 	@date 2019/08/11
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 일반적인 Set 입니다.
 *  @license : MIT-License
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
		explicit OneSet( const string _keyData, const DataType _valueDataType) : 
			SetLayer( _keyData, _valueDataType, STRUCTTYPE_ONESET) { }
		~OneSet() noexcept { }
	
		//Throw DataTypeException
		bool insertValue(const string _value );
	};
}

#endif
