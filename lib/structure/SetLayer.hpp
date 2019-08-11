/***
 *  @file : SetLayer.hpp
 * 	@version : 0.0.1
 * 	@date 2019/08/11
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : Set의 추상화 클래스입니다.
 *  @license : MIT-License
 * 
 ***/

#ifndef SETLAYER_HPP
# define SETLAYER_HPP

#include "Set.hpp"
#include "DataElement.hpp"

#include <string>

namespace structure {

	class SetLayer : public Set {
	public:
		explicit SetLayer(const string _keyData, const DataType _valueDataType, const StructType _structType) :
			Set( _keyData, _valueDataType, _structType) { }

		//데이터 삽입
		virtual bool insertValue(const string _value) = 0;
	};
}


#endif
