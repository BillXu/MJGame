#pragma once 
#include "MJRoomStateAskForPengOrHu.h"
class XLRoomStateAskForPengOrHu
	:public MJRoomStateAskForPengOrHu
{
public:
	void enterState(IMJRoom* pmjRoom, Json::Value& jsTranData)override
	{
		MJRoomStateAskForPengOrHu::enterState(pmjRoom, jsTranData);
		// check if have already hu player ?
		std::vector<uint8_t> vAlreadyHuWaitHu;
		for (auto& nCheckIdx : m_vWaitHuIdx)
		{
			auto pPlayer = getRoom()->getMJPlayerByIdx(nCheckIdx);
			if ( pPlayer && pPlayer->haveState(eRoomPeer_AlreadyHu) )
			{
				vAlreadyHuWaitHu.push_back(nCheckIdx);
			}
		}

		if (vAlreadyHuWaitHu.empty() == false)
		{
			m_tAlreadyHuActTimer.reset();
			m_tAlreadyHuActTimer.setInterval(1);
			m_tAlreadyHuActTimer.setIsAutoRepeat(false);
			m_tAlreadyHuActTimer.setCallBack([vAlreadyHuWaitHu,this](CTimer* pt , float f )
			{
				for (auto& refIdx : vAlreadyHuWaitHu)
				{
					auto pPlayer = getRoom()->getMJPlayerByIdx(refIdx);
					if (pPlayer == nullptr)
					{
						LOGFMTE("why you are nullptr ? idx = %",refIdx );
						continue;
					}

					LOGFMTD("auto hu dian pao , already hu player room id = %u uid = %u ",getRoom()->getRoomID(),pPlayer->getUID());
					Json::Value jsMsg;
					jsMsg["actType"] = eMJAct_Hu;
					onMsg(jsMsg, MSG_PLAYER_ACT,ID_MSG_PORT_CLIENT,pPlayer->getSessionID());
				}
			}
			);
			m_tAlreadyHuActTimer.start();
			return;
		}

		if ( m_vWaitPengGangIdx.empty() == false )
		{
			auto p = getRoom()->getMJPlayerByIdx(m_vWaitPengGangIdx.front());
			if (p == nullptr || p->haveState(eRoomPeer_AlreadyHu) == false)
			{
				return;
			}

			// already hu , then already do act 
			auto nIdx = p->getIdx();
			m_tAlreadyHuActTimer.reset();
			m_tAlreadyHuActTimer.setInterval(1);
			m_tAlreadyHuActTimer.setIsAutoRepeat(false);
			m_tAlreadyHuActTimer.setCallBack([nIdx, this](CTimer* pt, float f)
			{
				auto pPlayer = getRoom()->getMJPlayerByIdx(nIdx);
				if (pPlayer == nullptr)
				{
					LOGFMTE("why you are nullptr ? idx = %", nIdx);
				}

				LOGFMTD("auto ming gang , already hu player room id = %u uid = %u ", getRoom()->getRoomID(), pPlayer->getUID());
				Json::Value jsMsg;
				jsMsg["actType"] = eMJAct_MingGang;
				onMsg(jsMsg, MSG_PLAYER_ACT, ID_MSG_PORT_CLIENT, pPlayer->getSessionID());
			}
			);
			m_tAlreadyHuActTimer.start();
		}
	}
	void leaveState()override 
	{
		MJRoomStateAskForPengOrHu::leaveState();
		m_tAlreadyHuActTimer.reset();
	}
protected:
	CTimer m_tAlreadyHuActTimer;
};