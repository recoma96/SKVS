/***
 *  @file : Set.hpp
 * 	@version : 0.0.1
 * 	@date 2019/08/11
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : Set의 최상위 클래스
 *  @license : MIT-License
 * 
 ***/


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
	protected:
		set<DataElement*> valueSet;
		DataType valueDataType;

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
