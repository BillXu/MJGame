#pragma once 
#include "MJRoomStateWaitReady.h"
#include "DCMJRoom.h"
class DCMJRoomStateWaitReady
	:public IMJRoomState
{
public:
	uint32_t getStateID()final{ return eRoomSate_WaitReady; }

	void update(float fDeta)override
	{
		IMJRoomState::update(fDeta);
		if (getRoom()->canStartGame())
		{
			if (((DCMJRoom*)getRoom())->isEnableBuyCode())
			{

				getRoom()->goToState(eRoomState_DC_WaitPlayerBuyCode);
			}
			else
			{
				getRoom()->goToState(eRoomState_StartGame);
			}
		}
	}

	bool onMsg(Json::Value& prealMsg, uint16_t nMsgType, eMsgPort eSenderPort, uint32_t nSessionID)override
	{
		if (MSG_PLAYER_SET_READY == nMsgType)
		{
			auto pPlayer = getRoom()->getMJPlayerBySessionID(nSessionID);
			if (pPlayer == nullptr || (pPlayer->haveState(eRoomPeer_WaitNextGame) == false))
			{
				LOGFMTE("you are not in this room how to set ready ? session id = %u", nSessionID);
				return true;
			}
			getRoom()->onPlayerSetReady(pPlayer->getIdx());
			if (getRoom()->canStartGame())
			{
				if (((DCMJRoom*)getRoom())->isEnableBuyCode())
				{
					
					getRoom()->goToState(eRoomState_DC_WaitPlayerBuyCode);
				}
				else
				{
					getRoom()->goToState(eRoomState_StartGame);
				}
			}
			return true;
		}
		return false;
	}
};