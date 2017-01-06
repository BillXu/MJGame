#include "MJRoomStateWaitPlayerAct.h"
#include "WZMJRoom.h"
class WZMJRoomStateWaitPlayerAct
	:public MJRoomStateWaitPlayerAct
{
public:
	bool onMsg(Json::Value& prealMsg, uint16_t nMsgType, eMsgPort eSenderPort, uint32_t nSessionID)override
	{
		if (MSG_PLAYER_ACT != nMsgType && MSG_REQ_ACT_LIST != nMsgType)
		{
			return false;
		}

		if (MSG_PLAYER_ACT == nMsgType)
		{
			auto actType = prealMsg["actType"].asUInt();
			auto nCard = prealMsg["card"].asUInt();
			auto pPlayer = getRoom()->getMJPlayerBySessionID(nSessionID);
			auto pRoom = (WZMJRoom*)getRoom();
			if (actType == eMJAct_Chu && pPlayer && m_nIdx == pPlayer->getIdx() )
			{
				if ( pRoom->canPlayerChuCard(m_nIdx,nCard) == false )
				{
					Json::Value jsRet;
					jsRet["ret"] = 2;
					getRoom()->sendMsgToPlayer(jsRet, nMsgType, nSessionID);
					LOGFMTE("wz mj can not must chu feng ke card uid = %u room id  = %u", pPlayer->getUID(), getRoom()->getRoomID() );
					return true;
				}
			}
		}

		return MJRoomStateWaitPlayerAct::onMsg(prealMsg, nMsgType, eSenderPort, nSessionID);
	}
};