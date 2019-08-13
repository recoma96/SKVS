/***
 *  @file : DataStructureLayer.hpp
 * 	@date 2019/08/12
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 두개 이상의 데이터를 담는 데이터구조의 최상위 클래스의 레이어 입니다.
 * 
 ***/

#ifndef MULTIDATASTRUCTURELAYER_HPP
# define MULTIDATASTRUCTURELAYER_HPP

#include "MultiDataStructure.hpp"
#include "ScanDataCondition.hpp"
#include "TypePrinter.hpp"

#include <string>
#include <list>
#include <vector>
#include <iostream>

using namespace std;


namespace structure {


	class PrintDataList { //클라이언트로의 데이터 출력용
	public:
		vector<string> data;
		vector<DataType> dataType;
		vector<StructType> structType;
		int size;
		string dataInfo;

		PrintDataList(string _dataInfo, list<DataElement> _resultList) {
		
			this->dataInfo = _dataInfo;

			if( _resultList.empty() ) {

				this->size = 0;
				return;

			} else {

				//순회
				for(list<DataElement>::iterator iter = _resultList.begin();
						iter != _resultList.end(); iter++ ) {

					this->data.push_back(iter->getDataToString());
					this->dataType.push_back(iter->getDataType());
					this->structType.push_back(iter->getStructType());
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
	struct IsInNumberRange {

		//데이터컨디션 갯수는 ScanDataCondition에서 체크하므로 검사하지않음
		inline const bool operator()(ScanDataCondition& _condition, DataElement& _targetData) noexcept {

			//데이터가 STRING값이면 false 반환

			if( _targetData.getDataType() == DATATYPE_STRING ) return false;

			if( _condition.isEmpty()) return true;
			if( _condition.conditionToken[0]->dcf != DCF_RANGE) return false;

			//결과값
			bool result = false;
			bool nowResult = false; //result버퍼 -> 해당 문장에 대한 결과를 입력

			
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
					if( result && (logicFlag == DataLogicFlag::OR) ) continue;
					
					// 0 & 0 or 1 = 0
					if( !result && (logicFlag == DataLogicFlag::AND) ) continue;
					else {
						
						switch(  rangeFlag ) {

							// 0 | 1 = 1
							// 0 | 0 = 0
							// 1 & 0 = 0
							// 1 & 1 = 1

							case OVER:
								 nowResult = ( targetValue > rangeValue );
							break;

							case EOVER:
								nowResult = ( targetValue >= rangeValue);
							break;
							
							case UNDER:
								nowResult = ( targetValue < rangeValue );
							break;

							case EUNDER:
								nowResult = ( targetValue <= rangeValue );
							break;

							case EQUAL:
								nowResult = ( targetValue == rangeValue );
							break;

							case NONEQUAL:
								nowResult = ( targetValue != rangeValue );	
							break;
									
						}

						//데이터 로직 연산
						if( haveLogic ) {
							if( logicFlag == DataLogicFlag::OR )
								result |= nowResult;
							else {
								result &= nowResult;
							}
						} else {
							result = nowResult;
						}

						continue;

					}

				//논리연산 (AND, OR)
				} else if( (*iter)->dcf == DCF_LOGIC ) {

					haveLogic = true;
					logicFlag = ((DataLogicCondition*)(*iter))->logicFlag;
					continue;

				}
			}

			return result;
		}
		
	};


	//2.문자열 범위
	//문자열 범위는 Condition 하나만 취급 (vector원소갯수가 1개)
	struct IsInStringRange {
		inline const bool operator()(ScanDataCondition& _condition, const DataElement& _targetData) noexcept {
			
			if( _condition.isEmpty() ) return true;
			if( _condition.conditionToken.size() != 1 ) return false;
			if( _condition.conditionToken[0]->dcf != DCF_STRING ) return false;

			// *aaa -> ... aaa
			// aaa* -> aaa ...
			// *aaa* -> ... aaa ...

			//setString 판별에 기준이 되는 문자열
			//targetString 판별 대상 문자열
			string setString = ((DataStringCondition*)(_condition.conditionToken[0]))->str;
			string targetString = _targetData.getDataToString();


			//별 위치 여부
			const bool starAtFirst = (*(setString.begin()) == '*');
			const bool starAtEnd = (*(setString.end()-1) == '*');

			cout << "first" << starAtFirst << endl;
			cout << "end" << starAtEnd << endl;

			if( !starAtFirst && !starAtEnd )
				return ( targetString.compare(setString) == 0 );
			else {

				string findString;

				//문자열에서 별 떼이터내기
				if( starAtFirst && starAtEnd ) // *aa*
					findString = setString.substr(1, setString.length() - 2);
				else if( starAtFirst && !starAtEnd ) //*aa
					findString = setString.substr(1);
				else if( !starAtFirst && starAtEnd ) // aa*
					findString = setString.substr(0, setString.length() - 1);

				if( findString.length() > targetString.length() ) return false;	

				int foundLoc = targetString.find(findString);

				if( foundLoc == -1 ) return false; //못찾음
				else {

					if( starAtFirst && starAtEnd ) return true;
					else if( !starAtFirst && starAtEnd ) { //찾은 위치가 반드시 맨 앞을 가려켜야 한다
						if( foundLoc == 0 ) return true;
						else return false;
					} else { //찾은 위치가 맨 끝으로부터 찾는 문자열의 위치를 뺀 값
						if( (foundLoc + (findString.length() - 1) ) == targetString.length() - 1 )
							return true;
						else return false;
					}

				}

			}
			
			return false;
		}
	};


	//다중데이터컨테이너 추상클래스
	class MultiDataStructureLayer : public MultiDataStructure {
	protected:
		virtual void updateLength(void) = 0; //데이터갯수 업데이트
	public:
		explicit MultiDataStructureLayer( const string _data, const DataType _dataType,
								const StructType _structType ) :
			MultiDataStructure( _data, _dataType, _structType ) { }
			
	};	
}

#endif
