/***
 *  @file : HashMap.hpp
 * 	@date 2019/08/14
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : HashMap 최상위 클래스
 * 
 ***/

#ifndef HASHMAP_HPP
# define HASHMAP_HPP

#include "MultiDataStructureLayer.hpp"
#include "../Exception.hpp"

#include <list>
#include <map>
#include <string>

using namespace std;

namespace structure {
    class HashMap : public MultiDataStructureLayer {
    protected:
        map<string, shared_ptr<DataElement>> dataMap;
        void updateLength(void) { this->length = dataMap.size(); }
    public:
        explicit HashMap(const string _data, const StructType _structType ) : 
            MultiDataStructureLayer(_data, _structType) { }
        ~HashMap() {
            dataMap.clear();
        }

        const d_size_t getStructSize(void) noexcept {
            d_size_t resultSize = 0;
            for(map<string, shared_ptr<DataElement>>::iterator iter = dataMap.begin();
                iter != dataMap.end(); iter++ ) {

                resultSize += iter->first.length();
                resultSize += iter->second->getSize();
                
            }
            return resultSize;
        }
        const vector<string> getKey(void) noexcept {
            vector<string> resultVec;
            for(map<string, shared_ptr<DataElement>>::iterator iter = dataMap.begin();
                iter != dataMap.end(); iter++ ) {
                resultVec.push_back(iter->first);
            }
            return resultVec;
        }
    };
}



#endif