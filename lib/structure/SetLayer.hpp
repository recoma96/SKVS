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
		virtual bool insertValue(const string) = 0;
	};
}


#endif
