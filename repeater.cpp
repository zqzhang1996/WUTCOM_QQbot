#include <CQEVE_GroupMsg.h>
#include <CQAPI_EX.h>

#include <string>
#include <vector>

using namespace std;
using namespace CQ;

struct MessageDict
{
	int fromGroup = 0;
	vector<string> messageDict;
	vector<string> repeatedMessageDict;
};

EVE_GroupMsg_EX(Repeater) {
	if (eve.fromQQ == getLoginQQ() || eve.fromQQ == 1000000) {
		return;
	}

	auto msg = eve.sendMsg();
	static vector<MessageDict> messageDict;

	auto groupIndex = messageDict.begin();
	while (groupIndex != messageDict.end() && (*groupIndex).fromGroup != eve.fromGroup) {
		groupIndex++;
	}
	if (groupIndex == messageDict.end()) {
		MessageDict newDict;
		newDict.fromGroup = eve.fromGroup;
		messageDict.push_back(newDict);
		groupIndex = messageDict.end() - 1;
	}
	(*groupIndex).messageDict.push_back(eve.message);
	if ((*groupIndex).messageDict.size() > 10) {
		(*groupIndex).messageDict.erase((*groupIndex).messageDict.begin());
	}
	int count = 0;
	for (auto info : (*groupIndex).messageDict) {
		if (info == eve.message) {
			count++;
		}
	}
	for (auto info : (*groupIndex).repeatedMessageDict) {
		if (info == eve.message) {
			count = 0;
		}
	}
	if (count > 1) {
		msg << eve.message << send;
		(*groupIndex).repeatedMessageDict.push_back(eve.message);
		if ((*groupIndex).repeatedMessageDict.size() > 3) {
			(*groupIndex).repeatedMessageDict.erase((*groupIndex).repeatedMessageDict.begin());
		}
	}
}