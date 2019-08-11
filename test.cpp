#include <iostream>
#include "lib/structure/MultiDataStructure.hpp"

using namespace std;

using namespace structure;
int main(void) {

	IsInStringRange isInStringRange;

	DataElement strEle("HiTension", DATATYPE_STRING, STRUCTTYPE_ELEMENT);
	DataElement testNum("100", DATATYPE_NUMBER, STRUCTTYPE_ELEMENT);

	ScanDataCondition condition("*Tenas*");

	if( isInStringRange(condition, strEle) )
		cout << "yes!" << endl;
	else
		cout << "no!" << endl;

	return 0;
}
