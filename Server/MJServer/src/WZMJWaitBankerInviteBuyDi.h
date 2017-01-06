#pragma once 
#include "IMJRoomState.h"
#include "log4z.h"
#include "WZMJRoom.h"
#include "WZMJPlayer.h"
class WZMJWaitBankerInviteBuyDi
	:public IMJRoomState
{
public:
	enum eStage
	{
		eStage_WaitBankerInvite,
		eState_WaitOtherPlayerDingDi,
	};
public:
	uint32_t getStateID()final{ return eRoomState_WaitBankInviteBuyDi; }
	void enterState(IMJRoom* pmjRoom, Json::Value& jsTranData)override
	{
		IMJRoomState::enterState(pmjRoom,jsTranData);
		auto pRoom = (WZMJRoom*)getRoom();
		setStateDuringTime(eTime_WaitDecideInviteBuyDi);
		m_eStage = eStage_WaitBankerInvite;
		vResponedPlayer.clear();

		Json::Value jsRet;
		jsRet["bankIdx"] = getRoom()->getBankerIdx();
		getRoom()->sendRoomMsg(jsRet, MSG_ROOM_WAIT_BANKER_DECIDE_BUY_DI);
	}

	void onStateTimeUp()override
	{
		if (eStage_WaitBankerInvite == m_eStage)  // give up buy di ;
		{
			// send give up invite buy di msg ;
			Json::Value jsRet;
			jsRet["isInvite"] = 0;
			getRoom()->sendRoomMsg(jsRet, MSG_ROOM_BANKER_DECIDE_BUY_DI_RESULT);
		}

		Json::Value jsValue;
		jsValue["idx"] = getRoom()->getBankerIdx();
		getRoom()->goToState(eRoomState_WaitPlayerAct, &jsValue);
	}

	bool onMsg(Json::Value& prealMsg, uint16_t nMsgType, eMsgPort eSenderPort, uint32_t nSessionID)override
	{
		switch (nMsgType)
		{
		case MSG_BANKER_DECIDE_BUY_DI_RESULT:
		{
			if (prealMsg["isInvite"].isNull())
			{
				LOGFMTE("isInvite argument is not exist session id = %u ",nSessionID );
				Json::Value js;
				js["ret"] = 2;
				getRoom()->sendMsgToPlayer(js, nMsgType, nSessionID);
				break;
			}

			auto player = getRoom()->getMJPlayerBySessionID(nSessionID);
			if (player == nullptr || player->getIdx() != getRoom()->getBankerIdx())
			{
				LOGFMTE("you are not in room or you are not banker can not do this respone room id = %u, session id = %u",getRoom()->getRoomID(),nSessionID);
				Json::Value js;
				js["ret"] = 1;
				getRoom()->sendMsgToPlayer(js,nMsgType,nSessionID);
				break;
			}

			// tell others the decision ;
			auto isInvite = prealMsg["isInvite"].asUInt();
			Json::Value jsRet;
			jsRet["isInvite"] = isInvite;
			getRoom()->sendRoomMsg(jsRet, MSG_ROOM_BANKER_DECIDE_BUY_DI_RESULT);

			// process the decision ;
			if (isInvite == 0 ) // not invite other ding di 
			{
				Json::Value jsValue;
				jsValue["idx"] = getRoom()->getBankerIdx();
				getRoom()->goToState(eRoomState_WaitPlayerAct, &jsValue);
			}
			else
			{
				m_eStage = eState_WaitOtherPlayerDingDi;
				setStateDuringTime(eTime_WaitDecideInviteBuyDi);

				((WZMJPlayer*)player)->doDingDi();
			}

			// act back 
			Json::Value js;
			js["ret"] = 0;
			getRoom()->sendMsgToPlayer(js, nMsgType, nSessionID);
		}
		break;
		case MSG_PLAYER_DING_DI_RESPONE:
		{
			if (prealMsg["isDingDi"].isNull())
			{
				LOGFMTE("isDingDi argument is not exist session id = %u ", nSessionID);
				Json::Value js;
				js["ret"] = 4;
				getRoom()->sendMsgToPlayer(js, nMsgType, nSessionID);
				break;
			}

			auto player = getRoom()->getMJPlayerBySessionID(nSessionID);
			if (player == nullptr || player->getIdx() == getRoom()->getBankerIdx())
			{
				LOGFMTE("you are not in room or you are  banker can not do this respone room id = %u, session id = %u", getRoom()->getRoomID(), nSessionID);
				Json::Value js;
				js["ret"] = 2;
				getRoom()->sendMsgToPlayer(js, nMsgType, nSessionID);
				break;
			}

			auto iter = std::find(vResponedPlayer.begin(),vResponedPlayer.end(),player->getIdx() );
			if (iter != vResponedPlayer.end())
			{
				LOGFMTE("don not respone twice room id = %u, uid = %u", getRoom()->getRoomID(), player->getUID());
				Json::Value js;
				js["ret"] = 1;
				getRoom()->sendMsgToPlayer(js, nMsgType, nSessionID);
				break;
			}

			if (m_eStage != eState_WaitOtherPlayerDingDi)
			{
				LOGFMTE("state error = %u room id = %u, uid = %u", m_eStage,getRoom()->getRoomID(), player->getUID());
				Json::Value js;
				js["ret"] = 3;
				getRoom()->sendMsgToPlayer(js, nMsgType, nSessionID);
				break;
			}

			auto isDingDi = prealMsg["isDingDi"].asUInt();
			if ( 1 == isDingDi)
			{
				((WZMJPlayer*)player)->doDingDi();
			}
			vResponedPlayer.push_back(player->getIdx());

			Json::Value js;
			js["idx"] = player->getIdx();
			js["isDingDi"] = prealMsg["isDingDi"];
			getRoom()->sendRoomMsg(js, MSG_ROOM_DING_DI_RESPONE);
			LOGFMTD(" room id = %u , player uid = %u ding result = %u",getRoom()->getRoomID(),player->getUID(),isDingDi);

			if (vResponedPlayer.size() == 3)
			{
				LOGFMTD(" room id = %u all player respone ding go to banker act state ",getRoom()->getRoomID() );
				Json::Value jsValue;
				jsValue["idx"] = getRoom()->getBankerIdx();
				getRoom()->goToState(eRoomState_WaitPlayerAct, &jsValue);
			}
		}
		break;
		default :
			return false;
		}
		return true;
	}

protected:
	eStage m_eStage;
	std::vector<uint8_t> vResponedPlayer;
};