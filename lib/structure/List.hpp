/***
 *  @file : List.hpp
 * 	@date 2019/08/14
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 리스트 계열의 최상위 클래스 입니다.
 * 
 ***/


#ifndef LIST_HPP
# define LIST_HPP

#include "MultiDataStructureLayer.hpp"

#include <list>
#include <memory>
#include <string>


namespace structure {

    class List : public MultiDataStructureLayer {
    protected:
        list<shared_ptr<DataElement>> valueList;
        void updateLength(void) { this->length = valueList.size(); }
    public:

        explicit List( const string _data, const StructType _structType ) : 
            MultiDataStructureLayer(_data, _structType) { }
            
        const d_size_t getStructSize(void) noexcept { 
            
            d_size_t resultSize = 0;
            for( list<shared_ptr<DataElement>>::iterator iter = valueList.begin();
                iter != valueList.end(); iter++ )
                resultSize += (*iter)->getSize();
            
            return resultSize;
        }
    };
}
#endif