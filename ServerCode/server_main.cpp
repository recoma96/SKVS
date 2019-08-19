#include "../lib/packet/Packet.hpp"
#include "../lib/logbase/LogBase.hpp"
#include "../lib/loader/SystemLoader.hpp"
#include "../lib/database/DataBaseCmd.hpp"
#include <string>
#include <list>
#include <iostream>

using namespace std;


int main(void) {

	SystemLoader sysLoader;
	shared_ptr<ThreadAdapter::AdapterThreadUtility> test = make_shared<ThreadAdapter::AdapterThreadUtility>();
	LogBase logBase(sysLoader.getLogRoot(), test);
	
	
	LogPacket Packet01("user", "127.0.0.1", 1, 2, "This is 1st test");
	
	LogPacket Packet02("user", "127.0.0.1", 1, 2, "This is 2nd test");

	if(!logBase.writeLogToFile(&Packet01)) cout << "false" << endl;
	if(!logBase.writeLogToFile(&Packet02)) cout << "false2" << endl;
	

	return 0;
}