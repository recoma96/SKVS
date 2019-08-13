/***
 *  @file : Structure.hpp
 * 	@date 2019/08/11
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : SDKVS의 최상위 데이터구조 추상화 클래스입니다.
 * 
 ***/

#ifndef STRUCTURE_HPP
# define STRUCTURE_HPP
#include "DataElement.hpp"

#include <string>

using namespace std;

namespace structure {


	class Structure : public DataElement {
	public:
		explicit Structure( const string _data, const DataType _dataType, const StructType _structType ) :
			DataElement( _data, _dataType, _structType ) { }

	};


}

#endif
