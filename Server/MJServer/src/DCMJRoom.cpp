#include "DCMJRoom.h"
#include "DCMJPlayer.h"
#include "DCMJRoomStateWaitPlayerReady.h"
#include "MJRoomStateWaitPlayerChu.h"
#include "MJRoomStateWaitPlayerAct.h"
#include "MJRoomStateStartGame.h"
#include "MJRoomStateGameEnd.h"
#include "MJRoomStateDoPlayerAct.h"
#include "MJRoomStateAskForPengOrHu.h"
#include "DCMJRoomStateWaitPlayerBuyCode.h"
#include "IGameRoomManager.h"
#include "ServerMessageDefine.h"
bool DCMJRoom::init(IGameRoomManager* pRoomMgr, stBaseRoomConfig* pConfig, uint32_t nRoomID, Json::Value& vJsValue)
{
	IMJRoom::init(pRoomMgr,pConfig,nRoomID,vJsValue);
	// create room state ;
	IMJRoomState* vState[] = {
		new DCMJRoomStateWaitReady(), new DCMJRoomstateWaitPlayerBuyCode(), new MJRoomStateWaitPlayerChu(), new MJRoomStateWaitPlayerAct(), new MJRoomStateStartGame()
		, new MJRoomStateGameEnd(), new MJRoomStateDoPlayerAct(), new MJRoomStateAskForPengOrHu()
	};
	for (uint8_t nIdx = 0; nIdx < sizeof(vState) / sizeof(IMJRoomState*); ++nIdx)
	{
		addRoomState(vState[nIdx]);
	}
	setInitState(vState[0]);
	m_tPoker.initAllCard(eMJ_COMMON);
	m_isEnableYaoHu = false;
	m_isEnableBuyCode = false;
	if (vJsValue["isEnableBuycode"].isNull() == false)
	{
		m_isEnableBuyCode = vJsValue["isEnableBuycode"].asUInt() == 1 ;
	}
	else
	{
		LOGFMTE("create room do not have enableBuycode ");
	}

	if (vJsValue["isEableYao"].isNull() == false)
	{
		m_isEnableYaoHu = vJsValue["isEnableBuycode"].asUInt() == 1;
	}
	else
	{
		LOGFMTE("create room do not have enableYao");
	}

	return true;
}

void DCMJRoom::willStartGame()
{
	IMJRoom::willStartGame();
	m_nHuPlayerIdx = -1;
	if (getBankerIdx() == (uint8_t)-1)  // first time rand a new bank ;
	{
		setBankIdx(rand() % getSeatCnt());
	}
}

void DCMJRoom::startGame()
{
	IMJRoom::startGame();
	for (auto& p : m_vMJPlayers)
	{
		if (!p)
		{
			LOGFMTE("why have nullptr this room id = %u",getRoomID());
			continue;
		}
		auto pCard = (DCMJPlayerCard*)p->getPlayerCard();
		pCard->setEnableYao(m_isEnableYaoHu);
		if (p->getIdx() == getBankerIdx())
		{
			pCard->setIsHaveTianHuBao(true);
		}
	}
}

void DCMJRoom::onGameDidEnd()
{
	if (getBankerIdx() != m_nHuPlayerIdx)  // banker not hu , so change to next player to banker 
	{
		setBankIdx( (getBankerIdx() + 1 )%getSeatCnt() );
	}

	IMJRoom::onGameDidEnd();
	if (getDelegate())
	{
		getDelegate()->onDidGameOver(this);
		return;
	}
}

void DCMJRoom::onGameEnd()
{
	IMJRoom::onGameEnd();
	// send result 
	if (m_nHuPlayerIdx == (uint8_t)-1)
	{
		// liuju ;
		Json::Value jsMsg;
		jsMsg["isLiuJu"] = 1;
		jsMsg["bankerIdx"] = getBankerIdx();
		// send msg ;
		sendRoomMsg(jsMsg, MSG_DC_ROOM_RESULT);
	}
}

bool DCMJRoom::isGameOver()
{
	if (!isCanGoOnMoPai())
	{
		return true;
	}

	// is any one hu 
	return m_nHuPlayerIdx != (uint8_t)-1;
}

bool DCMJRoom::onPlayerApplyLeave(uint32_t nPlayerUID)
{
	auto pPlayer = getMJPlayerByUID(nPlayerUID);
	if (!pPlayer)
	{
		LOGFMTE("you are not in room id = %u , how to leave this room ? uid = %u", getRoomID(), nPlayerUID);
		return false;
	}

	auto curState = getCurRoomState()->getStateID();
	if (eRoomSate_WaitReady == curState || eRoomState_GameEnd == curState)
	{
		// direct leave just stand up ;
		stMsgSvrDoLeaveRoom msgdoLeave;
		msgdoLeave.nCoin = pPlayer->getCoin();
		msgdoLeave.nGameType = getRoomType();
		msgdoLeave.nRoomID = getRoomID();
		msgdoLeave.nUserUID = pPlayer->getUID();
		msgdoLeave.nMaxFangXingType = 0;
		msgdoLeave.nMaxFanShu = 0;
		msgdoLeave.nRoundsPlayed = 1;
		msgdoLeave.nGameOffset = pPlayer->getOffsetCoin();
		getRoomMgr()->sendMsg(&msgdoLeave, sizeof(msgdoLeave), pPlayer->getSessionID());

		return standup(nPlayerUID);

	}
	else
	{
		pPlayer->setState((pPlayer->getState() | eRoomPeer_DelayLeave));
		LOGFMTE("decide player already sync data uid = %u", pPlayer->getUID());
	}
	return true;
}

IMJPlayer* DCMJRoom::doCreateMJPlayer()
{
	return new DCMJPlayer();
}

void DCMJRoom::onPlayerHu(std::vector<uint8_t>& vHuIdx, uint8_t nCard, uint8_t nInvokeIdx)
{
	if (vHuIdx.size() != 1 || vHuIdx.front() != nInvokeIdx)
	{
		LOGFMTE("du chang ma jiang can only hu room id = %u",getRoomID() );
		return;
	}

	auto pWiner = (DCMJPlayer*)getMJPlayerByIdx(nInvokeIdx);
	auto pPlayerCard = (DCMJPlayerCard*)pWiner->getPlayerCard();
	eFanxingType eType;
	bool isUsedBao = false;
	uint8_t nBeiShu = 0;
	uint8_t nGangCnt = 0;
	if (!pPlayerCard->onDoHu(eType, isUsedBao, nBeiShu, nGangCnt))
	{
		LOGFMTE("room id = %u player uid = %u not hu , you say hu ? ",getRoomID(),pWiner->getUID());
		return;
	}

	m_nHuPlayerIdx = pWiner->getIdx();
	pWiner->setState(eRoomPeer_AlreadyHu);

	uint8_t nWinerBuyCode = pWiner->getBuyCode();
	if (pWiner->getIdx() == getBankerIdx())
	{
		nBeiShu *= 2;
	}

	Json::Value jsResult;
	// do caculate ;
	uint8_t nWinCoin = 0;
	uint8_t nCodeWin = 0;
	for (auto& pLose : m_vMJPlayers)
	{
		if (pLose->getIdx() == pWiner->getIdx())
		{
			continue;
		}

		uint8_t nLoseBuyCode = ((DCMJPlayer*)pLose)->getBuyCode();
		bool isLoseBanker = pLose->getIdx() == getBankerIdx();
		nWinCoin += (nBeiShu * (isLoseBanker ? 2 : 1));
		nCodeWin += (nWinerBuyCode + nLoseBuyCode);
		uint8_t nLoseCoin = (nBeiShu * (isLoseBanker ? 2 : 1) + nWinerBuyCode + nLoseBuyCode);
		pLose->addOffsetCoin((int8_t)nLoseCoin * -1);

		Json::Value jsItem;
		jsItem["idx"] = pLose->getIdx();
		jsItem["offset"] = nBeiShu * (isLoseBanker ? 2 : 1);
		jsItem["codeOffset"] = (nWinerBuyCode + nLoseBuyCode);
		jsResult[jsResult.size()] = jsItem;
	}
	pWiner->addOffsetCoin(nWinCoin + nCodeWin);
	Json::Value jsItem;
	jsItem["idx"] = pWiner->getIdx();
	jsItem["offset"] = nWinCoin;
	jsItem["codeOffset"] = nCodeWin;
	jsResult[jsResult.size()] = jsItem;

	Json::Value jsWinDetial;
	jsWinDetial["fanxing"] = eType;
	jsWinDetial["isUsedBao"] = isUsedBao ? 1 : 0;
	jsWinDetial["nGangCnt"] = nGangCnt;

	Json::Value jsMsg;
	jsMsg["isLiuJu"] = 0;
	jsMsg["bankerIdx"] = getBankerIdx();
	jsMsg["winDetail"] = jsWinDetial;
	jsMsg["result"] = jsResult;
	// send msg ;
	sendRoomMsg(jsMsg, MSG_DC_ROOM_RESULT);
 }

bool DCMJRoom::isEnableBuyCode()
{
	return m_isEnableBuyCode;
}