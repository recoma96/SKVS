/***
 *  @file : Basic.hpp
 * 	@date 2019/08/11
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 전형적인 key-value 형식의 데이터구조입니다
 * 
 ***/


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
