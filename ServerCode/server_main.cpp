#include "../lib/loader/AccountLoader.hpp"
#include "../lib/threadAdapter/AdapterThreadUtility.hpp"
#include "../lib/threadAdapter/AdapterThreadBridge.hpp"

#include <string>
#include <list>
#include <iostream>

using namespace std;


int main(void) {

	AccountLoader test;
	test.setUserPswd("customer01", "newPasword");
	test.updateFile();


	return 0;
}