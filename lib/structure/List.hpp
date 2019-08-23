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

    //리스트를 sort할 때 사용하는 객체함수
    struct SortStringListBackward {
        bool operator()( shared_ptr<DataElement> first, shared_ptr<DataElement> second ) {
            return (first->getDataToString().compare(second->getDataToString()) > 0);
        }
    };
    struct SortNumberListBackward {
        bool operator() ( shared_ptr<DataElement> first, shared_ptr<DataElement> second ) {
            return first->getData<number_t>() > second->getData<number_t>();
        }
    };
    struct SortFloatListBackward {
        bool operator() ( shared_ptr<DataElement> first, shared_ptr<DataElement> second ) {
            return first->getData<float_t>() > second->getData<float_t>();
        }
    };
    struct SortStringListForward {
        bool operator()( shared_ptr<DataElement> first, shared_ptr<DataElement> second ) {
            return (first->getDataToString().compare(second->getDataToString()) < 0);
        }
    };
    struct SortNumberListForward {
        bool operator() ( shared_ptr<DataElement> first, shared_ptr<DataElement> second ) {
            return first->getData<number_t>() < second->getData<number_t>();
        }
    };
    struct SortFloatListForward {
        bool operator() ( shared_ptr<DataElement> first, shared_ptr<DataElement> second ) {
            return first->getData<float_t>() < second->getData<float_t>();
        }
    };

    //sort 플래그
    enum SortFlag {
        SORTFLAG_ASEN,
        SORTFLAG_DESN
    };

    class List : public MultiDataStructureLayer {
    protected:
        list<shared_ptr<DataElement>> valueList;
        void updateLength(void) { this->length = valueList.size(); }
    public:

        explicit List( const string _data, const StructType _structType ) : 
            MultiDataStructureLayer(_data, _structType) { }
            
        ~List() {
            valueList.clear();
        }
            
        const d_size_t getStructSize(void) noexcept { 
            
            d_size_t resultSize = 0;
            for( list<shared_ptr<DataElement>>::iterator iter = valueList.begin();
                iter != valueList.end(); iter++ )
                resultSize += (*iter)->getSize();
            
            return resultSize;
        }

        //test
        void display(void) {

            for(list<shared_ptr<DataElement>>::iterator iter = this->valueList.begin(); 
                iter != this->valueList.end(); iter++ ) {
                cout << (*iter)->getDataToString() << endl;
            }
        }

        void sort(SortFlag sortFlag) {
            if(!valueList.empty()) {
                if(sortFlag == SORTFLAG_ASEN) { //오름차순
                    switch( (*(valueList.begin()))->getDataType()) {
                        case DATATYPE_STRING:
                            valueList.sort(SortStringListForward());
                        break;
                        case DATATYPE_NUMBER:
                            valueList.sort(SortNumberListForward());
                        break;
                        case DATATYPE_FLOAT:
                            valueList.sort(SortFloatListForward());
                        break;
                    }
                } else { //내림차순
                    switch((*(valueList.begin()))->getDataType()) {
                        case DATATYPE_STRING:
                            valueList.sort(SortStringListBackward());
                        break;
                        case DATATYPE_NUMBER:
                            valueList.sort(SortNumberListBackward());
                        break;
                        case DATATYPE_FLOAT:
                            valueList.sort(SortFloatListBackward());
                        break;
                    }
                }
            }
        }
    };
}
#endif