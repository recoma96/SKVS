/***
 *  @file : ScanDataCondition.hpp
 * 	@version : 0.0.1
 * 	@date 2019/08/11
 *  @author 하정현(seokbong60@gmail.com) Team:SweetCase Project (1-person Team)
 *  @brief : 탐색 범위를 작성하는 클래스 입니다.
 *  @license : MIT-License
 * 
 ***/


#ifndef SCANDATACONDITION_H
# define SCANDATACONDITION_H

#include <vector>
#include <string>

using namespace std;


namespace structure {
	
	enum DataConditionFlag {

		//DCF_RAGNE (범위 EX) 1 < 3 ))
		//DCF_STRING 문자열 검사
	
		DCF_RANGE = 1,
		DCF_STRING,
		DCF_LOGIC

	};

	enum DataRangeFlag {

		OVER = 3, // <
		EOVER,	// <=
		UNDER,	// >
		EUNDER, // >=
		EQUAL,	// ==
		NONEQUAL // !=

	};

	enum DataLogicFlag {

		AND = 10,	
		OR
	};

	const string printedOver = "over";
	const string printedEover = "eover";
	const string printedUnder = "under";
	const string printedEunder = "eunder";
	const string printedEqual = "equal";
	const string printedNonEqual = "nonequal";
	const string printedAnd = "and";
	const string printedOr = "or";

	//Converter
	//throw DataConvertException
	
	const DataRangeFlag convertStringToRangeCondition(const string str);
	const DataLogicFlag convertStringToLogicCondition(const string str);

	const string convertRangeConditionToString(const DataRangeFlag condition);
	const string convertLogicConditionToString(const DataLogicFlag condition);

	
	class DataCondition {
	public:
		DataConditionFlag dcf;
	
		DataCondition(const DataConditionFlag _dcf) noexcept: dcf(_dcf) { } 
	};

	// ex) under 1, eover 3...
	class DataRangeCondition : public DataCondition {
	public:
		DataRangeFlag rangeFlag;
		double numberical;

		DataRangeCondition(
				const DataRangeFlag _rangeFlag, const double _numberical) noexcept;

	};

	// ex) string*
	class DataStringCondition : public DataCondition {
	public:
		string str;

		DataStringCondition(const string _str) noexcept;

	};

	// ex) A and B or C.....
	class DataLogicCondition : public DataCondition {
	public:
		DataLogicFlag logicFlag;
		DataLogicCondition(const DataLogicFlag _logicFlag) noexcept;

	};


	class ScanDataCondition {
	public:
	
		vector<DataCondition*> conditionToken;

		ScanDataCondition(string _conditionStr);
		~ScanDataCondition();
		bool isEmpty(void) noexcept;

	private:
		void initCondition(void) noexcept;

	};
	

}
#endif
