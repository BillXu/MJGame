#include "IMJRoom.h"
#include "IMJRoomState.h"
#include "RoomConfig.h"
#include <assert.h>
#include "IRoomManager.h"
#include "LogManager.h"
#include "IMJPlayer.h"
#include "IMJPlayerCard.h"
#include "IMJPoker.h"
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

void IMJRoom::sendRoomInfo(uint32_t nSessionID)
{
	Json::Value jsMsg;
	jsMsg["roomID"] = getRoomID();
	jsMsg["configID"] = getRoomConfig()->nConfigID;
	jsMsg["roomState"] = getCurRoomState()->getStateID();
	Json::Value arrPlayers;
	for (auto& pPlayer : m_vMJPlayers )
	{
		if (pPlayer == nullptr)
		{
			continue;
		}
		Json::Value jsPlayer;
		jsPlayer["idx"] = pPlayer->getIdx();
		jsPlayer["uid"] = pPlayer->getUID();
		jsPlayer["coin"] = pPlayer->getCoin();
		jsPlayer["state"] = pPlayer->getState();
		arrPlayers[pPlayer->getIdx()] = jsPlayer;
	}

	jsMsg["waitTimer"] = 0;
	if (getCurRoomState()->getStateID() == eRoomState_WaitPlayerAct)
	{
		jsMsg["waitTimer"] = getCurRoomState()->getStateDuring();
	}

	jsMsg["players"] = arrPlayers;
	sendMsgToPlayer(jsMsg, MSG_ROOM_INFO, nSessionID);
	CLogMgr::SharedLogMgr()->PrintLog("send msg room info msg to player session id = %u", nSessionID);
}

void IMJRoom::sendPlayersCardInfo(uint32_t nSessionID)
{
	if (getCurRoomState()->getStateID() == eRoomSate_WaitReady || eRoomState_GameEnd == getCurRoomState()->getStateID())
	{
		CLogMgr::SharedLogMgr()->PrintLog("current room not start game , so need not send runtime info msg");
		return;
	}
	Json::Value jsmsg;
	Json::Value vPeerCards;
	for (auto& pp : m_vMJPlayers )
	{
		if (pp == nullptr || pp->haveState(eRoomPeer_CanAct) == false)
		{
			continue;
		}

		auto pCard = pp->getPlayerCard();
		// svr : { bankerIdx : 2, leftCardCnt : 32 ,playersCard: [ { idx : 2,queType: 2, anPai : [2,3,4,34], mingPai : [ 23,67,32] , huPai : [1,34], chuPai: [2,34,4] },{ anPai : [2,3,4,34], mingPai : [ 23,67,32] , huPai : [1,34] }, .... ] }
		// { idx : 2,queType: 2, anPai : [2,3,4,34], mingPai : [ 23,67,32] , huPai : [1,34], chuPai: [2,34,4] }
		Json::Value jsCardInfo;
		jsCardInfo["idx"] = pp->getIdx();
		jsCardInfo["queType"] = 0;

		IMJPlayerCard::VEC_CARD vAnPai, vMingPai, vChuPai , temp;
		pCard->getHoldCard(vAnPai);
		pCard->getChuedCard(vChuPai);
		
		pCard->getEatedCard(vMingPai);
		pCard->getPengedCard(temp);
		for (auto& pc : temp)
		{
			vMingPai.push_back(pc); vMingPai.push_back(pc); vMingPai.push_back(pc);
		}
		temp.clear();
		pCard->getGangedCard(temp);
		for (auto& pc : temp)
		{
			vMingPai.push_back(pc); vMingPai.push_back(pc); vMingPai.push_back(pc); vMingPai.push_back(pc);
		}
		temp.clear();

		auto toJs = [](IMJPlayerCard::VEC_CARD& vCards, Json::Value& js )
		{
			for (auto& c : vCards)
			{
				js[js.size()] = c;
			}
		};

		Json::Value jsMingPai, jsAnPai, jsChuPai, jaHupai;
		toJs(vMingPai, jsMingPai); toJs(vAnPai, jsAnPai); toJs(vChuPai,jsChuPai);
		jsCardInfo["mingPai"] = jsMingPai; jsCardInfo["anPai"] = jsAnPai; jsCardInfo["chuPai"] = jsChuPai; jsCardInfo["huPai"] = jaHupai;
		vPeerCards[vPeerCards.size()] = jsCardInfo;
	}

	jsmsg["playersCard"] = vPeerCards;
	jsmsg["bankerIdx"] = getBankerIdx();
	jsmsg["curActIdex"] = getCurRoomState()->getCurIdx();
	jsmsg["leftCardCnt"] = getMJPoker()->getLeftCardCount();
	sendMsgToPlayer(jsmsg, MSG_ROOM_PLAYER_CARD_INFO, nSessionID);
	CLogMgr::SharedLogMgr()->PrintLog("send player card infos !");
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

void IMJRoom::startGame()
{
	for (auto& pPlayer : m_vMJPlayers)
	{
		if (pPlayer)
		{
			pPlayer->onStartGame();
		}
	}

	// distribute card 
	Json::Value msg;
	Json::Value peerCards[4]; // used for sign for msg ;

	uint8_t nDice = rand() % getSeatCnt();
	auto pPoker = getMJPoker();
	pPoker->shuffle();
	for (auto& pPlayer : m_vMJPlayers)
	{
		if (!pPlayer)
		{
			CLogMgr::SharedLogMgr()->ErrorLog("why player is null hz mj must all player is not null");
			continue;
		}

		CLogMgr::SharedLogMgr()->PrintLog("distribute card for player idx = %u",pPlayer->getIdx());
		for (uint8_t nIdx = 0; nIdx < 13; ++nIdx)
		{
			auto nCard = pPoker->distributeOneCard();
			pPlayer->getPlayerCard()->addDistributeCard(nCard);

			peerCards[pPlayer->getIdx()][nIdx] = nCard; // sign for msg ;
			CLogMgr::SharedLogMgr()->PrintLog("card idx = %u card number = %u", nIdx,nCard);
		}

		if (getBankerIdx() == pPlayer->getIdx())
		{
			auto nCard = pPoker->distributeOneCard();
			pPlayer->getPlayerCard()->onMoCard(nCard);
			peerCards[pPlayer->getIdx()][13u] = nCard;  // sign for msg ;
		}
	}

	// construct msg ;
	msg["dice"] = nDice;
	msg["banker"] = m_nBankerIdx;
	Json::Value arrPeerCards;
	for (uint8_t nIdx = 0; nIdx < getSeatCnt(); ++nIdx)
	{
		Json::Value peer;
		peer["cards"] = peerCards[nIdx];
		arrPeerCards[nIdx] = peer;
	}
	msg["peerCards"] = arrPeerCards;
	sendRoomMsg(msg, MSG_ROOM_START_GAME);
	CLogMgr::SharedLogMgr()->SystemLog("room id = %u start game !",getRoomID());
}

void IMJRoom::willStartGame()
{
	for (auto& pPlayer : m_vMJPlayers)
	{
		if (pPlayer)
		{
			pPlayer->onWillStartGame();
		}
	}
}

void IMJRoom::onGameEnd()
{
	for (auto& pPlayer : m_vMJPlayers)
	{
		if (pPlayer)
		{
			pPlayer->onGameEnd();
		}
	}
}

void IMJRoom::onGameDidEnd()
{
	for (auto& pPlayer : m_vMJPlayers)
	{
		if (pPlayer)
		{
			pPlayer->onGameDidEnd();
		}
	}
}

bool IMJRoom::canStartGame()
{
	uint8_t nReadyCnt = 0;
	for (auto& pPlayer : m_vMJPlayers)
	{
		if (pPlayer && pPlayer->haveState(eRoomPeer_Ready) )
		{
			++nReadyCnt;
		}
	}
	return nReadyCnt == getSeatCnt();
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
		jsAct["act"] = eMJAct_Hu;
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

	auto nNewCard = getMJPoker()->distributeOneCard();
	pPlayer->getPlayerCard()->onMoCard(nNewCard);
	pPlayer->clearGangFlag();
	// send msg ;
	Json::Value msg;
	msg["idx"] = nIdx;
	msg["actType"] = eMJAct_Mo;
	msg["card"] = nNewCard;
	sendRoomMsg(msg, MSG_ROOM_ACT);
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

	Json::Value jsmsg;
	jsmsg["idx"] = nIdx;
	jsmsg["actType"] = eMJAct_Peng;
	jsmsg["card"] = nCard;
	sendRoomMsg(jsmsg, MSG_ROOM_ACT);
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

	// send msg ;
	Json::Value jsmsg;
	jsmsg["idx"] = nIdx;
	jsmsg["actType"] = eMJAct_Chi;
	jsmsg["card"] = nCard;
	Json::Value jseatwith;
	jseatwith[jseatwith.size()] = nWithA;
	jseatwith[jseatwith.size()] = nWithB;
	jsmsg["eatWith"] = jseatwith;
	sendRoomMsg(jsmsg, MSG_ROOM_ACT);
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
	pPlayer->signGangFlag();

	auto nGangGetCard = getMJPoker()->distributeOneCard();
	if (pPlayer->getPlayerCard()->onMingGang(nCard, nGangGetCard) == false)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("nidx = %u ming gang card = %u error,", nIdx, nCard );
	}
	pInvoker->getPlayerCard()->onCardBeGangPengEat(nCard);

	// send msg 
	Json::Value msg;
	msg["idx"] = nIdx;
	msg["actType"] = eMJAct_MingGang;
	msg["card"] = nCard;
	msg["gangCard"] = nGangGetCard;
	sendRoomMsg(msg, MSG_ROOM_ACT);
}

void IMJRoom::onPlayerAnGang(uint8_t nIdx, uint8_t nCard)
{
	auto pPlayer = getMJPlayerByIdx(nIdx);
	if (!pPlayer)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("why this player is null idx = %u , can not an gang", nIdx);
		return;
	}
	pPlayer->signGangFlag();
	auto nGangGetCard = getMJPoker()->distributeOneCard();
	if (pPlayer->getPlayerCard()->onAnGang(nCard, nGangGetCard) == false)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("nidx = %u an gang card = %u error,", nIdx, nCard);
	}

	// send msg ;
	Json::Value msg;
	msg["idx"] = nIdx;
	msg["actType"] = eMJAct_AnGang;
	msg["card"] = nCard;
	msg["gangCard"] = nGangGetCard;
	sendRoomMsg(msg, MSG_ROOM_ACT);
}

void IMJRoom::onPlayerBuGang(uint8_t nIdx, uint8_t nCard)
{
	auto pPlayer = getMJPlayerByIdx(nIdx);
	if (!pPlayer)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("why this player is null idx = %u , can not bu gang", nIdx);
		return;
	}
	pPlayer->signGangFlag();
	auto nGangCard = getMJPoker()->distributeOneCard();
	if (pPlayer->getPlayerCard()->onBuGang(nCard, nGangCard) == false)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("nidx = %u bu gang card = %u error,", nIdx, nCard);
	}

	// send msg 
	Json::Value msg;
	msg["idx"] = nIdx;
	msg["actType"] = eMJAct_BuGang_Done;
	msg["card"] = nCard;
	msg["gangCard"] = nGangCard;
	sendRoomMsg(msg, MSG_ROOM_ACT);
}

void IMJRoom::onPlayerHu(std::vector<uint8_t>& vHuIdx, uint8_t nCard, uint8_t nInvokeIdx)
{
	// send msg 
	Json::Value msg;
	msg["idx"] = vHuIdx.front();
	msg["actType"] = eMJAct_Hu;
	msg["card"] = nCard;
	sendRoomMsg(msg, MSG_ROOM_ACT);
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

	// send msg ;
	Json::Value msg;
	msg["idx"] = nIdx;
	msg["actType"] = eMJAct_Chu;
	msg["card"] = nCard;
	sendRoomMsg(msg, MSG_ROOM_ACT);
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
		if (pMJCard->canPengWithCard(nCard) || pMJCard->canHuWitCard(nCard))
		{
			return true;
		}

		if (isCanGoOnMoPai() && pMJCard->canMingGangWithCard(nCard) ) // must can gang 
		{
			return true;
		}

		if (ref->getIdx() == (nInvokeIdx + 1) % getSeatCnt())
		{
			uint8_t a = 0, b = 0;
			if (pMJCard->canEatCard(nCard, a, b))
			{
				return true;
			}
		}
	}

	return false;
}

void IMJRoom::onAskForPengOrHuThisCard(uint8_t nInvokeIdx, uint8_t nCard, std::vector<uint8_t>& vWaitHuIdx, std::vector<uint8_t>& vWaitPengGangIdx, bool& isNeedWaitEat)
{
	for (auto& ref : m_vMJPlayers)
	{
		if (ref == nullptr || nInvokeIdx == ref->getIdx())
		{
			continue;
		}

		Json::Value jsMsg;
		jsMsg["invokerIdx"] = nInvokeIdx;
		jsMsg["cardNum"] = nCard;

		Json::Value jsActs;
		auto pMJCard = ref->getPlayerCard();

		// check peng 
		if (pMJCard->canPengWithCard(nCard))
		{
			jsActs[jsActs.size()] = eMJAct_Peng;
			vWaitPengGangIdx.push_back(ref->getIdx());
		}

		// check ming gang 
		if ( isCanGoOnMoPai() && pMJCard->canMingGangWithCard(nCard))
		{
			jsActs[jsActs.size()] = eMJAct_MingGang;
			// already add in peng ;  vWaitPengGangIdx
		}

		if (ref->getIdx() == (nInvokeIdx + 1) % getSeatCnt())
		{
			uint8_t a = 0, b = 0;
			isNeedWaitEat = false;
			if (pMJCard->canEatCard(nCard, a, b))
			{
				isNeedWaitEat = true;
				jsActs[jsActs.size()] = eMJAct_Chi;
			}
		}

		// check hu ;
		if (pMJCard->canHuWitCard(nCard))
		{
			jsActs[jsActs.size()] = eMJAct_Hu;
			vWaitHuIdx.push_back(ref->getIdx());
		}

		if (jsActs.size() > 0)
		{
			jsActs[jsActs.size()] = eMJAct_Pass;
		}

		jsMsg["acts"] = jsActs;
		sendMsgToPlayer(jsMsg, MSG_PLAYER_WAIT_ACT_ABOUT_OTHER_CARD, ref->getSessionID());
		CLogMgr::SharedLogMgr()->PrintLog("inform uid = %u act about other card room id = %u card = %u", ref->getUID(), getRoomID(),nCard );
	}
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
	// send decalre gang msg ;
	Json::Value msg;
	msg["idx"] = nInvokeIdx;
	msg["actType"] = eMJAct_BuGang_Pre;
	msg["card"] = nCard;
	sendRoomMsg(msg, MSG_ROOM_ACT);

	// inform target player do this things 
	for (auto& ref : m_vMJPlayers)
	{
		if (ref == nullptr || nInvokeIdx == ref->getIdx())
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
			vCandinates.push_back(ref->getIdx());
		}

		if (jsActs.size() > 0)
		{
			jsActs[jsActs.size()] = eMJAct_Pass;
		}

		jsMsg["acts"] = jsActs;
		sendMsgToPlayer(jsMsg, MSG_PLAYER_WAIT_ACT_ABOUT_OTHER_CARD, ref->getSessionID());
		CLogMgr::SharedLogMgr()->PrintLog("inform uid = %u robot gang card = %u room id = %u ", ref->getUID(),nCard, getRoomID());
	}
}

uint8_t IMJRoom::getNextActPlayerIdx(uint8_t nCurActIdx)
{
	return (nCurActIdx + 1) % getSeatCnt();
}

bool IMJRoom::isGameOver()
{
	return !isCanGoOnMoPai();
}

bool IMJRoom::isCanGoOnMoPai()
{
	return getMJPoker()->getLeftCardCount() > 0 ;
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