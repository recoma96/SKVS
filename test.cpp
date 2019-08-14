#include "lib/structure/MultiSet.hpp"
#include "lib/structure/StaticList.hpp"

#include <string>
#include <list>
#include <iostream>

using namespace structure;
using namespace std;


int main(void) {

	StaticList A("test", DATATYPE_NUMBER);

	A.insertValue("1");
	A.insertValue("2");
	A.insertValue("2");
	A.insertValue("3");

	ScanDataCondition ex("");

	A.updateDataByIndex(0, "7");\
	A.deleteIndex("1 1");

	list<DataElement> resultList = 	A.searchIndex("0 2");

	for(list<DataElement>::iterator iter = resultList.begin();
			iter != resultList.end(); iter++ ) {
				cout << iter->getDataToString() << endl;
	}

	return 0;
}
