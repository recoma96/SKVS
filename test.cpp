
#include "lib/packet/Packet.hpp"
#include "lib/packet/SerialController.hpp"

#include <string>
#include <list>
#include <iostream>

using namespace structure;
using namespace std;


int main(void) {

	//PakcetTest
	
	DataElement myData("aaa", DATATYPE_STRING, STRUCTTYPE_ELEMENT);

	SendCmdPacket* testPacket = new SendCmdPacket("aaa", "bbb", 1,1,"sssss sssss");
	RecvDataPacket* testRecvDataPacket = new RecvDataPacket("aaa", "bbb", 1, 1, myData);
	RecvMsgPacket* testMsgPacket = new RecvMsgPacket("aaa", "bbb", 1, 1, "msg");
	SignalPacket* testSignal = new SignalPacket("aaa", "bbb", 1, 1, SIGNALTYPE_SHUTDOWN);
	LogPacket* testLog = new LogPacket("aaa", "bbb", 1, 1, "shit");

	char* test = nullptr;

	//test Packet
	test = makePacketToCharArray<LogPacket>(*testLog);
	delete testLog;
	testLog = returnToPacket<LogPacket>(test);


	return 0;
}
