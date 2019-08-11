#include "ScanDataCondition.hpp"
#include "../Exception.hpp"
#include "../Tokenizer.hpp"
#include "DataElement.hpp"

#include <vector>
#include <string>
#include <typeinfo>

using namespace std;
using namespace structure;

const DataRangeFlag structure::convertStringToRangeCondition(const string str){

	//1. return DataRangeFlag
	if( str.compare(printedOver) == 0 ) return OVER;
	else if(str.compare(printedEover) == 0 ) return EOVER;
	else if(str.compare(printedUnder) == 0 ) return UNDER;
	else if(str.compare(printedEunder) == 0 ) return EUNDER;
	else if(str.compare(printedEqual) == 0 ) return EQUAL;
	else if(str.compare(printedNonEqual) == 0 ) return NONEQUAL;
	else
		throw DataConvertException("this string is not be matched with range condition.");
}
	
const DataLogicFlag structure::convertStringToLogicCondition(const string str) {

	//2. return DataLogicFlag
	if(str.compare(printedAnd) == 0 ) return AND;
	else if(str.compare(printedOr) == 0 ) return OR;

	else
		throw DataConvertException("string is not be matched with logic condition.");
		
}


const string structure::convertRangeConditionToString(const DataRangeFlag condition){

	//1. return DataRangeFlag
	if( condition == OVER ) return printedOver;
	else if(condition == EOVER) return printedEover;
	else if(condition == UNDER) return printedUnder;
	else if(condition == EUNDER) return printedEunder;
	else if(condition == EQUAL) return printedEqual; 
	else if(condition == NONEQUAL) return printedNonEqual;

	else
		throw DataConvertException("this type is not mathced range and logic.");
}

const string structure::convertLogicConditionToString(const DataLogicFlag condition) {

	//2. return DataLogicFlag
	if(condition == AND) return printedAnd;
	else if(condition == OR) return printedOr;

	//.throw DataCovertException
	else
		throw DataConvertException("this type is not mathced range and logic.");

}

//Condition 생성자
DataRangeCondition::DataRangeCondition(const DataRangeFlag _rangeFlag, const double _numberical) noexcept : 
		DataCondition(DCF_RANGE) {

		this->rangeFlag = _rangeFlag; this->numberical = _numberical;
}
DataStringCondition::DataStringCondition(const string _str) noexcept : DataCondition(DCF_STRING) {
	this->str = _str;
}
DataLogicCondition::DataLogicCondition(const DataLogicFlag _logicFlag) noexcept :
		DataCondition(DCF_LOGIC) {
	this->logicFlag = _logicFlag;
}

//vector에 있는 데이터 초기화
//public 함수에 있는 예외처리에 사용
void ScanDataCondition::initCondition(void) noexcept {
	
	if( !this->conditionToken.empty() ) {

		for( vector<DataCondition*>::iterator iter = this->conditionToken.end()-1;
				iter != this->conditionToken.begin(); ) {

			if( (*iter)->dcf == DCF_STRING )
				
				
			DataCondition* deleteCondition = (*iter);
			iter--;
			this->conditionToken.erase(iter);
		}

	}

}

//ScanDataCondition
// ex) over 10 and eunder 20
// Range나 String 사이에는 반드시 Logic연산자가 존재해야 하며
// Range와  String이 하나의 Condition에 공존할 수 없습니다.
ScanDataCondition::ScanDataCondition(string _conditionStr) {

	//문자열 토큰화
	vector<string> resultVector	 = tok::tokenizer(_conditionStr);

	//해당 Condition이 range나 string Condition이라는 것을 나타내는 boolean
	//두 개가 true가 되면 DataConditionException을 throw합니다.
	bool rangeCheckFlag = false;
	bool stringCheckFlag = false;

	//토큰화된 데이터 없음
	if( resultVector.empty() || _conditionStr.compare("")) return;

	//토큰회된 데이터 검토
	for( vector<string>::iterator iter = resultVector.begin();
				iter != resultVector.end(); iter++ ) {

		//해당 문자열이 RangeFlag에 해당되는 지 검토

		try {
			
			CheckDataType checkDataType;

			DataRangeFlag rangeFlag = convertStringToRangeCondition((*iter));

			rangeCheckFlag = true;

			//앞에 데이터가 있는데 그 데이터가 Logic이 아니면 DataException 송출
			if( !this->conditionToken.empty() && 
					this->conditionToken[this->conditionToken.size()-1]->dcf != DCF_LOGIC ) {

				initCondition();
				throw DataException("there must be logic before range.");

			}

			iter++; //포인터를 한 단계 앞으로 가서 숫자가 있는 지 검토
			//실수 범위로 계산

			//실수가 아니면 throw 발생
			if(!checkDataType( (*iter), DATATYPE_FLOAT) ) {

				//포인터 정리
				initCondition();				
				throw DataException("this string must be number or float for set range condition.");
			}
			
			//DataCondition  생성
			DataRangeCondition* insertCondition = new DataRangeCondition( rangeFlag, atof((*iter).c_str()));
			this->conditionToken.push_back(insertCondition);
			

		} catch (DataConvertException e) {

			//해당 문자열이 LogicFlag에 해당되는 지 검토
			try {

				DataLogicFlag logicFlag = structure::convertStringToLogicCondition((*iter));

				//앞부분이 존재하지 않거나, 똑같은 Logic일 경우 예외처리
				//DataConverException은 string 전용이므로 DataException으로 처리
				if( this->conditionToken.empty() || 
						this->conditionToken[this->conditionToken.size()-1]->dcf == DCF_LOGIC ) {

					initCondition();
					throw DataException("2 logic is must not be exist each other or 1 logic must not be alone.");
				}

				//데이터 추가
				DataLogicCondition* insertCondition = new DataLogicCondition(logicFlag);
				this->conditionToken.push_back(insertCondition);

			} catch(DataConvertException e) {

				//그냥 string설정
				//conditionToken에 1개 이상의 토큰이 있는 상태에서
				//바로 직전의 데이터가 Logic이 아니면 DataException 송출
				if( !this->conditionToken.empty() && 
						this->conditionToken[this->conditionToken.size()-1]->dcf != DCF_LOGIC ) {

					initCondition();
					throw DataException("string condition setting error.");
				}


				//데이터추가
				stringCheckFlag = true;
				DataStringCondition* insertData = new DataStringCondition((*iter));
				this->conditionToken.push_back(insertData);

			}
		}


		//range와 sring이 같이 있으면 예외처리
		if( rangeCheckFlag && stringCheckFlag ) {
			initCondition();				
			throw DataException("rangeFlag and stringFlag must not be exist each other.");
		}

	}

	DataConditionFlag finalFlag = this->conditionToken[this->conditionToken.size()-1]->dcf;

	//끝부분이 Logic일 경우 예외 처리 
	if( finalFlag == DCF_LOGIC) {
		throw DataException("logic must not be exist last location.");
	}

}
ScanDataCondition::~ScanDataCondition() {

	initCondition();
}
bool ScanDataCondition::isEmpty(void) noexcept {

	return this->conditionToken.empty();
}
