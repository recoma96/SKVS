/***
 *  @file : Structureutility.hpp
 * 	@date 2019/08/12
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 데이터컨테이너의 인터페이스(추상클래스) 입니다.
 * 
 ***/

#include "ScanDataCondition.hpp"
#include "MultiDataStructure.hpp"

#include <string>
#include <list>
#include <map>

using namespace std;

namespace structure {

    class StructureUtility {
    public:
        virtual const list<DataElement> searchRange(ScanDataCondition& _condition) = 0;
        virtual const bool deleteRange(ScanDataCondition& _condition) = 0; 
    };


    //Static형 컨테이너용 인터페이스(정확히는 Abstract Class)
    class StaticStructureUtility : public StructureUtility {
    protected:
        DataType structureDataType; //Static형 컨테이너는 정해진 데이터타입에 따라 데이터를 삽입합니다.
    public:
        //StructureUtility
        virtual const list<DataElement> searchRange(ScanDataCondition& _condition) = 0;
        virtual const bool deleteRange(ScanDataCondition& _condition) = 0; 

        //Static
        const DataType getStructureDataType(void) noexcept { return this->structureDataType; }
    };

    //Dynamic형 컨테이너용 인터페이스(Abstrct Class)
    class DynamicStructureUtility : public StructureUtility {
    protected:
        map<shared_ptr<DataElement>, MultiDataStructure*> linkMap;
    public:
        //StructureUtility
        virtual const list<DataElement> searchRange(ScanDataCondition& _condition) = 0;
        virtual const bool deleteRange(ScanDataCondition& _condition) = 0;

        //Dynamic
        virtual const bool linkChildElement(string _targetKey, MultiDataStructure* _targetElement) = 0;
        virtual const bool unlinkChildElement(string _targetKey) = 0;
    };

}

