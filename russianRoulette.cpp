#include <CQEVE_GroupMsg.h>
#include <CQAPI_EX.h>
#include <CQcode.h>
#include <thread>

#include <string>
#include <vector>

#include <random>

//�����������
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
			sendGroupMsg((*game).groupId, "�����˲μ���ս������˹���̹ر�");
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
				ms << "���п۶��˰����" << code::at((*game).player[(*game).round % 2]) << "������" << send;
				setGroupBan((*game).groupId, (*game).player[(*game).round % 2], 180);
				(*game).isPlay = false;
			}
			else {
				ms << "���п۶��˰����ʲô��û�з���" << send;
				(*game).round++;
				ms << "��" << (*game).round + 1 << "�֣���" << code::at((*game).player[(*game).round % 2]) << "��ǹ" << send;
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

	if (!eve.message.compare("����˹����")) {
		if ((*game).isPlay) {
			eve.sendMsg("��Ϸ���ڽ���");
		}
		else {
			msg << code::at(eve.fromQQ) << "��ʼ��һ������˹���̣����ڵȴ���ս��" << send;
			(*game).isPlay = true;
			(*game).gameId = eve.msgId;

			(*game).player[0] = eve.fromQQ;
			(*game).player[1] = -1;

			(*game).round = -1;
			eve.sendMsg("����[����]�μ���ս");

			thread t(WaitChellanger, &(*game), eve.msgId);
			t.detach();
		}
	}

	if (!eve.message.compare("����")) {
		if ((*game).isPlay) {
			if ((*game).player[1] == -1 && eve.fromQQ != (*game).player[0]) {
				msg << "��ս��" << code::at(eve.fromQQ) << "�ϳ����ӵ����ţ�����[��ǹ]�۶����" << send;
				(*game).player[1] = eve.fromQQ;

				(*game).bullet = RandNum() % 6;

				//eve.sendMsg("����[��ǹ]�۶����");

				(*game).round = 0;
				msg << "��" << (*game).round + 1 << "�֣���" << code::at((*game).player[(*game).round % 2]) << "��ǹ" << send;
				thread t(StartNewRound, &(*game), (*game).gameId, (*game).round);
				t.detach();
			}
		}
	}

	if (!eve.message.compare("��ǹ")) {
		if ((*game).isPlay) {
			if (eve.fromQQ == (*game).player[(*game).round % 2]) {
				if ((*game).round == (*game).bullet) {
					msg << code::at((*game).player[(*game).round % 2]) << "�۶������ǹ����" << send;
					setGroupBan((*game).groupId, (*game).player[(*game).round % 2], 180);
					(*game).isPlay = false;
				}
				else {
					msg << code::at((*game).player[(*game).round % 2]) << "�۶������ʲô��û�з���" << send;
					(*game).round++;
					msg << "��" << (*game).round + 1 << "�֣���" << code::at((*game).player[(*game).round % 2]) << "��ǹ" << send;
					thread t(StartNewRound, &(*game), (*game).gameId, (*game).round);
					t.detach();
				}
			}
		}
	}
}