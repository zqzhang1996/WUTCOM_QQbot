#include <CQEVE_GroupMsg.h>
#include <CQAPI_EX.h>

#include <string>

using namespace std;
using namespace CQ;

EVE_GroupMsg_EX(YingKill) {
	auto message = eve.message;
	auto msg = eve.sendMsg();
	if (message.find("��") != string::npos) {
		msg << "һȭһ�����ӹ�" << send;
		setGroupBan(eve.fromGroup, eve.fromQQ, 10);
	}
}