#include "lib/structure/MultiSet.hpp"
#include "lib/structure/StaticList.hpp"
#include "lib/structure/DynamicList.hpp"
#include "lib/structure/StaticHashMap.hpp"

#include <string>
#include <list>
#include <iostream>

using namespace structure;
using namespace std;


int main(void) {

	StaticHashMap A("mytest", DATATYPE_FLOAT);

	A.insertKeyValue("P1", "100.13.33");
	A.insertKeyValue("P2", "200");
	A.insertKeyValue("P3", "300");
	A.insertKeyValue("P4", "400");
	//A.updateValue("P1", "10a10a10a");

	ScanDataCondition all("P*");
	list<DataElement> resultList = A.searchRange(all);

	for(list<DataElement>::iterator iter = resultList.begin();
		iter != resultList.end(); iter++ ) {
			cout << iter->getDataToString() << endl;
		}
	

	return 0;
}
