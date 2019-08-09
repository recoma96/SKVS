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
