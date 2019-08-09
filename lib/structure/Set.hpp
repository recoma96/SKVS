#ifndef SET_HPP
# define SET_HPP	

#include "MultiDataStructure.hpp"
#include "ScanDataCondition.hpp"

#include <string>
#include <set>
#include <list>

using namespace std;


namespace structure {

	class Set : public MultiDataStructure {
	private:	
		set<DataElement*> valueSet;
		DataType dataType;
	protected:
		void updateLength(void) noexcept; //from MultiDataStructure.hpp
	public:
		explicit Set( const string _keyData, const DataType _valueDataType, 
						const StructType _structType);
		~Set() noexcept;

		//virtual function
		//데이터 탐색
		list<DataElement*> searchRange(ScanDataCondition& _condition);
		//데이터 삭제
		bool deleteRange(ScanDataCondition& _condition);
		//데이터 크기
		d_size_t getStructSize(void) noexcept;

	};

}



#endif
