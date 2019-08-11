/***
 *  @file : MultiSet.hpp
 * 	@version : 0.0.1
 * 	@date 2019/08/11
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 같은 원소를 여러개 삽입할 수 있는 Set 입니다.
 *  @license : MIT-License
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
		explicit MultiSet(const string _keyData, const DataType _valueDataType ) :
			SetLayer( _keyData, _valueDataType, STRUCTTYPE_MULTISET ) { }
		~MultiSet() noexcept { }

		//throw DataTypeException
		bool insertValue(const string _value );
		const unsigned int coutValue(const string _value);

	};


}



#endif
