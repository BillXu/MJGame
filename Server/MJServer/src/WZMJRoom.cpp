#include "WZMJRoom.h"
#include "WZMJPlayer.h"
#include "log4z.h"
#include "RoomConfig.h"
#include "WZMJPlayerCard.h"
#include "WZMJRoomStateWaitPlayerAct.h"
#include "ServerMessageDefine.h"
#include "IGameRoomManager.h"
#include "MJRoomStateWaitReady.h"
#include "MJRoomStateWaitPlayerChu.h"
#include "MJRoomStateWaitPlayerAct.h"
#include "WZMJRoomStateStartGame.h"
#include "MJRoomStateGameEnd.h"
#include "MJRoomStateDoPlayerAct.h"
#include "MJRoomStateAskForPengOrHu.h"
#include "WZMJWaitBankerInviteBuyDi.h"
bool WZMJRoom::init(IGameRoomManager* pRoomMgr, stBaseRoomConfig* pConfig, uint32_t nRoomID, Json::Value& vJsValue)
{
	IMJRoom::init(pRoomMgr,pConfig,nRoomID,vJsValue);
	m_nCaiShenDice = 0;
	m_nCaiShenCard = 0;
	m_nGangedCnt = 0;
	m_nLastHuPlayerIdx = -1;
	m_nLastChuCard = 0;
	m_nLianZhuangCnt = 0;
	m_tPoker.initAllCard(eMJ_WZ);

	// create room state ;
	IMJRoomState* vState[] = {
		new CMJRoomStateWaitReady(), new MJRoomStateWaitPlayerChu(), new MJRoomStateWaitPlayerAct(), new WZMJRoomStateStartGame(), new WZMJWaitBankerInviteBuyDi()
		, new MJRoomStateGameEnd(), new MJRoomStateDoPlayerAct(), new MJRoomStateAskForPengOrHu()
	};
	for (uint8_t nIdx = 0; nIdx < sizeof(vState) / sizeof(IMJRoomState*); ++nIdx)
	{
		addRoomState(vState[nIdx]);
	}
	setInitState(vState[0]);
	return true;
}

void WZMJRoom::willStartGame()
{
	IMJRoom::willStartGame();
	if ((uint8_t)-1 == m_nBankerIdx)
	{
		m_nBankerIdx = 0;
		m_nLianZhuangCnt = 1;
	}
	else if ((uint8_t)-1 == m_nLastHuPlayerIdx || m_nLianZhuangCnt == 5 )
	{
		m_nBankerIdx = (m_nBankerIdx + 1) % 4;
		m_nLianZhuangCnt = 1;
	}
	else
	{
		if (m_nBankerIdx == m_nLastHuPlayerIdx)
		{
			++m_nLianZhuangCnt;
		}
		m_nBankerIdx = m_nLastHuPlayerIdx;

	}

	m_nLastHuPlayerIdx = -1;
	m_nGangedCnt = 0;
	m_nLastChuCard = 0;
}

void WZMJRoom::startGame()
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

	uint8_t nDice = 2 + rand() % 11;
	auto pPoker = getMJPoker();
	LOGFMTD("room id = %u start game shuffle card , Dice = %u", getRoomID(), nDice);
	pPoker->shuffle();
	LOGFMTD("room id = %u shuffle end", getRoomID());
	for (auto& pPlayer : m_vMJPlayers)
	{
		if (!pPlayer)
		{
			LOGFMTE("why player is null hz mj must all player is not null");
			continue;
		}

		if ((uint32_t)pPlayer->getCoin() < getRoomConfig()->nDeskFee)
		{
			LOGFMTE("uid = %u coin = %u, 你的钱太少了，还是别玩了吧，这次就不扣你的台费了", pPlayer->getUID(), pPlayer->getCoin());
		}
		else
		{
			pPlayer->setCoin(pPlayer->getCoin() - (int32_t)getRoomConfig()->nDeskFee);
		}

		LOGFMTD("distribute card for player idx = %u and decrease desk fee = %u", pPlayer->getIdx(), getRoomConfig()->nDeskFee);

		for (uint8_t nIdx = 0; nIdx < 16; ++nIdx)
		{
			//if (pPlayer->getIdx() == 2 && nIdx < 8)
			//{
			//	continue;
			//}

			auto nCard = pPoker->distributeOneCard();
			//auto nCardF = make_Card_Num(eCT_Tiao, 9);
			//auto nCardT = make_Card_Num(eCT_Tong, 7);
			//while ( nCard == nCardF || nCardT == nCard )
			//{
			//	nCard = pPoker->distributeOneCard();
			//}

			pPlayer->getPlayerCard()->addDistributeCard(nCard);

			peerCards[pPlayer->getIdx()][peerCards[pPlayer->getIdx()].size()] = nCard; // sign for msg ;
			//LOGFMTD("card idx = %u card number = %u", nIdx,nCard);
		}

		//if (pPlayer->getIdx() == 2)
		//{
		//	auto nCard = make_Card_Num(eCT_Tiao, 9);
		//	pPlayer->getPlayerCard()->addDistributeCard(nCard);
		//	peerCards[pPlayer->getIdx()][peerCards[pPlayer->getIdx()].size()] = nCard; // sign for msg ;
		//	nCard = make_Card_Num(eCT_Tiao, 9);
		//	pPlayer->getPlayerCard()->addDistributeCard(nCard);
		//	peerCards[pPlayer->getIdx()][peerCards[pPlayer->getIdx()].size()] = nCard; // sign for msg ;
		//	nCard = make_Card_Num(eCT_Tiao, 9);
		//	pPlayer->getPlayerCard()->addDistributeCard(nCard);
		//	peerCards[pPlayer->getIdx()][peerCards[pPlayer->getIdx()].size()] = nCard; // sign for msg ;

		//	nCard = make_Card_Num(eCT_Tiao, 9);
		//	pPlayer->getPlayerCard()->addDistributeCard(nCard);
		//	peerCards[pPlayer->getIdx()][peerCards[pPlayer->getIdx()].size()] = nCard; // sign for msg ;

		//	nCard = make_Card_Num(eCT_Tong, 7);
		//	pPlayer->getPlayerCard()->addDistributeCard(nCard);
		//	peerCards[pPlayer->getIdx()][peerCards[pPlayer->getIdx()].size()] = nCard; // sign for msg ;

		//	nCard = make_Card_Num(eCT_Tong, 7);
		//	pPlayer->getPlayerCard()->addDistributeCard(nCard);
		//	peerCards[pPlayer->getIdx()][peerCards[pPlayer->getIdx()].size()] = nCard; // sign for msg ;
		//	nCard = make_Card_Num(eCT_Tong, 7);
		//	pPlayer->getPlayerCard()->addDistributeCard(nCard);
		//	peerCards[pPlayer->getIdx()][peerCards[pPlayer->getIdx()].size()] = nCard; // sign for msg ;
		//	nCard = make_Card_Num(eCT_Tong, 7);
		//	pPlayer->getPlayerCard()->addDistributeCard(nCard);
		//	peerCards[pPlayer->getIdx()][peerCards[pPlayer->getIdx()].size()] = nCard; // sign for msg ;
		//}

		if (getBankerIdx() == pPlayer->getIdx())
		{
			auto nCard = pPoker->distributeOneCard();
			pPlayer->getPlayerCard()->onMoCard(nCard);
			peerCards[pPlayer->getIdx()][16u] = nCard;  // sign for msg ;
		}
	}

	m_nCaiShenDice = 2 + rand() % 11;
	m_nCaiShenCard = pPoker->distributeOneCard();
	// construct msg ;
	msg["dice"] = nDice;
	msg["banker"] = m_nBankerIdx;
	msg["caishenDice"] = m_nCaiShenDice;
	msg["caiShenCard"] = m_nCaiShenCard;

	Json::Value arrPeerCards;
	for (uint8_t nIdx = 0; nIdx < getSeatCnt(); ++nIdx)
	{
		Json::Value peer;
		peer["cards"] = peerCards[nIdx];
		arrPeerCards[nIdx] = peer;
	}
	msg["peerCards"] = arrPeerCards;
	sendRoomMsg(msg, MSG_ROOM_START_GAME);
	LOGFMTI("room id = %u start game !", getRoomID());

	// set cai shen card to player card 
	for (uint8_t nIdx = 0; nIdx < 4; ++nIdx)
	{
		auto pPlayer = getMJPlayerByIdx(nIdx);
		auto pCard = (WZMJPlayerCard*)pPlayer->getPlayerCard();
		pCard->setCaiShenCard(m_nCaiShenCard);
	}
}

void WZMJRoom::onGameEnd()
{
	if ((uint8_t)-1 != m_nLastHuPlayerIdx)  
	{
		IMJRoom::onGameEnd();
		return;
	}

		// svr : { lianBankerCnt : 2 , results: [ {uid : 2345 , offset : -23, final : 23， caiShenCnt : 2 } , ....  ]   } 
	// no one hu 
	Json::Value jsResult;
	jsResult["lianBankerCnt"] = m_nLianZhuangCnt - 1;
	std::vector<uint8_t> vCaishen3 , vCaiShen2, vCaiShen1 , vCaiShen0;
	for (auto& pp : m_vMJPlayers)
	{
		if (pp == nullptr)
		{
			continue;
		}

		auto pcard = (WZMJPlayerCard*)pp->getPlayerCard();
		switch (pcard->getCaiShenCnt())
		{
		case 3 :
			vCaishen3.push_back(pp->getIdx());
			break;
		case 2 :
			vCaiShen2.push_back(pp->getIdx());
			break;
		case 1:
			vCaiShen1.push_back(pp->getIdx());
			break;
		default:
			vCaiShen0.push_back(pp->getIdx());
			break;
		}
	}

	// do caculate ;
	auto pfuncCal = [this](std::vector<uint8_t>& vWin, std::vector<uint8_t>& vLose, uint8_t nFen)
	{
		for (auto& nWinIdx : vWin)
		{
			auto pWin = getMJPlayerByIdx(nWinIdx);
			if (pWin == nullptr)
			{
				LOGFMTE("room id = %u why win player is nullptr ",getRoomID());
				continue;
			}

			for (auto& nLoseIdx : vLose)
			{
				auto pLoser = getMJPlayerByIdx(nLoseIdx);
				if (pLoser == nullptr)
				{
					LOGFMTE("room id = %u why lose player is nullptr ", getRoomID());
					continue;
				}

				if (nFen > (uint32_t)pLoser->getCoin())
				{
					nFen = pLoser->getCoin();
				}
				pWin->addOffsetCoin(nFen);
				pLoser->addOffsetCoin((int32_t)nFen*-1);
			}
		}
	};

	pfuncCal(vCaishen3,vCaiShen2,1);
	pfuncCal(vCaishen3, vCaiShen1, 2);
	pfuncCal(vCaishen3, vCaiShen0, 3);

	pfuncCal(vCaiShen2, vCaiShen1, 1);
	pfuncCal(vCaiShen2, vCaiShen0, 2);

	pfuncCal(vCaiShen1, vCaiShen0, 1);

	Json::Value jsResultDetail;
	for (auto& pp : m_vMJPlayers)
	{
		if (pp == nullptr)
		{
			continue;
		}

		Json::Value jsP;
		jsP["uid"] = pp->getUID();
		jsP["offset"] = pp->getOffsetCoin();
		jsP["final"] = pp->getCoin();

		auto pcard = (WZMJPlayerCard*)pp->getPlayerCard();
		jsP["caiShenCnt"] = pcard->getCaiShenCnt();
		jsResultDetail[jsResultDetail.size()] = jsP;
		LOGFMTD("room id = %u liu ju uid = %u final = %u ,offset = %d , caishen Cnt = %u ", getRoomID(), pp->getUID(), pp->getCoin(), pp->getOffsetCoin(), pcard->getCaiShenCnt());
	}

	jsResult["results"] = jsResult;
	sendRoomMsg(jsResult, MSG_ROOM_WZMJ_RESULT_LIUJU);
	LOGFMTI("room id = %u send liu ju info ",getRoomID());
	IMJRoom::onGameEnd();
}

uint8_t WZMJRoom::getAutoChuCardWhenWaitActTimeout(uint8_t nIdx)
{
	auto player = getMJPlayerByIdx(nIdx);
	if (!player)
	{
		LOGFMTE("room id = %u why player idx = %u is nullptr ",getRoomID(),nIdx);
		return 0 ;
	}

	auto pCard = (WZMJPlayerCard*)player->getPlayerCard();
	IMJPlayerCard::VEC_CARD vCardMustChu;
	if (pCard->getMustChuFisrtCards(vCardMustChu))
	{
		auto lastChuIter = std::find(vCardMustChu.begin(), vCardMustChu.end(), m_nLastChuCard);
		if (lastChuIter != vCardMustChu.end())
		{
			return m_nLastChuCard;
		}
		return vCardMustChu.front();
	}
	else
	{
		return IMJRoom::getAutoChuCardWhenWaitActTimeout(nIdx);
	}
}

uint8_t WZMJRoom::getAutoChuCardWhenWaitChuTimeout(uint8_t nIdx)
{
	auto player = getMJPlayerByIdx(nIdx);
	if (!player)
	{
		LOGFMTE("room id = %u why player idx = %u is nullptr ", getRoomID(), nIdx);
		return 0;
	}

	auto pCard = (WZMJPlayerCard*)player->getPlayerCard();
	IMJPlayerCard::VEC_CARD vCardMustChu;
	if (pCard->getMustChuFisrtCards(vCardMustChu))
	{
		auto lastChuIter = std::find(vCardMustChu.begin(), vCardMustChu.end(), m_nLastChuCard);
		if (lastChuIter != vCardMustChu.end())
		{
			return m_nLastChuCard;
		}

		return vCardMustChu.front();
	}
	else
	{
		return IMJRoom::getAutoChuCardWhenWaitChuTimeout(nIdx);
	}
}

bool WZMJRoom::isGameOver()
{
	if (!isCanGoOnMoPai())
	{
		return true;
	}

	// is any one hu 
	for (auto& pPlayer : m_vMJPlayers)
	{
		if (pPlayer && pPlayer->haveState(eRoomPeer_AlreadyHu))
		{
			return true;
		}
	}
	return false;
}

bool WZMJRoom::isCanGoOnMoPai()
{
	return getMJPoker()->getLeftCardCount() > 8;
}

bool WZMJRoom::onPlayerApplyLeave(uint32_t nPlayerUID)
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

IMJPlayer* WZMJRoom::doCreateMJPlayer()
{
	return new WZMJPlayer();
}

void WZMJRoom::sendRoomInfo(uint32_t nSessionID)
{
	Json::Value jsMsg;
	jsMsg["roomID"] = getRoomID();
	jsMsg["configID"] = getRoomConfig()->nConfigID;
	jsMsg["roomState"] = getCurRoomState()->getStateID();
	Json::Value arrPlayers;
	for (auto& pPlayer : m_vMJPlayers)
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
		jsPlayer["isTrusteed"] = pPlayer->isTrusteed() ? 1 : 0;
		arrPlayers[pPlayer->getIdx()] = jsPlayer;
	}

	jsMsg["waitTimer"] = 0;
	if (getCurRoomState()->getStateID() == eRoomState_WaitPlayerAct)
	{
		jsMsg["waitTimer"] = getCurRoomState()->getStateDuring();
	}

	jsMsg["players"] = arrPlayers;

	jsMsg["bankerIdx"] = getBankerIdx();
	jsMsg["curActIdex"] = getCurRoomState()->getCurIdx();
	jsMsg["leftCardCnt"] = getMJPoker()->getLeftCardCount();
	jsMsg["caiShenCard"] = m_nCaiShenCard;
	jsMsg["caiShenDice"] = m_nCaiShenDice;
	jsMsg["lasChuedCard"] = m_nLastChuCard;

	sendMsgToPlayer(jsMsg, MSG_ROOM_INFO, nSessionID);
	LOGFMTD("send msg room info msg to player session id = %u", nSessionID);

	if (getCurRoomState()->getStateID() == eRoomSate_WaitReady || eRoomState_GameEnd == getCurRoomState()->getStateID())
	{
		LOGFMTD("current room not start game , so need not send runtime info msg");
		return;
	}
	sendPlayersCardInfo(nSessionID);
}

bool WZMJRoom::canPlayerChuCard(uint8_t nIdx, uint8_t nCheckCard)
{
	if (nCheckCard == m_nCaiShenCard)
	{
		LOGFMTD("room id = %u idx = %u can not chu cai shen ", getRoomID(),nIdx);
		return false;
	}

	auto player = getMJPlayerByIdx(nIdx);
	if (!player)
	{
		LOGFMTE("room id = %u why player idx = %u is nullptr ", getRoomID(), nIdx);
		return false ;
	}

	auto pCard = (WZMJPlayerCard*)player->getPlayerCard();
	IMJPlayerCard::VEC_CARD vCardMustChu;
	if (pCard->getMustChuFisrtCards(vCardMustChu))
	{
		auto iter = std::find(vCardMustChu.begin(),vCardMustChu.end(),nCheckCard);
		if (iter == vCardMustChu.end())
		{
			LOGFMTD("room id = %u player idx = %u , you must chu , must chu first card ", getRoomID(),nIdx );
			return false;
		}

		auto lastChuIter = std::find(vCardMustChu.begin(), vCardMustChu.end(), m_nLastChuCard );
		if (lastChuIter != vCardMustChu.end() && nCheckCard != m_nLastChuCard)
		{
			LOGFMTE("room id = %u player idx = %u , you try best to follow previous player chued jian or feng = %u",getRoomID(),nIdx,m_nLastChuCard );
			return false;
		}
		return true;
	}

	return pCard->isHaveCard(nCheckCard);
}

void WZMJRoom::onPlayerChu(uint8_t nIdx, uint8_t nCard)
{
	IMJRoom::onPlayerChu(nIdx,nCard);
	m_nLastChuCard = nCard;
}

void WZMJRoom::onPlayerHu(std::vector<uint8_t>& vHuIdx, uint8_t nCard, uint8_t nInvokeIdx)
{
	// svr : { winners : [ {idx : 23 , fanxing : 23 }..   ] , invokerIdx : 2, isGangKai : 1 , lianBankerCnt : 2 , results: [ {uid : 2345 , offset : -23, final : 23} , ....  ]   } 
	
	Json::Value jsArrayWiners;
	Json::Value jsResult;
	Json::Value jsResultDetail;
	// do check base bet ;
	auto pConfig = (stMJRoomConfig*)getRoomConfig();
	uint32_t nBasetBet = pConfig->nBaseBet;
	for (uint8_t nLian = 0; nLian < m_nLianZhuangCnt - 1; ++nLian)
	{
		nBasetBet *= 2;
	}

	jsResult["invokerIdx"] = nInvokeIdx;
	jsResult["lianBankerCnt"] = m_nLianZhuangCnt - 1;
	jsResult["isGangKai"] = 0;

	bool bIsSelfMo = vHuIdx.front() == nInvokeIdx;	
	m_nLastHuPlayerIdx = vHuIdx.front();
	if (bIsSelfMo)
	{
		auto pPlayer = (WZMJPlayer*)getMJPlayerByIdx(vHuIdx.front());
		if (!pPlayer)
		{
			LOGFMTE("hu player is null , idx = %u", vHuIdx.front());
			return;
		}

		pPlayer->setState(eRoomPeer_AlreadyHu);
		bool isGangKai = pPlayer->haveGangFalg();
		bool bIsDingDi = pPlayer->isDingDi();
		bool bIsBanker = pPlayer->getIdx() == getBankerIdx();
		uint8_t nFanxing = 0, nBeiShu = 0;
		auto pWZCard = (WZMJPlayerCard*)pPlayer->getPlayerCard();
		auto b = pWZCard->onDoHu(bIsSelfMo, nCard, nFanxing, nBeiShu);
		if ( b == false)
		{
			LOGFMTE("room id = %u player idx = %u , why not hu ?",getRoomID(),pPlayer->getIdx());
			pWZCard->debugCardInfo();
			Assert(0, "should hu");
		}

		Json::Value jsWiner;
		jsWiner["idx"] = pPlayer->getIdx();
		jsWiner["fanxing"] = nFanxing;
		jsArrayWiners[jsArrayWiners.size()] = jsWiner;

		// check gang kai 
		if (isGangKai)
		{
			jsResult["isGangKai"] = 1;
			nBeiShu = 4;
			LOGFMTD("room id = %u idx = %u gang kai , so 4 bei", getRoomID(), pPlayer->getIdx() );
		}

		for (auto& pLoser : m_vMJPlayers)
		{
			if (pLoser == nullptr || pLoser->getIdx() == pPlayer->getIdx())
			{
				continue;
			}

			uint32_t nCurBase = nBasetBet * nBeiShu + pWZCard->getCaiShenCnt();

			bool bIsLoserDingDi = pPlayer->isDingDi();
			bool bIsLoserBanker = pPlayer->getIdx() == getBankerIdx();
			if ((bIsLoserBanker || bIsBanker) && bIsLoserDingDi && bIsDingDi)
			{
				nCurBase = nBasetBet * 2 * nBeiShu  + pWZCard->getCaiShenCnt();;
			}

			if (nCurBase > (uint32_t)pLoser->getCoin())
			{
				nCurBase = pLoser->getCoin();
			}
			pPlayer->addOffsetCoin(nCurBase);
			pLoser->addOffsetCoin((int32_t)nCurBase*-1);
			LOGFMTI("room id = %u uid = %u win , lose uid = %u , offset = %u iloseDing = %u , winDing = %u , bankIdx = %u", getRoomID(), pPlayer->getUID(), pLoser->getUID(), nCurBase, bIsLoserDingDi, bIsDingDi, getBankerIdx());
		}

		// send msg 
		Json::Value msg;
		msg["idx"] = vHuIdx.front();
		msg["actType"] = eMJAct_Hu;
		msg["card"] = nCard;
		sendRoomMsg(msg, MSG_ROOM_ACT);
	}
	else
	{
		auto pLoser = (WZMJPlayer*)getMJPlayerByIdx(nInvokeIdx);
		if (!pLoser)
		{
			LOGFMTE("room id = %u loser player is null , idx = %u", getRoomID(), nInvokeIdx);
			return;
		}

		bool bIsLoserDingDi = pLoser->isDingDi();
		bool bIsLoserBanker = pLoser->getIdx() == getBankerIdx();
		m_nLastHuPlayerIdx = vHuIdx.front();
		for (auto& nWinIdx : vHuIdx)
		{
			auto pPlayer = (WZMJPlayer*)getMJPlayerByIdx(nWinIdx);
			if (!pPlayer)
			{
				LOGFMTE("room id = %u hu player is null , idx = %u", getRoomID(),nWinIdx);
				continue;
			}

			pPlayer->setState(eRoomPeer_AlreadyHu);

			bool bIsDingDi = pPlayer->isDingDi();
			bool bIsBanker = pPlayer->getIdx() == getBankerIdx();
			uint8_t nFanxing = 0, nBeiShu = 0;
			auto pWZCard = (WZMJPlayerCard*)pPlayer->getPlayerCard();
			auto b = pWZCard->onDoHu(bIsSelfMo, nCard, nFanxing, nBeiShu);
			if (b == false)
			{
				LOGFMTE("room id = %u player idx = %u , why not hu ?", getRoomID(), pPlayer->getIdx());
				pWZCard->debugCardInfo();
				Assert(0, "should hu");
			}

			Json::Value jsWiner;
			jsWiner["idx"] = pPlayer->getIdx();
			jsWiner["fanxing"] = nFanxing;
			jsArrayWiners[jsArrayWiners.size()] = jsWiner;

			uint32_t nCurBase = nBasetBet * nBeiShu + pWZCard->getCaiShenCnt();
			if ((bIsLoserBanker || bIsBanker) && bIsLoserDingDi && bIsDingDi)
			{
				nCurBase = nBasetBet * 2 * nBeiShu + pWZCard->getCaiShenCnt();
			}

			if (nCurBase > (uint32_t)pLoser->getCoin())
			{
				nCurBase = pLoser->getCoin();
			}
			pPlayer->addOffsetCoin(nCurBase);
			pLoser->addOffsetCoin((int32_t)nCurBase*-1);

			// send msg 
			Json::Value msg;
			msg["idx"] = nWinIdx;
			msg["actType"] = eMJAct_Hu;
			msg["card"] = nCard;
			sendRoomMsg(msg, MSG_ROOM_ACT);

			LOGFMTI("room id = %u uid = %u win , lose uid = %u , offset = %u iloseDing = %u , winDing = %u , bankIdx = %u", getRoomID(), pPlayer->getUID(), pLoser->getUID(), nCurBase, bIsLoserDingDi, bIsDingDi, getBankerIdx());
		}
	}
	
	auto checkBankHu = std::find(vHuIdx.begin(),vHuIdx.end(),m_nBankerIdx );
	if (checkBankHu != vHuIdx.end())
	{
		m_nLastHuPlayerIdx = m_nBankerIdx;
	}

	jsResult["winners"] = jsArrayWiners;

	for (auto& pp : m_vMJPlayers)
	{
		if (pp == nullptr)
		{
			continue;
		}

		Json::Value jsP;
		jsP["uid"] = pp->getUID();
		jsP["offset"] = pp->getOffsetCoin();
		jsP["final"] = pp->getCoin();
		jsResultDetail[jsResultDetail.size()] = jsP;
	}

	jsResult["results"] = jsResult;
	sendRoomMsg(jsResult, MSG_ROOM_WZMJ_RESULT);
	LOGFMTD("send game result !");
}