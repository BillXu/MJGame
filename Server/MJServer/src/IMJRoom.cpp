#include "IMJRoom.h"
#include "IMJRoomState.h"
#include "RoomConfig.h"
#include <assert.h>
#include "IRoomManager.h"
#include "LogManager.h"
#include "IMJPlayer.h"
#include "IMJPlayerCard.h"
IMJRoom::~IMJRoom()
{
	for (auto& ref : m_vMJPlayers)
	{
		delete ref;
		ref = nullptr;
	}

	for (auto& ref : m_vRoomStates)
	{
		if (ref.second)
		{
			delete ref.second;
			ref.second = nullptr;
		}
	}
}

bool IMJRoom::init(IRoomManager* pRoomMgr, stBaseRoomConfig* pConfig, uint32_t nRoomID, Json::Value& vJsValue)
{
	// zero data 
	memset(m_vMJPlayers, 0, sizeof(m_vMJPlayers));
	m_pCurState = nullptr;
	m_pRoomMgr = pRoomMgr;
	m_nRoomID = nRoomID;
	m_pRoomConfig = pConfig;
	if (MAX_SEAT_CNT < pConfig->nMaxSeat)
	{
		assert(0 && "invalid seat cnt");
		pConfig->nMaxSeat = MAX_SEAT_CNT;
	}
	setBankIdx(-1);
	return true;
}

bool IMJRoom::onPlayerEnter(stEnterRoomData* pEnterRoomPlayer)
{
	uint8_t nEmptyIdx = -1;
	for (uint8_t nIdx = 0; nIdx < getSeatCnt(); ++nIdx)
	{
		if (m_vMJPlayers[nIdx] == nullptr)
		{
			nEmptyIdx = nIdx;
			break;
		}
	}

	if (nEmptyIdx == (uint8_t)-1)
	{
		return false;
	}

	auto pMJPlayer = doCreateMJPlayer();
	pMJPlayer->init(pEnterRoomPlayer);
	pMJPlayer->setIdx(nEmptyIdx);
	sitdown(pMJPlayer, nEmptyIdx);
	return true;
}

bool IMJRoom::onPlayerApplyLeave(uint32_t nPlayerUID)
{
	return false;
}

bool IMJRoom::isRoomFull()
{
	uint8_t nSeatCnt = getSeatCnt() < MAX_SEAT_CNT ? getSeatCnt() : MAX_SEAT_CNT;
	for (uint8_t nIdx = 0; nIdx < nSeatCnt; ++nIdx)
	{
		if (m_vMJPlayers[nIdx] == nullptr)
		{
			return false;
		}
	}
	return true;
}

uint32_t IMJRoom::getRoomID()
{
	return m_nRoomID;
}

void IMJRoom::update(float fDelta)
{
	getCurRoomState()->update(fDelta);
}

bool IMJRoom::onMessage(stMsg* prealMsg, eMsgPort eSenderPort, uint32_t nPlayerSessionID)
{
	return false;
}

bool IMJRoom::onMsg(Json::Value& prealMsg, uint16_t nMsgType, eMsgPort eSenderPort, uint32_t nSessionID)
{
	return getCurRoomState()->onMsg(prealMsg,nMsgType,eSenderPort,nSessionID);
}

void IMJRoom::sendRoomMsg(Json::Value& prealMsg, uint16_t nMsgType)
{
	for (auto& ref : m_vMJPlayers)
	{
		if (ref)
		{
			sendMsgToPlayer(prealMsg,nMsgType,ref->getSessionID());
		}
	}
}

void IMJRoom::sendMsgToPlayer(Json::Value& prealMsg, uint16_t nMsgType, uint32_t nSessionID)
{
	getRoomMgr()->sendMsg(prealMsg, nMsgType, nSessionID);
}

bool IMJRoom::sitdown(IMJPlayer* pPlayer, uint8_t nIdx)
{
	if (nIdx >= getSeatCnt())
	{
		assert("invalid sit down idx , please delete player obj");
		return false;
	}

	if (m_vMJPlayers[nIdx])
	{
		CLogMgr::SharedLogMgr()->ErrorLog("this pos already have player , find other seat nidx = %u",nIdx);
		return false;
	}
	m_vMJPlayers[nIdx] = pPlayer;
	pPlayer->setIdx(nIdx);

	// msg ;
	Json::Value jsMsg;
	jsMsg["idx"] = nIdx;
	jsMsg["uid"] = pPlayer->getUID();
	jsMsg["coin"] = pPlayer->getCoin();
	jsMsg["state"] = pPlayer->getState();
	sendRoomMsg(jsMsg, MSG_ROOM_PLAYER_ENTER);
	return true;
}

bool IMJRoom::standup(uint32_t nUID)
{
	for (auto& ref : m_vMJPlayers)
	{
		if ( ref && ref->getUID() == nUID)
		{
			// msg ;
			Json::Value jsMsg;
			jsMsg["idx"] = ref->getIdx();
			sendRoomMsg(jsMsg, MSG_ROOM_PLAYER_LEAVE);

			delete ref;
			ref = nullptr;
			return true;
		}
	}
	CLogMgr::SharedLogMgr()->ErrorLog("uid = %u , not sit down can not standup",nUID);
	return false;
}

uint8_t IMJRoom::getSeatCnt()
{
	return (uint8_t)getRoomConfig()->nMaxSeat;
}

IMJPlayer* IMJRoom::getMJPlayerBySessionID(uint32_t nSessionid)
{
	for (auto& ref : m_vMJPlayers)
	{
		if ( ref && ref->getSessionID() == nSessionid)
		{
			return ref;
		}
	}
	return nullptr;
}

IMJPlayer* IMJRoom::getMJPlayerByUID(uint32_t nUID)
{
	for (auto& ref : m_vMJPlayers)
	{
		if (ref &&  ref->getUID() == nUID)
		{
			return ref;
		}
	}
	return nullptr;
}

bool IMJRoom::canStartGame()
{
	for (uint8_t nIdx = 0; nIdx < getSeatCnt(); ++nIdx)
	{
		if ( !m_vMJPlayers[nIdx])
		{
			return false;
		}

		if (m_vMJPlayers[nIdx]->haveState(eRoomPeer_Ready) == false)
		{
			return false;
		}
	}
	return true;
}

void IMJRoom::goToState(IMJRoomState* pTargetState, Json::Value* jsValue )
{
	if (pTargetState == getCurRoomState() )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("go to the same state %d , room id = %d ? ", pTargetState->getStateID(), getRoomID());
	}

	CLogMgr::SharedLogMgr()->SystemLog("roomID = %u 进入房间状态： %u", getRoomID(), pTargetState->getStateID());

	getCurRoomState()->leaveState();
	m_pCurState = pTargetState;
	m_pCurState->enterState(this, *jsValue);

	//stMsgRoomEnterNewState msgNewState;
	//msgNewState.m_fStateDuring = m_pCurState->getStateDuring();
	//msgNewState.nNewState = m_pCurState->getStateID();
	CLogMgr::SharedLogMgr()->SystemLog("not tell client state changed");
}

void IMJRoom::goToState(uint16_t nStateID, Json::Value* jsValue )
{
	auto pSateIter = m_vRoomStates.find(nStateID);
	if (pSateIter == m_vRoomStates.end())
	{
		CLogMgr::SharedLogMgr()->ErrorLog("can not find state id = %u , so can not to it",nStateID );
		return;
	}
	goToState(pSateIter->second, jsValue);
}

uint8_t IMJRoom::getBankerIdx()
{
	return m_nBankerIdx;
}

void IMJRoom::setBankIdx(uint8_t nIdx)
{
	m_nBankerIdx = nIdx;
}

void IMJRoom::onPlayerSetReady(uint8_t nIdx)
{
	auto pPlayer = getMJPlayerByIdx(nIdx);
	if (!pPlayer)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("idx = %u target player is null ptr can not set ready",nIdx);
		return;
	}
	pPlayer->setState(eRoomPeer_Ready);
	// msg ;
	Json::Value jsMsg;
	jsMsg["idx"] = nIdx;
	sendRoomMsg(jsMsg, MSG_ROOM_PLAYER_READY);
}

IMJPlayer* IMJRoom::getMJPlayerByIdx(uint8_t nIdx)
{
	if ( nIdx >= getSeatCnt())
	{
		CLogMgr::SharedLogMgr()->ErrorLog("invalid seat idx = %u get player",nIdx );
		return nullptr;
	}
	return m_vMJPlayers[nIdx];
}

// mj function ;
void IMJRoom::onWaitPlayerAct(uint8_t nIdx, bool& isCanPass)
{
	auto pPlayer = getMJPlayerByIdx(nIdx);
	if (!pPlayer)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("player idx = %u is null can not tell it wait act",nIdx);
		return;
	}
	auto pMJCard = pPlayer->getPlayerCard();
	// send msg to tell player do act 
	Json::Value jsArrayActs;

	if ( isCanGoOnMoPai() )
	{
		// check bu gang .
		IMJPlayerCard::VEC_CARD vCards;
		pMJCard->getHoldCardThatCanBuGang(vCards);
		for (auto& ref : vCards)
		{
			Json::Value jsAct;
			jsAct["act"] = eMJAct_BuGang;
			jsAct["cardNum"] = ref;
			jsArrayActs[jsArrayActs.size()] = jsAct;
		}
		// check an gang .
		vCards.clear();
		pMJCard->getHoldCardThatCanAnGang(vCards);
		for (auto& ref : vCards)
		{
			Json::Value jsAct;
			jsAct["act"] = eMJAct_AnGang;
			jsAct["cardNum"] = ref;
			jsArrayActs[jsArrayActs.size()] = jsAct;
		}
	}

	// check hu .
	if ( pMJCard->isHoldCardCanHu())
	{
		Json::Value jsAct;
		jsAct["act"] = eMJAct_AnGang;
		jsAct["cardNum"] = pMJCard->getNewestFetchedCard();
		jsArrayActs[jsArrayActs.size()] = jsAct;
	}

	isCanPass = jsArrayActs.empty() == false;

	// add default alwasy chu , infact need not add , becaust it alwasy in ,but compatable with current client ;
	Json::Value jsAct;
	jsAct["act"] = eMJAct_Chu;
	jsAct["cardNum"] = getAutoChuCardWhenWaitActTimeout(nIdx);
	jsArrayActs[jsArrayActs.size()] = jsAct;

	Json::Value jsMsg;
	jsMsg["acts"] = jsArrayActs;
	sendMsgToPlayer(jsMsg, MSG_PLAYER_WAIT_ACT_AFTER_RECEIVED_CARD, pPlayer->getSessionID());
	
	CLogMgr::SharedLogMgr()->PrintLog("tell player idx = %u do act size = %u",nIdx,jsArrayActs.size());
}

uint8_t IMJRoom::getAutoChuCardWhenWaitActTimeout(uint8_t nIdx)
{
	auto pPlayer = getMJPlayerByIdx(nIdx);
	if (!pPlayer)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("why this player is null idx = %u , can not chu card",nIdx);
		return 0;
	}
	return pPlayer->getPlayerCard()->getNewestFetchedCard();
}

uint8_t IMJRoom::getAutoChuCardWhenWaitChuTimeout(uint8_t nIdx)
{
	auto pPlayer = getMJPlayerByIdx(nIdx);
	if (!pPlayer)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("why this player is null idx = %u , can not chu card", nIdx);
		return 0;
	}
	IMJPlayerCard::VEC_CARD vCard;
	pPlayer->getPlayerCard()->getHoldCard(vCard);
	if (vCard.empty() == false)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("hold card can not be empty");
		assert(0&&"hold card must no be empty");
		return 0;
	}
	return vCard.back();
}

void IMJRoom::onPlayerMo(uint8_t nIdx)
{
	auto pPlayer = getMJPlayerByIdx(nIdx);
	if (!pPlayer)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("why this player is null idx = %u , can not mo", nIdx);
		return;
	}
	pPlayer->getPlayerCard()->onMoCard(distributeOneCard());
}

void IMJRoom::onPlayerPeng(uint8_t nIdx, uint8_t nCard, uint8_t nInvokeIdx)
{
	auto pPlayer = getMJPlayerByIdx(nIdx);
	auto pInvoker = getMJPlayerByIdx(nInvokeIdx);
	if (!pPlayer || !pInvoker)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("why this player is null idx = %u , can not peng", nIdx);
		return;
	}

	if (pPlayer->getPlayerCard()->onPeng(nCard) == false)
	{
		CLogMgr::SharedLogMgr()->ErrorLog( "nidx = %u peng card = %u error",nIdx,nCard );
	}
	pInvoker->getPlayerCard()->onCardBeGangPengEat(nCard);
}

void IMJRoom::onPlayerEat(uint8_t nIdx, uint8_t nCard, uint8_t nWithA, uint8_t nWithB, uint8_t nInvokeIdx)
{
	auto pPlayer = getMJPlayerByIdx(nIdx);
	auto pInvoker = getMJPlayerByIdx(nInvokeIdx);
	if (!pPlayer || !pInvoker )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("why this player is null idx = %u , can not eat", nIdx);
		return;
	}

	if (pPlayer->getPlayerCard()->onEat(nCard,nWithA,nWithB) == false)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("nidx = %u eat card = %u error, with a = %u ,b = %u", nIdx, nCard,nWithA,nWithB);
	}
	pInvoker->getPlayerCard()->onCardBeGangPengEat(nCard);
}

void IMJRoom::onPlayerMingGang(uint8_t nIdx, uint8_t nCard, uint8_t nInvokeIdx)
{
	auto pPlayer = getMJPlayerByIdx(nIdx);
	auto pInvoker = getMJPlayerByIdx(nInvokeIdx);
	if (!pPlayer || !pInvoker)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("why this player is null idx = %u , can not ming gang", nIdx);
		return;
	}

	auto nGangGetCard = distributeOneCard();
	if (pPlayer->getPlayerCard()->onMingGang(nCard, nGangGetCard) == false)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("nidx = %u ming gang card = %u error,", nIdx, nCard );
	}
	pInvoker->getPlayerCard()->onCardBeGangPengEat(nCard);
}

void IMJRoom::onPlayerAnGang(uint8_t nIdx, uint8_t nCard)
{
	auto pPlayer = getMJPlayerByIdx(nIdx);
	if (!pPlayer)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("why this player is null idx = %u , can not an gang", nIdx);
		return;
	}

	auto nGangGetCard = distributeOneCard();
	if (pPlayer->getPlayerCard()->onAnGang(nCard, nGangGetCard) == false)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("nidx = %u an gang card = %u error,", nIdx, nCard);
	}
}

void IMJRoom::onPlayerBuGang(uint8_t nIdx, uint8_t nCard)
{
	auto pPlayer = getMJPlayerByIdx(nIdx);
	if (!pPlayer)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("why this player is null idx = %u , can not bu gang", nIdx);
		return;
	}

	auto nGangCard = distributeOneCard();
	if (pPlayer->getPlayerCard()->onBuGang(nCard, nGangCard) == false)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("nidx = %u bu gang card = %u error,", nIdx, nCard);
	}
}

void IMJRoom::onPlayerHu(std::vector<uint8_t>& vHuIdx, uint8_t nCard, uint8_t nInvokeIdx)
{

}

void IMJRoom::onPlayerChu(uint8_t nIdx, uint8_t nCard)
{
	auto pPlayer = getMJPlayerByIdx(nIdx);
	if (!pPlayer)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("why this player is null idx = %u , can not chu", nIdx);
		return;
	}

	if (!pPlayer->getPlayerCard()->onChuCard(nCard))
	{
		CLogMgr::SharedLogMgr()->ErrorLog("chu card error idx = %u , card = %u",nIdx,nCard );
	}
}

bool IMJRoom::isAnyPlayerPengOrHuThisCard(uint8_t nInvokeIdx, uint8_t nCard)
{
	for (auto& ref : m_vMJPlayers)
	{
		if (ref == nullptr || nInvokeIdx == ref->getIdx() )
		{
			continue;
		}

		auto pMJCard = ref->getPlayerCard();
		uint8_t a = 0, b = 0;
		if (pMJCard->canPengWithCard(nCard) || pMJCard->canEatCard(nCard, a, b) || pMJCard->canHuWitCard(nCard))
		{
			return true;
		}

		if (pMJCard->canMingGangWithCard(nCard) && isCanGoOnMoPai()) // must can gang 
		{
			return true;
		}
	}

	return false;
}

void IMJRoom::onAskForPengOrHuThisCard(uint8_t nInvokeIdx, uint8_t nCard, std::vector<uint8_t>& vWaitHuIdx, std::vector<uint8_t>& vWaitPengGangIdx, bool& isNeedWaitEat)
{

}

bool IMJRoom::isAnyPlayerRobotGang(uint8_t nInvokeIdx, uint8_t nCard)
{
	for (auto& ref : m_vMJPlayers)
	{
		if (ref == nullptr || nInvokeIdx == ref->getIdx())
		{
			continue;
		}

		auto pMJCard = ref->getPlayerCard();
		uint8_t a = 0, b = 0;
		if (pMJCard->canHuWitCard(nCard))
		{
			return true;
		}
	}

	return false;
}

void IMJRoom::onAskForRobotGang(uint8_t nInvokeIdx, uint8_t nCard, std::vector<uint8_t>& vCandinates)
{

}

uint8_t IMJRoom::getNextActPlayerIdx(uint8_t nCurActIdx)
{
	return (nCurActIdx + 1) % getSeatCnt();
}

bool IMJRoom::isGameOver()
{
	return !isCanGoOnMoPai();
}

bool IMJRoom::addRoomState(IMJRoomState* pState)
{
	auto iter = m_vRoomStates.find(pState->getStateID());
	if (iter == m_vRoomStates.end())
	{
		m_vRoomStates[pState->getStateID()] = pState;
	}
	CLogMgr::SharedLogMgr()->ErrorLog("already add this state id =%u , be remember delete failed add obj",pState->getStateID());
	return false;
}