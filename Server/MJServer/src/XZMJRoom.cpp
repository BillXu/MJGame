#include "XZMJRoom.h"
#include "IMJPlayer.h"
#include "IMJPlayerCard.h"
#include "log4z.h"
bool XZMJRoom::isGameOver()
{
	if (XLMJRoom::isGameOver())
	{
		return true;
	}

	uint8_t nNotCanAct = 0;
	for (auto& ref : m_vMJPlayers)
	{
		if (ref && (false == ref->haveState(eRoomPeer_DecideLose)) && (false == ref->haveState(eRoomPeer_AlreadyHu)))
		{
			++nNotCanAct;
		}

		if (nNotCanAct >= 2)
		{
			return false;
		}
	}
	return true;
}

void XZMJRoom::onPlayerAnGang(uint8_t nIdx, uint8_t nCard)
{
	XLMJRoom::onPlayerAnGang(nIdx, nCard);
	// do settle 
	auto nSettleCoin = getBaseBet() * 2;
	auto pGanger = getMJPlayerByIdx(nIdx);
	auto pSettle = new stSettleAnGang(nIdx);
	for (auto& pPlayer : m_vMJPlayers)
	{
		if (nullptr == pPlayer || pPlayer->getIdx() == nIdx || pPlayer->haveState(eRoomPeer_AlreadyHu) || pPlayer->haveState(eRoomPeer_DecideLose))
		{
			continue;
		}

		auto nSeCoin = nSettleCoin;
		if ((int32_t)nSeCoin > pPlayer->getCoin())
		{
			nSeCoin = pPlayer->getCoin();
		}
		pPlayer->addOffsetCoin(-1 * (int32_t)nSeCoin);
		pGanger->addOffsetCoin(nSeCoin);
		pSettle->addLosePlayer(pPlayer->getIdx(), nSeCoin);
	}
	addSettle(pSettle);
	LOGFMTD("room id = %u , idx = %u win coin = %u  final = %u anGang", getRoomID(), nIdx, pSettle->getWinCoin(), pGanger->getCoin());
}

void XZMJRoom::onPlayerBuGang(uint8_t nIdx, uint8_t nCard)
{
	XLMJRoom::onPlayerBuGang(nIdx, nCard);
	// do settle 
	auto nSettleCoin = getBaseBet();
	auto pGanger = getMJPlayerByIdx(nIdx);
	auto pSettle = new stSettleBuGang(nIdx);
	for (auto& pPlayer : m_vMJPlayers)
	{
		if (nullptr == pPlayer || pPlayer->getIdx() == nIdx || pPlayer->haveState(eRoomPeer_AlreadyHu) || pPlayer->haveState(eRoomPeer_DecideLose))
		{
			continue;
		}

		auto nSeCoin = nSettleCoin;
		if ((int32_t)nSeCoin > pPlayer->getCoin())
		{
			nSeCoin = pPlayer->getCoin();
		}
		pPlayer->addOffsetCoin(-1 * (int32_t)nSeCoin);
		pGanger->addOffsetCoin(nSeCoin);
		pSettle->addLosePlayer(pPlayer->getIdx(), nSeCoin);
	}
	addSettle(pSettle);
	LOGFMTD("room id = %u , idx = %u win coin = %u  final = %u BuGang", getRoomID(), nIdx, pSettle->getWinCoin(), pGanger->getCoin());
}

bool XZMJRoom::isAnyPlayerPengOrHuThisCard(uint8_t nInvokeIdx, uint8_t nCard)
{
	for (auto& ref : m_vMJPlayers)
	{
		if (ref == nullptr || nInvokeIdx == ref->getIdx() || ref->haveState(eRoomPeer_DecideLose) || ref->haveState(eRoomPeer_AlreadyHu) )
		{
			continue;
		}

		auto pMJCard = ref->getPlayerCard();
		if ( pMJCard->canPengWithCard(nCard))
		{
			return true;
		}

		if (pMJCard->canHuWitCard(nCard))
		{
			return true;
		}
	}

	return false;
}

bool XZMJRoom::isAnyPlayerRobotGang(uint8_t nInvokeIdx, uint8_t nCard)
{
	for (auto& ref : m_vMJPlayers)
	{
		if (ref == nullptr || nInvokeIdx == ref->getIdx() || ref->haveState(eRoomPeer_DecideLose) || ref->haveState(eRoomPeer_AlreadyHu) )
		{
			continue;
		}

		auto pMJCard = ref->getPlayerCard();
		if (pMJCard->canHuWitCard(nCard))
		{
			return true;
		}
	}

	return false;
}

void XZMJRoom::onAskForPengOrHuThisCard(uint8_t nInvokeIdx, uint8_t nCard, std::vector<uint8_t>& vOutWaitHuIdx, std::vector<uint8_t>& vOutWaitPengGangIdx, bool& isNeedWaitEat)
{
	isNeedWaitEat = false;
	for (auto& ref : m_vMJPlayers)
	{
		if (ref == nullptr || nInvokeIdx == ref->getIdx() || ref->haveState(eRoomPeer_DecideLose) || ref->haveState(eRoomPeer_AlreadyHu) )
		{
			continue;
		}

		Json::Value jsMsg;
		jsMsg["invokerIdx"] = nInvokeIdx;
		jsMsg["cardNum"] = nCard;

		Json::Value jsActs;
		auto pMJCard = ref->getPlayerCard();

		// check hu ;
		if (pMJCard->canHuWitCard(nCard))
		{
			jsActs[jsActs.size()] = eMJAct_Hu;
			vOutWaitHuIdx.push_back(ref->getIdx());
		}

		if (ref->haveState(eRoomPeer_AlreadyHu) == false)  // already hu player can not peng ,gang 
		{
			// check peng 
			if (pMJCard->canPengWithCard(nCard))
			{
				jsActs[jsActs.size()] = eMJAct_Peng;
				vOutWaitPengGangIdx.push_back(ref->getIdx());
			}

			// check ming gang 
			if (isCanGoOnMoPai() && pMJCard->canMingGangWithCard(nCard))
			{
				jsActs[jsActs.size()] = eMJAct_MingGang;
				// already add in peng ;  vWaitPengGangIdx
			}
		}

		if (jsActs.size() > 0)
		{
			jsActs[jsActs.size()] = eMJAct_Pass;
		}

		jsMsg["acts"] = jsActs;
		sendMsgToPlayer(jsMsg, MSG_PLAYER_WAIT_ACT_ABOUT_OTHER_CARD, ref->getSessionID());
		LOGFMTD("inform uid = %u act about other card room id = %u card = %u", ref->getUID(), getRoomID(), nCard);
	}
}

void XZMJRoom::onAskForRobotGang(uint8_t nInvokeIdx, uint8_t nCard, std::vector<uint8_t>& vOutCandinates)
{
	// send decalre gang msg ;
	Json::Value msg;
	msg["idx"] = nInvokeIdx;
	msg["actType"] = eMJAct_BuGang_Pre;
	msg["card"] = nCard;
	sendRoomMsg(msg, MSG_ROOM_ACT);

	// inform target player do this things 
	for (auto& ref : m_vMJPlayers)
	{
		if (ref == nullptr || nInvokeIdx == ref->getIdx() || ref->haveState(eRoomPeer_DecideLose) || ref->haveState(eRoomPeer_AlreadyHu) )
		{
			continue;
		}

		Json::Value jsMsg;
		jsMsg["invokerIdx"] = nInvokeIdx;
		jsMsg["cardNum"] = nCard;

		Json::Value jsActs;
		auto pMJCard = ref->getPlayerCard();
		// check hu 
		if (pMJCard->canHuWitCard(nCard))
		{
			jsActs[jsActs.size()] = eMJAct_Hu;
			vOutCandinates.push_back(ref->getIdx());
		}

		if (jsActs.size() > 0)
		{
			jsActs[jsActs.size()] = eMJAct_Pass;
		}

		jsMsg["acts"] = jsActs;
		sendMsgToPlayer(jsMsg, MSG_PLAYER_WAIT_ACT_ABOUT_OTHER_CARD, ref->getSessionID());
		LOGFMTD("inform uid = %u robot gang card = %u room id = %u ", ref->getUID(), nCard, getRoomID());
	}
}

uint8_t XZMJRoom::getNextActPlayerIdx(uint8_t nCurActIdx)
{
	for (uint8_t nIdx = nCurActIdx + 1; nIdx < getSeatCnt() * 2; ++nIdx)
	{
		auto nActIdx = nIdx % getSeatCnt();
		auto p = getMJPlayerByIdx(nActIdx);
		if (p && (p->haveState(eRoomPeer_DecideLose) == false) && (p->haveState(eRoomPeer_AlreadyHu) == false))
		{
			return nActIdx;
		}
	}

	LOGFMTE("why can not find a can do act player ? ");
	return 0;
}