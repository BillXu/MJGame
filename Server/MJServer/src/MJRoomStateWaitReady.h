#pragma once 
#include "IMJRoomState.h"
#include "IMJRoom.h"
#include "LogManager.h"
#include "IMJPlayer.h"
class CMJRoomStateWaitReady
	:public IMJRoomState
{
public:
	uint32_t getStateID()final{ return eRoomSate_WaitReady; }
	bool onMsg(Json::Value& prealMsg, uint16_t nMsgType, eMsgPort eSenderPort, uint32_t nSessionID)override
	{
		if ( MSG_PLAYER_SET_READY == nMsgType)
		{
			auto pPlayer = getRoom()->getMJPlayerBySessionID(nSessionID);
			if (pPlayer == nullptr)
			{
				CLogMgr::SharedLogMgr()->ErrorLog("you are not in this room how to set ready ? session id = %u", nSessionID );
				return true;
			}
			pPlayer->setState(eRoomPeer_Ready);
			if (getRoom()->canStartGame())
			{
				getRoom()->goToState(eRoomState_StartGame);
			}
			return true;
		}
		return false;
	}
};