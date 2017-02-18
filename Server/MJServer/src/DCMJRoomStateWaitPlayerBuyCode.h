#pragma once 
#include "IMJRoomState.h"
#include "log4z.h"
#include "DCMJRoom.h"
#include "DCMJPlayer.h"
class DCMJRoomstateWaitPlayerBuyCode
	:public IMJRoomState
{
public:
	uint32_t getStateID()final{ return eRoomState_DC_WaitPlayerBuyCode; }
	void enterState(IMJRoom* pmjRoom, Json::Value& jsTranData)override
	{
		IMJRoomState::enterState(pmjRoom, jsTranData);
		setStateDuringTime(eTime_WaitPlayerBuyCode);
		vResponedPlayer.clear();

		Json::Value jsRet;
		getRoom()->sendRoomMsg(jsRet, MSG_DC_ROOM_WAIT_PLAYER_BUY_CODE);
	}

	void onStateTimeUp()override
	{
		getRoom()->goToState(eRoomState_StartGame);
	}

	bool onMsg(Json::Value& prealMsg, uint16_t nMsgType, eMsgPort eSenderPort, uint32_t nSessionID)override
	{
		switch (nMsgType)
		{
		case MSG_DC_PLAYER_BUY_CODE:
		{
			if (prealMsg["code"].isNull())
			{
				Json::Value js;
				js["ret"] = 3;
				getRoom()->sendMsgToPlayer(js, nMsgType, nSessionID);
				break;
			}

			auto player = (DCMJPlayer*)getRoom()->getMJPlayerBySessionID(nSessionID);
			if (player == nullptr )
			{
				LOGFMTE("you are not in room or you are not banker can not do this respone room id = %u, session id = %u", getRoom()->getRoomID(), nSessionID);
				Json::Value js;
				js["ret"] = 2;
				getRoom()->sendMsgToPlayer(js, nMsgType, nSessionID);
				break;
			}

			auto iter = std::find(vResponedPlayer.begin(),vResponedPlayer.end(),player->getIdx() );
			if (iter != vResponedPlayer.end())
			{
				Json::Value js;
				js["ret"] = 1;
				getRoom()->sendMsgToPlayer(js, nMsgType, nSessionID);
				break;
			}
			vResponedPlayer.push_back(player->getIdx());
			
			// tell others the result
			auto nCode = prealMsg["code"].asUInt();
			Json::Value jsRet;
			jsRet["code"] = nCode;
			jsRet["idx"] = player->getIdx();
			getRoom()->sendRoomMsg(jsRet, MSG_DC_ROOM_PLAYER_BUY_CODE);

			// do set buy code 
			player->setBuyCode(nCode);

			if (vResponedPlayer.size() == 4)
			{
				getRoom()->goToState(eRoomState_StartGame);
			}
		}
		break;
		default:
			return false;
		}
		return true;
	}

protected:
	std::vector<uint8_t> vResponedPlayer;
};