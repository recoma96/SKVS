/***
 *  @file : Set.hpp
 * 	@date 2019/08/13
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : Set의 최상위클래스입니다.
 * 
 ***/

#ifndef SET_HPP
# define SET_HPP

#include "MultiDataStructureLayer.hpp"
#include "StructureUtility.hpp"

#include <memory>
#include <string>
#include <set>
#include <list>


using namespace std;

namespace structure {

    class Set : public MultiDataStructureLayer, public StaticStructureUtility {
    protected:
        set<shared_ptr<DataElement>> valueSet;

        void updateLength(void) { this->length = static_cast<struct_len_t>(valueSet.size()); }
    public:

        //생성x소멸자
        Set(const string _key,  const StructType _structType, const DataType _setDataType);
        ~Set();

        //from MultiDataStructureLayer
        const d_size_t getStructSize(void);

        //from StaticStructureUtility
        const list<DataElement> searchRange(ScanDataCondition& _condition);
        const bool deleteRange(ScanDataCondition& _condition);
    
    };

}   
#endif