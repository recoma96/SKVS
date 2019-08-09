#ifndef BASIC_HPP
# define BASIC_HPP

#include "Structure.hpp"
#include <string>

using namespace std;

namespace structure {


	class Basic : public Structure {

	private:
		DataElement* value;
	public:
		//throw DataVaildException
		explicit Basic( const string _keyData, const string _valueData, const DataType _valueDataType );
		~Basic();

		template <typename T>
		T getValue(void) const noexcept;

		string getValueToString(void) const noexcept;
		DataType getValueDataType(void) const noexcept;
		StructType getValueStructType(void) const noexcept;
		d_size_t getValueSize(void) const noexcept;
		void setValue(string _value, DataType _valueType) const noexcept;


	};


}


#endif
