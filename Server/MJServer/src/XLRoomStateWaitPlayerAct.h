#include "MJRoomStateWaitPlayerAct.h"
#include "IMJPlayerCard.h"
class XLRoomStateWaitPlayerAct
	:public MJRoomStateWaitPlayerAct
{
public:
	void enterState(IMJRoom* pmjRoom, Json::Value& jsTranData)override
	{
		IMJRoomState::enterState(pmjRoom, jsTranData);
		setStateDuringTime(pmjRoom->isWaitPlayerActForever() ? 100000000 : eTime_WaitPlayerAct);
		if (jsTranData["idx"].isNull() == false && jsTranData["idx"].isUInt())
		{
			m_nIdx = jsTranData["idx"].asUInt();
			getRoom()->onWaitPlayerAct(m_nIdx, m_isCanPass);

			auto pp = getRoom()->getMJPlayerByIdx(m_nIdx);
			if (!pp)
			{
				LOGFMTE("wait you act but you are nullptr idx = %u , room id = %u",m_nIdx,getRoom()->getRoomID());
			}
			else
			{
				if (pmjRoom->isWaitPlayerActForever() && pp->haveState(eRoomPeer_AlreadyHu) )
				{
					setStateDuringTime(1);
					if ( m_isCanPass) // have opt , must let player chose 
					{
						setStateDuringTime(eTime_WaitPlayerAct); 
					}
				}
			}
			// check tuo guan 
			getRoom()->onCheckTrusteeForWaitPlayerAct(m_nIdx, m_isCanPass);
			return;
		}
		assert(0 && "invalid argument");
	}

	void onStateTimeUp()override
	{
		auto pPlayer = getRoom()->getMJPlayerByIdx(m_nIdx);
		if (pPlayer == nullptr)
		{
			LOGFMTE("why wait cur player is null , idx = %u, roomID = %u",m_nIdx,getRoom()->getRoomID());
			return;
		}

		if (pPlayer->haveState(eRoomPeer_AlreadyHu))
		{
			if (pPlayer->getPlayerCard()->isHoldCardCanHu())
			{
				// do hu ;
				LOGFMTD("wait time out , auto hu , already hu idx = %u roomID = %u",  m_nIdx,getRoom()->getRoomID());
				Json::Value jsTran;
				jsTran["idx"] = m_nIdx;
				jsTran["act"] = eMJAct_Hu;
				jsTran["card"] = pPlayer->getPlayerCard()->getNewestFetchedCard();
				getRoom()->goToState(eRoomState_DoPlayerAct, &jsTran);
				return;
			}
		}

		MJRoomStateWaitPlayerAct::onStateTimeUp();
	}

	bool onMsg(Json::Value& prealMsg, uint16_t nMsgType, eMsgPort eSenderPort, uint32_t nSessionID)override
	{
		if (MSG_PLAYER_ACT != nMsgType && MSG_REQ_ACT_LIST != nMsgType )
		{
			return false;
		}

		auto actType = prealMsg["actType"].asUInt();
		auto nCard = prealMsg["card"].asUInt();
		auto pPlayer = getRoom()->getMJPlayerBySessionID(nSessionID);
		if (actType == eMJAct_Chu && pPlayer && pPlayer->haveState(eRoomPeer_AlreadyHu) && m_nIdx == pPlayer->getIdx() )
		{
			if ( nCard != pPlayer->getPlayerCard()->getNewestFetchedCard())
			{
				Json::Value jsRet;
				jsRet["ret"] = 2;
				getRoom()->sendMsgToPlayer(jsRet, nMsgType, nSessionID);
				LOGFMTE("already hu can not must chu newest fetched card uid = %u ncard = %u", pPlayer->getUID(), pPlayer->getPlayerCard()->getNewestFetchedCard());
				return true;
			}
		}

		return MJRoomStateWaitPlayerAct::onMsg(prealMsg, nMsgType, eSenderPort, nSessionID );
	}
};