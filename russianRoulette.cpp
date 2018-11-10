#include <CQEVE_GroupMsg.h>
#include <CQAPI_EX.h>
#include <CQcode.h>
#include <thread>

#include <string>
#include <vector>

#include <random>

//随机数生成器
int RandNum(int num_min = 0, int num_max = 1024, int seed = time(nullptr)) {
	static std::default_random_engine e(seed);
	std::uniform_int_distribution<> u(num_min, num_max);
	return u(e);
}

using namespace std;
using namespace CQ;

struct Roulette {
	long long groupId = 0;

	bool isPlay = false;
	long long gameId = 0;
	long long player[2];

	int round = -1;
	int bullet = 0;
};

void WaitChellanger(Roulette* game, long long gameId) {
	Sleep(60 * 1000);
	if ((*game).isPlay && gameId == (*game).gameId) {
		if((*game).player[1] == -1){
			sendGroupMsg((*game).groupId, "因无人参加挑战，俄罗斯轮盘关闭");
			(*game).isPlay = false;
		}
	}
}

void StartNewRound(Roulette* game, long long gameId ,int round) {
	Sleep(20 * 1000);
	if ((*game).isPlay && gameId == (*game).gameId) {
		if (round == (*game).round) {
			auto ms = MsgSend((*game).groupId, 1);
			if ((*game).round == (*game).bullet) {
				ms << "裁判扣动了扳机，" << code::at((*game).player[(*game).round % 2]) << "倒下了" << send;
				setGroupBan((*game).groupId, (*game).player[(*game).round % 2], 180);
				(*game).isPlay = false;
			}
			else {
				ms << "裁判扣动了扳机，什么都没有发生" << send;
				(*game).round++;
				ms << "第" << (*game).round + 1 << "轮，由" << code::at((*game).player[(*game).round % 2]) << "持枪" << send;
				thread t(StartNewRound, game, (*game).gameId, (*game).round);
				t.detach();
			}
		}
	}
}

EVE_GroupMsg_EX(RussianRoulette) {

	//return;

	if (eve.fromQQ == getLoginQQ() || eve.fromQQ == 1000000) {
		return;
	}
	static vector<Roulette> games;
	auto game = games.end();
	bool isNewGame = true;

	for (auto i = games.begin(); i != games.end(); i++) {
		if ((*i).groupId == eve.fromGroup) {
			isNewGame = false;
			game = i;
			break;
		}
	}
	if (isNewGame) {
		Roulette newGame;
		newGame.groupId = eve.fromGroup;

		games.push_back(newGame);
		game = games.end() - 1;
	}

	auto msg = eve.sendMsg();

	if (!eve.message.compare("俄罗斯轮盘")) {
		if ((*game).isPlay) {
			eve.sendMsg("游戏正在进行");
		}
		else {
			msg << code::at(eve.fromQQ) << "开始了一场俄罗斯轮盘，正在等待挑战者" << send;
			(*game).isPlay = true;
			(*game).gameId = eve.msgId;

			(*game).player[0] = eve.fromQQ;
			(*game).player[1] = -1;

			(*game).round = -1;
			eve.sendMsg("发送[加入]参加挑战");

			thread t(WaitChellanger, &(*game), eve.msgId);
			t.detach();
		}
	}

	if (!eve.message.compare("加入")) {
		if ((*game).isPlay) {
			if ((*game).player[1] == -1 && eve.fromQQ != (*game).player[0]) {
				msg << "挑战者" << code::at(eve.fromQQ) << "上场，子弹上膛，发送[开枪]扣动扳机" << send;
				(*game).player[1] = eve.fromQQ;

				(*game).bullet = RandNum() % 6;

				//eve.sendMsg("发送[开枪]扣动扳机");

				(*game).round = 0;
				msg << "第" << (*game).round + 1 << "轮，由" << code::at((*game).player[(*game).round % 2]) << "持枪" << send;
				thread t(StartNewRound, &(*game), (*game).gameId, (*game).round);
				t.detach();
			}
		}
	}

	if (!eve.message.compare("开枪")) {
		if ((*game).isPlay) {
			if (eve.fromQQ == (*game).player[(*game).round % 2]) {
				if ((*game).round == (*game).bullet) {
					msg << code::at((*game).player[(*game).round % 2]) << "扣动扳机，枪响了" << send;
					setGroupBan((*game).groupId, (*game).player[(*game).round % 2], 180);
					(*game).isPlay = false;
				}
				else {
					msg << code::at((*game).player[(*game).round % 2]) << "扣动扳机，什么都没有发生" << send;
					(*game).round++;
					msg << "第" << (*game).round + 1 << "轮，由" << code::at((*game).player[(*game).round % 2]) << "持枪" << send;
					thread t(StartNewRound, &(*game), (*game).gameId, (*game).round);
					t.detach();
				}
			}
		}
	}
}