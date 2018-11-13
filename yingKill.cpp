#include <CQEVE_GroupMsg.h>
#include <CQAPI_EX.h>
#include <CQcode.h>
#include <thread>

#include <string>

using namespace std;
using namespace CQ;

void unBan(long long group, long long qq, int time) {
	Sleep(time);
	setGroupBan(group, qq, 0);
}

EVE_GroupMsg_EX(YingKill) {
	if (eve.fromQQ == getLoginQQ() || eve.isSystem()) {
		return;
	}
	auto message = eve.message;
	auto msg = eve.sendMsg();
	if (message.find("嘤") != string::npos) {
		msg << "一拳一个嘤嘤怪" << code::at(eve.fromQQ) << send;
		setGroupBan(eve.fromGroup, eve.fromQQ, 10);//按秒接收参数，转换成分向上取整

		//thread t(unBan, eve.fromGroup, eve.fromQQ, 10 * 1000);//毫秒
		//t.detach();

	}
}