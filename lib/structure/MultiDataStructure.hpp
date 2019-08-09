#ifndef MULTIDATASTRUCTURE_HPP
# define MULTIDATASTRUCTURE_HPP

#include "Structure.hpp"
#include "ScanDataCondition.hpp"
#include "TypePrinter.hpp"

#include <string>
#include <list>
#include <vector>
#include <iostream>

using namespace std;

typedef unsigned int struct_len_t;

namespace structure {


	class PrintDataList { //클라이언트로의 데이터 출력용
	public:
		vector<string> data;
		vector<DataType> dataType;
		vector<StructType> structType;
		int size;
		string dataInfo;

		PrintDataList(string _dataInfo, list<DataElement*> _resultList) {
		
			this->dataInfo = _dataInfo;

			if( _resultList.empty() ) {

				this->size = 0;
				return;

			} else {

				//순회
				for(list<DataElement*>::iterator iter = _resultList.begin();
						iter != _resultList.end(); iter++ ) {

					this->data.push_back((*iter)->getDataToString());
					this->dataType.push_back((*iter)->getDataType());
					this->structType.push_back((*iter)->getStructType());
					this->size++;

				}
			}
		}

		//결과데이터를 콘솔에 출력
		void printResultInConsole(void) {

			cout << this->dataInfo << endl;

			if( this->size == 0 )
				cout << "-- empty --" << endl;
			else {

				
				for(int i = 0; i < size; i++ ) {

					cout << i << ". ";
					cout << this->data[i] << '\t';
					cout << convertDataTypeToString(this->dataType[i]) << '\t';
					cout << convertStructTypeToString(this->structType[i]) << '\n';
				}
			}
			
		}

	};

	//범위에 해당되는 데이터 조사
	//데이터를 순회하면서 실행되는 객체함수이므로
	//noexcept로 설정됩니다.
	//즉, ScanDataCondition의 dcf가 다르면 Exception이 아닌
	//false를 반환합니다.
	//ScanDataCondition이 비어있으면 전체 범위로 간주합니다
	
	//1.숫자 범위
	struct isInNumberRange {

		//데이터컨디션 갯수는 ScanDataCondition에서 체크하므로 검사하지않음
		bool operator()(ScanDataCondition& _condition, DataElement& _targetData) noexcept {

			//데이터가 STRING값이면 false 반환

			if( _targetData.getDataType() == DATATYPE_STRING ) return false;

			if( _condition.isEmpty()) return true;
			if( _condition.conditionToken[0]->dcf != DCF_RANGE) return false;

			//결과값
			bool result = false;

			//논리연산 여부
			bool haveLogic = false;
			DataLogicFlag logicFlag;

			for( vector<DataCondition*>::iterator iter = _condition.conditionToken.begin();
					iter != _condition.conditionToken.end(); iter++ ) {
				
				//범위
				if( (*iter)->dcf == DCF_RANGE ) {
					
					//unpack Condition
					DataRangeCondition* targetRange = (DataRangeCondition*)(*iter);

					int rangeValue = targetRange->numberical;
					DataRangeFlag rangeFlag = targetRange->rangeFlag;

					targetRange = nullptr;


					//unpack targetValue in DataElement
					double targetValue = 0.0;

					if( _targetData.getDataType() == DATATYPE_NUMBER )
						targetValue = _targetData.getData<number_t>();
					else
						targetValue = _targetData.getData<float_t>();


					//범위 지정

					//그런데 result가 true 인 상태에서 or연산을 했을 경우 true false와 상관없이 
					//무조건 true이므로 패스

					// 1 | 0 or 1 = 1
					if( result && logicFlag == DataLogicFlag::OR ) continue;
					
					// 0 & 0 or 1 = 0
					if( !result && logicFlag == DataLogicFlag::AND ) continue;
					else {
						
						switch(  rangeFlag ) {

							// 0 | 1 = 1
							// 0 | 0 = 0
							// 1 & 0 = 0
							// 1 & 1 = 1

							case OVER:

								if( targetValue > rangeValue ) {

									
									continue;
								} else {

									result = false;
									continue;

								}
							break;

						}

					}

				//논리연산 (AND, OR)
				} else if( (*iter)->dcf == DCF_LOGIC ) {

					haveLogic = true;
					logicFlag = ((DataLogicCondition*)(*iter))->logicFlag;
					continue;

				}
			}
		}
	};


	//2.문자열 범위
	struct isInStringRange {
		bool operator()(const ScanDataCondition& _condition, const DataElement& _targetData) noexcept {
			return false;
		}

	};


	//다중데이터컨테이너 추상클래스
	class MultiDataStructure : public Structure {
	private:
		DataElement* parentElement;
	protected:
		struct_len_t length; //데이터 갯수
		virtual void updateLength(void) = 0; //데이터갯수 업데이트
		//데이터 갯수에 변화가 생길 때 이 함수를 실행
	public:
		explicit MultiDataStructure( const string _data, const DataType _dataType,
								const StructType _structType ) :
			Structure( _data, _dataType, _structType ) { 

				this->parentElement = nullptr;
				this->length = 0;
			}
		
		~MultiDataStructure() noexcept {
			parentElement = nullptr;
		}

		//상위데이터 추출
		DataElement& getParentElement(void) noexcept { return *parentElement; }
		void setParentElement(DataElement* const _targetElement) noexcept { 
			this->parentElement =  _targetElement;
		}

		//데이터길이 확인
		bool isEmpty(void) noexcept {  return this->length == 0; }
		struct_len_t getLength(void) noexcept { return this->length; }

		//virtual function
		
		virtual list<DataElement*> searchRange(ScanDataCondition& _condition) = 0;
		virtual bool deleteRange(ScanDataCondition& _condition) = 0;
		virtual d_size_t getStructSize(void) = 0;


	};	
}

#endif
