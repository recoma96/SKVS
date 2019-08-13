/***
 *  @file : DataStructure.hpp
 * 	@date 2019/08/12
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 두개 이상의 데이터를 담는 데이터구조의 최상위 클래스 입니다.
 * 
 ***/

#ifndef MULTIDATASTRUCTURE_HPP
# define MULTIDATASTRUCTURE_HPP

#include "Structure.hpp"
#include "ScanDataCondition.hpp"
#include "TypePrinter.hpp"

#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <memory>

using namespace std;
typedef unsigned int struct_len_t;

namespace structure {

	const unsigned int linkCount = 3; 
	// 자기 자신의 부모 앨리먼트 스마트포인터 + 부모 컨테이너에 등록된 앨리먼트 스마트포인터 + 부모 링크리스트에 등록된 앨리먼트 스마트포인터
	const unsigned int unlinkCount = 1; //컨테이너생서이 스마트포인터 하나 생성

    class MultiDataStructure : public Structure {
	private:
		//parent Element로부터 연결할 다음에, 
		//두번째로 생성 -> get_shared_ptr
		shared_ptr<DataElement> parentElement;
	protected:
		struct_len_t length; //데이터 갯수
		
		//데이터 갯수에 변화가 생길 때 이 함수를 실행  
    public:
        explicit MultiDataStructure( const string _data, const StructType _structType ) :
			Structure( _data, DATATYPE_STRING, _structType ) { 

				this->parentElement = make_shared<DataElement>();
				this->length = 0;
		}

		//in C++11 destructors default to noexcept
		//이 시스템 규칙상 데이터를 제거하기 전에 링크를 끊어야 합니다.
		//데이터베이스에서 데이터를 삭제할 때 소멸자를 사용하기 전에
		//isLinkedByParent을 사용해서 연결된 포인터의 갯수를 파악해야 합니다.
        ~MultiDataStructure() {
			
			//링크가 걸려있는 경우,
			//링크해제를 먼저 해야 한다.
			/*
			if( parentElement.use_count() != 1)
				throw DataLinkException("This Data have parentElement.");
			*/
		}

        //상위데이터 추출
		shared_ptr<DataElement> getParentElement(void) noexcept { return parentElement; }

		void setParentElement(shared_ptr<DataElement>& _targetElement) noexcept { 
			this->parentElement = _targetElement->getSharedPtr();
		}

		void unlinkParentElement(void) noexcept {
			this->parentElement = make_shared<DataElement>();
		}

		//데이터길이 확인
		bool isEmpty(void) noexcept {  return this->length == 0; }
		
		struct_len_t getLength(void) noexcept { return this->length; }

		//링크 갯수 확인
		const unsigned int getUseParentPointerNum(void) { return parentElement.use_count(); }
		
		//링크 연결 여부
		//이 클래스에서 shared_ptr 연결, 상위클래스에서의 shared_ptr, 데이터링크의 shared_ptr
		const bool isLinkedByParent(void) { return (parentElement.use_count() >= linkCount); }
    };
}


#endif