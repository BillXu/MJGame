#include "NewMJRoom.h"
#include "WaitStartState.h"
#include "StartGameState.h"
#include "WaitPlayerActState.h"
#include "WaitActWithChuedCard.h"
#include "GameOverState.h"
#include "RoomConfig.h"
#include "NewMJRoomPlayer.h"
#include "LogManager.h"
#include "RobotDispatchStrategy.h"
CTwoBirdFanxingChecker CNewMJRoom::m_tTowBirdFanxingChecker ;
CNewMJRoom::CNewMJRoom()
{

}

bool CNewMJRoom::onFirstBeCreated(IRoomManager* pRoomMgr,stBaseRoomConfig* pConfig, uint32_t nRoomID , Json::Value& vJsValue)
{
	m_pRoomConfig = (stMJRoomConfig*)pConfig ;
	ISitableRoom::onFirstBeCreated(pRoomMgr,pConfig,nRoomID,vJsValue) ;
	m_tPoker.initAllCard(eMJ_TwoBird);
	m_nBankerIdx = 0 ;
	return true ;
}

void CNewMJRoom::serializationFromDB(IRoomManager* pRoomMgr,stBaseRoomConfig* pConfig,uint32_t nRoomID , Json::Value& vJsValue )
{
	m_pRoomConfig = (stMJRoomConfig*)pConfig ;
	ISitableRoom::serializationFromDB(pRoomMgr,pConfig,nRoomID,vJsValue);
	m_tPoker.initAllCard(eMJ_BloodRiver);
	m_nBankerIdx = 0 ;
}

uint32_t CNewMJRoom::getConfigID()
{ 
	return m_pRoomConfig->nConfigID ;
}

void CNewMJRoom::roomInfoVisitor(Json::Value& vOutJsValue)
{
	vOutJsValue["bankIdx"] = m_nBankerIdx;
	vOutJsValue["baseBet"] = getBaseBet();
}

void CNewMJRoom::prepareState()
{
	// create room state ;
	IRoomState* vState[] = {
		new CWaitStartState(),new CStartGameState(),new CWaitPlayerAct(),new CWaitActWithChuedCard()
		,new CGameOverState()
	};
	for ( uint8_t nIdx = 0 ; nIdx < sizeof(vState) / sizeof(IRoomState*); ++nIdx )
	{
		addRoomState(vState[nIdx]) ;
	}
	setInitState(vState[0]);
}

void CNewMJRoom::sendRoomPlayersCardInfo(uint32_t nSessionID)
{
	if ( getCurRoomState()->getStateID() == eRoomSate_WaitReady || eRoomState_GameEnd == getCurRoomState()->getStateID() )
	{
		CLogMgr::SharedLogMgr()->PrintLog("current room not start game , so need not send runtime info msg") ;
		return ;
	}
	Json::Value jsmsg ;
	Json::Value vPeerCards ;
	for ( uint8_t nIdx = 0 ; nIdx < getSeatCount(); ++nIdx )
	{
		auto pp  = (CNewMJRoomPlayer*)getPlayerByIdx(nIdx) ;
		if ( pp == nullptr || pp->isHaveState(eRoomPeer_CanAct) == false )
		{
			continue;
		}
		Json::Value jsCardInfo ;
		pp->getCardInfo(jsCardInfo);
		vPeerCards[vPeerCards.size()] = jsCardInfo ;
	}

	jsmsg["playersCard"] = vPeerCards ;
	jsmsg["bankerIdx"] = getBankerIdx() ;
	jsmsg["curActIdex"] = m_nCurIdx;
	jsmsg["leftCardCnt"] = m_tPoker.getLeftCardCount();
	sendMsgToPlayer(jsmsg,MSG_ROOM_PLAYER_CARD_INFO,nSessionID) ;
}

void CNewMJRoom::sendRoomInfo(uint32_t nSessionID )
{
	Json::Value jsMsg ;
	jsMsg["roomID"] = getRoomID();
	jsMsg["configID"] = getConfigID() ;
	jsMsg["roomState"] = getCurRoomState()->getStateID();
	Json::Value arrPlayers ;
	for ( uint8_t nIdx = 0; nIdx < getSeatCount() ; ++nIdx )
	{
		ISitableRoomPlayer* pPlayer = getPlayerByIdx(nIdx);
		if ( pPlayer == nullptr )
		{
			continue;
		}
		Json::Value jsPlayer ; 
		jsPlayer["idx"] =  nIdx ;
		jsPlayer["uid"] =  pPlayer->getUserUID() ;
		jsPlayer["coin"] = pPlayer->getCoin() ;
		jsPlayer["state"] = pPlayer->getState() ;
		arrPlayers[nIdx] = jsPlayer ;
	}

	jsMsg["players"] = arrPlayers ;
	sendMsgToPlayer(jsMsg,MSG_ROOM_INFO,nSessionID) ;
	CLogMgr::SharedLogMgr()->PrintLog("send msg room info msg to player session id = %u",nSessionID) ;
}

bool CNewMJRoom::canPlayerDirectLeave( uint32_t nUID )
{
	return ( getCurRoomState()->getStateID() == eRoomSate_WaitReady || eRoomState_GameEnd == getCurRoomState()->getStateID() ) ;
}

bool CNewMJRoom::canStartGame()
{
	return getPlayerCntWithState(eRoomPeer_Ready) == getSeatCount() ;
}

uint32_t CNewMJRoom::getBaseBet()
{
	return m_pRoomConfig->nBaseBet ;
}

uint32_t CNewMJRoom::coinNeededToSitDown()
{
	return getBaseBet() * 2 ;
}

ISitableRoomPlayer* CNewMJRoom::doCreateSitableRoomPlayer()
{
	return new CNewMJRoomPlayer();
}

bool CNewMJRoom::onMsg(Json::Value& prealMsg ,uint16_t nMsgType, eMsgPort eSenderPort , uint32_t nSessionID)
{
	switch (nMsgType)
	{
	case MSG_ROOM_REQ_TOTAL_INFO:
		{
			CLogMgr::SharedLogMgr()->PrintLog("reback room state and info msg to session id =%u",nSessionID) ;
			sendRoomInfo(nSessionID) ;
			sendRoomPlayersCardInfo(nSessionID) ;
		}
		break;
	case MSG_PLAYER_LEAVE_ROOM:
		{
			Json::Value jsMsg ;
			stStandPlayer* pp = getPlayerBySessionID(nSessionID) ;
			if ( pp )
			{
				CLogMgr::SharedLogMgr()->PrintLog("player session id = %d apply to leave room ok",nSessionID) ;
				onPlayerApplyLeaveRoom(pp->nUserUID) ;
				jsMsg["ret"] = 0 ;
			}
			else
			{
				jsMsg["ret"] = 1 ;
				CLogMgr::SharedLogMgr()->ErrorLog("session id not in this room how to leave session id = %d",nSessionID) ;
			}
			sendMsgToPlayer(jsMsg,nMsgType,nSessionID);
		}
		break;
	case MSG_PLAYER_SET_READY:
		{
			auto pp = getSitdownPlayerBySessionID(nSessionID) ;
			if ( pp == nullptr )
			{
				CLogMgr::SharedLogMgr()->ErrorLog("you are not sit down , can not ready session id = %u",nSessionID) ;
				return true ;
			}

			if ( pp->isHaveState(eRoomPeer_WaitNextGame) )
			{
				pp->setState(eRoomPeer_Ready) ;
			}
			else
			{
				CLogMgr::SharedLogMgr()->ErrorLog("you are not have wati next game state why you set ready ? idx = %u",pp->getIdx() ) ;
				return true ;
			}

			Json::Value jsMsg ;
			jsMsg["idx"] = pp->getIdx() ;
			sendRoomMsg(jsMsg,MSG_ROOM_PLAYER_READY) ;
			return true ;
		}
		break ;
	default:
		return ISitableRoom::onMsg(prealMsg,nMsgType,eSenderPort,nSessionID) ;
	}

	return true ;
}

void CNewMJRoom::doStartGame()
{
	ISitableRoom::doStartGame();
	m_nBankerIdx = getNextActIdx(m_nBankerIdx) ;
	CLogMgr::SharedLogMgr()->PrintLog("room game begin");
	prepareCards();
}

void CNewMJRoom::prepareCards()
{
	Json::Value msg ;
	Json::Value peerCards[4];

	uint8_t nDice = rand() % getSeatCount() ;
	m_tPoker.shuffle();
//#define __test 
#ifndef __test
	for ( uint8_t nIdx = 0; nIdx < getSeatCount() ; ++nIdx,++nDice )
	{
		uint8_t nRealIdx = nDice % getSeatCount() ;
		auto pPlayer = (CNewMJRoomPlayer*)getPlayerByIdx(nRealIdx);
		CLogMgr::SharedLogMgr()->PrintLog("card player idx = %d",nRealIdx);
		for (uint8_t nCardIdx = 0 ; nCardIdx < 13 ; ++nCardIdx )
		{
			uint8_t nCard = m_tPoker.getCard();
			pPlayer->getMJPeerCard()->addHoldCard(nCard) ;
			peerCards[nRealIdx][(uint32_t)nCardIdx] = nCard ;
			CLogMgr::SharedLogMgr()->PrintLog("card number = %u",nCard);
		}
	}
#else
	uint8_t pCard[2][7] = { {CMJCard::makeCardNumber(eCT_Tiao,1),CMJCard::makeCardNumber(eCT_Tiao,1),CMJCard::makeCardNumber(eCT_Tiao,1),CMJCard::makeCardNumber(eCT_Tiao,1),
							CMJCard::makeCardNumber(eCT_Tiao,2),CMJCard::makeCardNumber(eCT_Tiao,2),CMJCard::makeCardNumber(eCT_Tiao,2)},
	 {CMJCard::makeCardNumber(eCT_Tiao,3),CMJCard::makeCardNumber(eCT_Tiao,3),CMJCard::makeCardNumber(eCT_Tiao,3),CMJCard::makeCardNumber(eCT_Tiao,3),
		CMJCard::makeCardNumber(eCT_Tiao,4),CMJCard::makeCardNumber(eCT_Tiao,4),CMJCard::makeCardNumber(eCT_Tiao,2)} };

	for ( uint8_t nIdx = 0; nIdx < getSeatCount() ; ++nIdx,++nDice )
	{
		uint8_t nRealIdx = nDice % getSeatCount() ;
		auto pPlayer = (CNewMJRoomPlayer*)getPlayerByIdx(nRealIdx);
		CLogMgr::SharedLogMgr()->PrintLog("card player idx = %d",nRealIdx);
		for (uint8_t nCardIdx = 0 ; nCardIdx < 13 ; ++nCardIdx )
		{
			uint8_t nCard = m_tPoker.getCard();
			if ( nIdx < 2 && nCardIdx < 7 )
			{
				nCard = pCard[nIdx][nCardIdx];
			}
			else
			{
				while ( nCard == CMJCard::makeCardNumber(eCT_Tiao,1) || nCard == CMJCard::makeCardNumber(eCT_Tiao,3) || nCard == CMJCard::makeCardNumber(eCT_Tiao,2) )
				{
					nCard = m_tPoker.getCard();
				}
			}
			pPlayer->getMJPeerCard()->addHoldCard(nCard) ;
			peerCards[nRealIdx][(uint32_t)nCardIdx] = nCard ;
			CLogMgr::SharedLogMgr()->PrintLog("card number = %u",nCard);
		}
	}

#endif

	// banker fetch card 
	uint8_t nFetchCard = m_tPoker.getCard() ;
	auto pBankerPlayer = (CNewMJRoomPlayer*)getPlayerByIdx(m_nBankerIdx);
	pBankerPlayer->getMJPeerCard()->onMoCard(nFetchCard);
	peerCards[m_nBankerIdx][(uint32_t)13] = nFetchCard ;

	msg["dice"] = nDice ;
	msg["banker"] = m_nBankerIdx ;
	Json::Value arrPeerCards ;
	for ( uint8_t nIdx = 0 ; nIdx < getSeatCount() ; ++nIdx )
	{
		Json::Value peer ;
		peer["cards"] = peerCards[nIdx] ;
		arrPeerCards[nIdx] = peer ;
	}
	msg["peerCards"] = arrPeerCards ;
	sendRoomMsg(msg,MSG_ROOM_START_GAME) ;
}

void CNewMJRoom::onGameOver()
{
	// send msg 
	Json::Value msg ;
	Json::Value msgArray ;
	for ( uint8_t nIdx = 0 ; nIdx < getSeatCount() ; ++nIdx )
	{
		auto pPlayer = (CNewMJRoomPlayer*)getPlayerByIdx(nIdx) ;
		Json::Value info ;
		info["idx"] = nIdx ;
		info["coin"] = pPlayer->getCoin() ; 
		info["huType"] = eFanxing_PingHu;
		msgArray[(uint32_t)nIdx] = info ;
	}
	msg["players"] = msgArray ;
	sendRoomMsg(msg,MSG_ROOM_GAME_OVER);
	CLogMgr::SharedLogMgr()->PrintLog("send game over msg , %u ", MSG_ROOM_GAME_OVER);
	ISitableRoom::onGameOver() ;
}

void CNewMJRoom::onGameWillBegin()
{
	ISitableRoom::onGameWillBegin();
}

void CNewMJRoom::onGameDidEnd()
{
	std::map<uint32_t,bool> vMapSessionIDIsRobot ;
	for ( uint8_t nidx = 0 ; nidx < getSeatCount() ; ++nidx )
	{
		auto pp = getPlayerByIdx(nidx);
		if ( pp->isDelayStandUp() == false )
		{
			continue;
		}

		auto pst = getPlayerByUserUID(pp->getUserUID()) ;
		vMapSessionIDIsRobot[pst->nUserSessionID] = pst->nPlayerType == ePlayer_Robot ;
	}

	ISitableRoom::onGameDidEnd();
	CLogMgr::SharedLogMgr()->PrintLog("room game End");

	for ( auto& si : vMapSessionIDIsRobot )
	{
		getRobotDispatchStrage()->onPlayerLeave(si.first,si.second) ;
	}
}

uint8_t CNewMJRoom::getBankerIdx()
{
	return m_nBankerIdx ;
}

bool CNewMJRoom::isAnyOneNeedTheCard( uint8_t nCardOwner , uint8_t nCard, eMJActType eCardFrom ,Json::Value& jsArrayNeedIdx )
{
	uint8_t nNextIdx = getNextActIdx(nCardOwner) ;
	for ( uint8_t nIdx = 0 ; nIdx < getSeatCount() ; ++nIdx )
	{
		if ( nIdx == nCardOwner )
		{
			continue;
		}

		auto pp = (CNewMJRoomPlayer*)getPlayerByIdx(nIdx) ;
		if ( !pp || !pp->isHaveState(eRoomPeer_WaitCaculate) )
		{
			continue;
		}

		auto pcard = pp->getMJPeerCard();
		if ( eMJAct_BuGang_Declare !=  eCardFrom )
		{
			 if ( pcard->isCardCanPeng(nCard) )
			 {
				 jsArrayNeedIdx[jsArrayNeedIdx.size()] = nIdx ;
				 continue;
			 }

			 CMJPeerCardNew::VEC_EAT_PAIR v ;
			 if ( nIdx == nNextIdx && pcard->isCardCanEat(nCard,v) )
			 {
				 jsArrayNeedIdx[jsArrayNeedIdx.size()] = nIdx ;
				 continue;
			 }
		}

		// check hu 
		if ( pcard->isCardCanHu(nCard) )
		{
			jsArrayNeedIdx[jsArrayNeedIdx.size()] = nIdx ;
			continue;
		}
	}
	return jsArrayNeedIdx.empty() == false ;
}

uint8_t CNewMJRoom::getAutoChuCardWhenWaitTimeOut(uint8_t nPlayerIdx )
{
	auto pp = (CNewMJRoomPlayer*)getPlayerByIdx(nPlayerIdx) ;
	if ( !pp )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("nplayer idx = %u is null , why need it chu card",nPlayerIdx ) ;
		return 0 ;
	}

	auto nCard = pp->getNewRecievedCard() ;
	if ( pp->getMJPeerCard()->isHoldCardExist(nCard) )
	{
		return nCard ;
	}

	nCard = pp->getMJPeerCard()->getFirstHoldCard(); 
	CLogMgr::SharedLogMgr()->PrintLog("new received card is used, so auto use the first card = %u",nCard) ;
	return nCard ;
}

uint8_t CNewMJRoom::getNextActIdx( uint8_t nCurActIdx )
{
	for ( uint8_t nidx = nCurActIdx + 1 ;  nidx < (nCurActIdx + getSeatCount()) ; ++nidx )
	{
		auto pRealIdx = nidx % getSeatCount() ;
		auto pp = getPlayerByIdx(pRealIdx) ;
		if ( !pp || !pp->isHaveState(eRoomPeer_WaitCaculate) )
		{
			continue;
		}
		return pRealIdx ;
	}

	CLogMgr::SharedLogMgr()->ErrorLog("why we can not find the proper next idx , curidx = %u",nCurActIdx ) ;
	return 0 ;
}

bool CNewMJRoom::isGameOver()
{
	if ( m_tPoker.getLeftCardCount() < 1 )
	{
		return true ;
	}

	for ( uint8_t nIdx = 0 ; nIdx < getSeatCount() ; ++nIdx )
	{
		auto pp = getPlayerByIdx(nIdx) ;
		if ( pp && pp->isHaveState(eRoomPeer_AlreadyHu) )
		{
			return true ;
		}
	}
	return false ;
}

void CNewMJRoom::sendActListToPlayerAboutCard( uint8_t nPlayerIdx , std::list<eMJActType>& vList , uint8_t nCard , uint8_t nInvokeIdx  )
{
	auto pp = getPlayerByIdx(nPlayerIdx) ;
	if ( !pp )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("player idx = %u , not in room , how to send act list",nPlayerIdx ) ;
		return ;
	}

	Json::Value jsmsg ;
	jsmsg["cardNum"] = nCard ;
	jsmsg["invokerIdx"] = nInvokeIdx ;
	Json::Value jsActList ;
	for ( auto& ref : vList )
	{
		jsActList[jsActList.size()] = ref ;
	}

	jsmsg["acts"] = jsActList ;
	sendMsgToPlayer(jsmsg,MSG_PLAYER_WAIT_ACT_ABOUT_OTHER_CARD,pp->getSessionID());
}

void CNewMJRoom::sendPlayerActListOnRecievedCard( uint8_t nCurPlayerIdx )
{
	auto pp = (CNewMJRoomPlayer*)getPlayerByIdx(nCurPlayerIdx) ;
	if ( !pp )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("player idx = %u , not in room , how to send act list, why recieved card ?",nCurPlayerIdx ) ;
		return ;
	}

	auto pmjCard = pp->getMJPeerCard();

	Json::Value jsActList ;
	bool isHaveGangCard = m_tPoker.getLeftCardCount() > 1 ;
	std::vector<uint8_t> vCards ;
	// check an gang 
	if ( isHaveGangCard && pmjCard->isHaveAnGangCards(vCards) )
	{
		Json::Value jsAct;
		jsAct["act"] = eMJAct_AnGang ;
		jsAct["cardNum"] = vCards.front();
		jsActList[jsActList.size()] = jsAct ;
	}
	
	// check bu gang .
	vCards.clear() ;
	if ( isHaveGangCard && pmjCard->isHaveBuGang(vCards) )
	{
		Json::Value jsAct;
		jsAct["act"] = eMJAct_BuGang ;
		jsAct["cardNum"] = vCards.front();
		jsActList[jsActList.size()] = jsAct ;
	}
	
	// check hu ,
	if ( pmjCard->isCardCanHu(0) )
	{
		Json::Value jsAct;
		jsAct["act"] = eMJAct_Hu ;
		jsAct["cardNum"] = pp->getNewRecievedCard();
		jsActList[jsActList.size()] = jsAct ;
	}

	// you always can chu 
	Json::Value jsAct;
	jsAct["act"] = eMJAct_Chu ;
	jsAct["cardNum"] = getAutoChuCardWhenWaitTimeOut(nCurPlayerIdx);
	jsActList[jsActList.size()] = jsAct ;

	// send msg 
	Json::Value jsmsg ;
	jsmsg["acts"] = jsActList ;
	sendMsgToPlayer(jsmsg,MSG_PLAYER_WAIT_ACT_AFTER_RECEIVED_CARD,pp->getSessionID());
}

// mj room function 
bool CNewMJRoom::onPlayerEat( uint8_t nActPlayerIdx  , uint8_t nInvokePlayerIdx ,uint8_t nTargetCard , uint8_t nWithCardA , uint8_t nWithCardB )
{
	auto pp = (CNewMJRoomPlayer*)getPlayerByIdx(nActPlayerIdx) ;
	if ( !pp )
	{
		CLogMgr::SharedLogMgr()->ErrorLog( "player idx = %u is null how to eat ?",nActPlayerIdx );
		return false ;
	}
	CMJPeerCardNew::stEatPair sPair ;
	sPair.nCard[0] = nWithCardA ;
	sPair.nCard[1] = nWithCardB ;
	auto bRet = pp->getMJPeerCard()->onEat(nTargetCard,sPair);

	if ( bRet )
	{
		auto ppInvoker = (CNewMJRoomPlayer*)getPlayerByIdx(nInvokePlayerIdx) ;
		if ( !ppInvoker )
		{
			CLogMgr::SharedLogMgr()->ErrorLog("why Eat invoker is null idx = %u",nInvokePlayerIdx);
		}
		else
		{
			ppInvoker->getMJPeerCard()->onCardBeRobted(nTargetCard) ;
		}

		// send msg ;
		Json::Value jsmsg ;
		jsmsg["idx"] = nActPlayerIdx ;
		jsmsg["actType"] = eMJAct_Chi ;
		jsmsg["card"] = nTargetCard ;
		Json::Value jseatwith ;
		jseatwith[jseatwith.size()] = nWithCardA;
		jseatwith[jseatwith.size()] = nWithCardB;
		jsmsg["eatWith"] = jseatwith ;
		sendRoomMsg(jsmsg,MSG_ROOM_ACT);
		CLogMgr::SharedLogMgr()->PrintLog("do send 吃 invoke idx = %u ",nInvokePlayerIdx);
	}
	else
	{
		CLogMgr::SharedLogMgr()->ErrorLog("do eat act error invoke idx = %u ",nInvokePlayerIdx);
	}
	return bRet ;
}

bool CNewMJRoom::onPlayerPeng( uint8_t nActPlayerIdx , uint8_t nInvokePlayrIdx , uint8_t nTargetCard )
{
	auto pp = (CNewMJRoomPlayer*)getPlayerByIdx(nActPlayerIdx) ;
	if ( !pp )
	{
		CLogMgr::SharedLogMgr()->ErrorLog( "player idx = %u is null how to peng ?",nActPlayerIdx );
		return false ;
	}

	auto bRet = pp->getMJPeerCard()->onPeng(nTargetCard);

	if ( bRet )
	{
		auto ppInvoker = (CNewMJRoomPlayer*)getPlayerByIdx(nInvokePlayrIdx) ;
		if ( !ppInvoker )
		{
			CLogMgr::SharedLogMgr()->ErrorLog("why Peng invoker is null idx = %u",nInvokePlayrIdx);
		}
		else
		{
			ppInvoker->getMJPeerCard()->onCardBeRobted(nTargetCard) ;
		}

		// send msg ;
		Json::Value jsmsg ;
		jsmsg["idx"] = nActPlayerIdx ;
		jsmsg["actType"] = eMJAct_Peng ;
		jsmsg["card"] = nTargetCard ;
		sendRoomMsg(jsmsg,MSG_ROOM_ACT);
	}
	return bRet ;
}

bool CNewMJRoom::onPlayerHu( uint8_t nActPlayerIdx, uint8_t nInvokerPlayerIdx , uint8_t nTargetCard,bool isRbotGang )
{
	bool bIsSelf = nActPlayerIdx == nInvokerPlayerIdx ;
	auto ppHu = (CNewMJRoomPlayer*)getPlayerByIdx(nActPlayerIdx) ;
	if ( !ppHu )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("why hu player is null idx = %u",nActPlayerIdx ) ;
		return false ;
	}

	if ( bIsSelf && !ppHu->getMJPeerCard()->isHoldCardExist(nTargetCard) )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("you hu card = %u ,bu you don't have card, idx = %u ",nTargetCard,nActPlayerIdx ) ;
	}

	if ( bIsSelf && ppHu->getNewRecievedCard() != nTargetCard )
	{
		CLogMgr::SharedLogMgr()->PrintLog("why don't you tell me the new received card to hu = %u, you tell = %u ",ppHu->getNewRecievedCard(),nTargetCard ) ;
	}

	auto ppInvoker = (CNewMJRoomPlayer*)getPlayerByIdx(nInvokerPlayerIdx) ;
	if ( !ppInvoker )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("why hu invoker is null , idx = %u",nInvokerPlayerIdx) ;
		return false ;
	}

	uint8_t nFanType = 0 ; uint8_t nFanShu = 0 ;
	bool bRet = ppHu->getMJPeerCard()->onHu(nTargetCard,bIsSelf);
	if ( !bRet )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("say you hu , but you donot hu, idx = %u,card = %u",nActPlayerIdx,nTargetCard) ;
		return false ;
	}

	if ( !getHuFanxing(ppHu,ppInvoker,nTargetCard,isRbotGang ,nFanType,nFanShu) )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("why check fanxing return error ?") ;
		return false ;
	}

	uint32_t nHuWinCoin = getHuWinCoin(nFanType,nFanShu,bIsSelf) ; 
	// caculate coin ;
	uint32_t nPlayerWin = 0 ;
	if ( bIsSelf )
	{
		for ( uint8_t nIdx = 0 ; nIdx < getSeatCount() ; ++nIdx )
		{
			if ( nIdx == nActPlayerIdx )
			{
				continue;
			}

			auto pp = getPlayerByIdx(nIdx) ;
			if ( !pp || !pp->isHaveState(eRoomPeer_WaitCaculate) )
			{
				continue;
			}

			uint32_t nWinFromPP = nHuWinCoin ;
			if ( pp->getCoin() < nHuWinCoin )
			{
				nWinFromPP = pp->getCoin() ;
			}
			nPlayerWin += nWinFromPP ;
			pp->setCoin( pp->getCoin() - nWinFromPP );
			((CNewMJRoomPlayer*)pp)->addGameOffset(-1 * (int32_t)nWinFromPP ) ;
		}
	}
	else
	{
		ppInvoker->getMJPeerCard()->onCardBeRobted(nTargetCard);
		nPlayerWin = nHuWinCoin ;
		if ( ppInvoker->getCoin() < nHuWinCoin )
		{
			nPlayerWin = ppInvoker->getCoin();
		}
		ppInvoker->setCoin( ppInvoker->getCoin() - nPlayerWin );
		ppInvoker->addGameOffset(-1 * (int32_t)nPlayerWin ) ;
	}

	ppHu->setCoin(ppHu->getCoin() + nPlayerWin) ;
	ppHu->setState(eRoomPeer_AlreadyHu) ;
	CLogMgr::SharedLogMgr()->SystemLog("playerUID = %u fan shu = %u , winCoin = %u",ppHu->getUserUID(),nFanShu,nHuWinCoin);
	// send msg 
	Json::Value msg ;
	msg["idx"] = nActPlayerIdx ;
	msg["actType"] = eMJAct_Hu ;
	msg["card"] = nTargetCard ;
	sendRoomMsg(msg,MSG_ROOM_ACT) ;
	return true ;
}

bool CNewMJRoom::onPlayerMingGang( uint8_t nActPlayerIdx , uint8_t nInvokerIdx , uint8_t nTargetCard )
{
	auto pp = (CNewMJRoomPlayer*)getPlayerByIdx(nActPlayerIdx) ;
	if ( !pp )
	{
		CLogMgr::SharedLogMgr()->ErrorLog( "player idx = %u is null how to MingGang ?",nActPlayerIdx );
		return false ;
	}
	auto pNewCard = m_tPoker.getCard();
	pp->onRecievedCard(eMJAct_MingGang,pNewCard) ;
	auto bRet = pp->getMJPeerCard()->onMingGang(nTargetCard,pNewCard) ;
	if ( bRet )
	{
		auto ppInvoker = (CNewMJRoomPlayer*)getPlayerByIdx(nInvokerIdx) ;
		if ( !ppInvoker )
		{
			CLogMgr::SharedLogMgr()->ErrorLog("why MingGang invoker is null idx = %u",nInvokerIdx);
		}
		else
		{
			ppInvoker->getMJPeerCard()->onCardBeRobted(nTargetCard) ;
		}
	}
	else
	{
		CLogMgr::SharedLogMgr()->ErrorLog("why you can not ming gang ?") ;
		return false ;
	}

	// send msg 
	Json::Value msg ;
	msg["idx"] = nActPlayerIdx ;
	msg["actType"] = eMJAct_MingGang ;
	msg["card"] = nTargetCard ;
	msg["gangCard"] = pNewCard ;
	sendRoomMsg(msg,MSG_ROOM_ACT) ;
	return bRet;
}

bool CNewMJRoom::onPlayerDeclareBuGang(uint8_t nActPlayerIdx , uint8_t nTargetCard)
{
	auto pp = (CNewMJRoomPlayer*)getPlayerByIdx(nActPlayerIdx) ;
	if ( !pp )
	{
		CLogMgr::SharedLogMgr()->ErrorLog( "player idx = %u is null how to DeclareBuGang ?",nActPlayerIdx );
		return false ;
	}

	if ( m_tPoker.getLeftCardCount() < 1 )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("no more card , so can not declare buGang ") ;
		return false ;
	}

	if ( !pp->getMJPeerCard()->isCardCanBuGang(nTargetCard) )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("you can not bu gang with card = %u , idx = %u",nTargetCard,nActPlayerIdx );
		return false ;
	}

	Json::Value msg ;
	msg["idx"] = nActPlayerIdx ;
	msg["actType"] = eMJAct_BuGang_Pre ;
	msg["card"] = nTargetCard ;
	sendRoomMsg(msg,MSG_ROOM_ACT) ;
	return true ;
}

bool CNewMJRoom::onPlayerBuGang(uint8_t nActPlayerIdx , uint8_t nTargetCard )
{
	auto pp = (CNewMJRoomPlayer*)getPlayerByIdx(nActPlayerIdx) ;
	if ( !pp )
	{
		CLogMgr::SharedLogMgr()->ErrorLog( "player idx = %u is null how to BuGang ?",nActPlayerIdx );
		return false ;
	}
	auto pNewCard = m_tPoker.getCard();
	pp->onRecievedCard(eMJAct_BuGang,pNewCard) ;
	auto bRet = pp->getMJPeerCard()->onBuGang(nTargetCard,pNewCard) ;
	// send msg 
	Json::Value msg ;
	msg["idx"] = nActPlayerIdx ;
	msg["actType"] = eMJAct_BuGang_Done ;
	msg["card"] = nTargetCard ;
	msg[ "gangCard" ] = pNewCard ;
	sendRoomMsg(msg,MSG_ROOM_ACT) ;
	return bRet ;
}

bool CNewMJRoom::onPlayerAnGang(uint8_t nActPlayerIdx , uint8_t nTargetCard )
{
	auto pp = (CNewMJRoomPlayer*)getPlayerByIdx(nActPlayerIdx) ;
	if ( !pp )
	{
		CLogMgr::SharedLogMgr()->ErrorLog( "player idx = %u is null how to AnGang ?",nActPlayerIdx );
		return false ;
	}

	if ( !pp->getMJPeerCard()->isCardCanAnGang(nTargetCard) )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("say you can an gang , why can not ? idx = %u",nActPlayerIdx ) ;
		return false ;
	}

	if ( m_tPoker.getLeftCardCount() < 1 )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("now more card , so can not do an gang ") ;
		return false ;
	}
	auto pNewCard = m_tPoker.getCard();
	pp->onRecievedCard(eMJAct_AnGang,pNewCard) ;
	auto nRet = pp->getMJPeerCard()->onAnGang(nTargetCard,pNewCard);
	
	// send msg ;
	Json::Value msg ;
	msg["idx"] = nActPlayerIdx ;
	msg["actType"] = eMJAct_AnGang ;
	msg["card"] = nTargetCard ;
	msg[ "gangCard" ] = pNewCard ;
	sendRoomMsg(msg,MSG_ROOM_ACT) ;
	
	return nRet;
}

bool CNewMJRoom::onPlayerMoPai( uint8_t nActPlayerIdx )
{
	auto pp = (CNewMJRoomPlayer*)getPlayerByIdx(nActPlayerIdx) ;
	if ( !pp )
	{
		CLogMgr::SharedLogMgr()->ErrorLog( "player idx = %u is null how to MoPai ?",nActPlayerIdx );
		return false ;
	}
	auto pNewCard = m_tPoker.getCard();
	pp->onRecievedCard(eMJAct_Mo,pNewCard) ;
	pp->getMJPeerCard()->onMoCard(pNewCard);

	// send msg ;
	Json::Value msg ;
	msg["idx"] = nActPlayerIdx ;
	msg["actType"] = eMJAct_Mo ;
	msg["card"] = pNewCard ;
	sendRoomMsg(msg,MSG_ROOM_ACT) ;
	return true ;
}

bool CNewMJRoom::onPlayerChu(uint8_t nActPlayerIdx , uint8_t nCard )
{
	auto pp = (CNewMJRoomPlayer*)getPlayerByIdx(nActPlayerIdx) ;
	if ( !pp )
	{
		CLogMgr::SharedLogMgr()->ErrorLog( "player idx = %u is null how to Chu ?",nActPlayerIdx );
		return false ;
	}

	auto ret = pp->getMJPeerCard()->onChuCard(nCard);
	// send msg ;
	Json::Value msg ;
	msg["idx"] = nActPlayerIdx ;
	msg["actType"] = eMJAct_Chu ;
	msg["card"] = nCard ;
	sendRoomMsg(msg,MSG_ROOM_ACT) ;
	return ret;
}

void CNewMJRoom::onPlayerDeclareGangBeRobted( uint8_t nPlayerIdx , uint8_t nCard )
{
	auto pp = (CNewMJRoomPlayer*)getPlayerByIdx(nPlayerIdx) ;
	if ( !pp )
	{
		CLogMgr::SharedLogMgr()->ErrorLog( "player idx = %u is null how to DeclareGangBeRobted ?",nPlayerIdx );
		return  ;
	}

	if ( !pp->getMJPeerCard()->isHoldCardExist(nCard) )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("you don not have the card = %u , how to be robot ",nCard ) ;
		return  ;
	}

	pp->getMJPeerCard()->onChuCard(nCard);  // if declare be robot , means , it as chu ;
}

// mj check function 
bool CNewMJRoom::canPlayerPeng(uint8_t nActPlayerIdx , uint8_t nCard )
{
	auto pp = (CNewMJRoomPlayer*)getPlayerByIdx(nActPlayerIdx) ;
	if ( !pp )
	{
		CLogMgr::SharedLogMgr()->ErrorLog( "player idx = %u is null how to check peng ?",nActPlayerIdx );
		return false ;
	}
	return pp->getMJPeerCard()->isCardCanPeng(nCard);
}

bool CNewMJRoom::canPlayerMingGang(uint8_t nActlayerIdx ,uint8_t nCard )
{
	auto pp = (CNewMJRoomPlayer*)getPlayerByIdx(nActlayerIdx) ;
	if ( !pp )
	{
		CLogMgr::SharedLogMgr()->ErrorLog( "player idx = %u is null how to check MingGang ?",nActlayerIdx );
		return false ;
	}

	if ( m_tPoker.getLeftCardCount() < 1 )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("no more card , so can not do gang") ;
		return false ;
	}
	return pp->getMJPeerCard()->isCardCanMingGang(nCard);
}

bool CNewMJRoom::canPlayerHu(uint8_t nActlayerIdx , uint8_t nCard )
{
	auto pp = (CNewMJRoomPlayer*)getPlayerByIdx(nActlayerIdx) ;
	if ( !pp )
	{
		CLogMgr::SharedLogMgr()->ErrorLog( "player idx = %u is null how to check Eat ?",nActlayerIdx );
		return false ;
	}
	return pp->getMJPeerCard()->isCardCanHu(nCard);
}

bool CNewMJRoom::canPlayerEat(uint8_t nActlayerIdx , uint8_t nCard )
{
	auto pp = (CNewMJRoomPlayer*)getPlayerByIdx(nActlayerIdx) ;
	if ( !pp )
	{
		CLogMgr::SharedLogMgr()->ErrorLog( "player idx = %u is null how to check Eat ?",nActlayerIdx );
		return false ;
	}
	CMJPeerCardNew::VEC_EAT_PAIR vPair ;
	return pp->getMJPeerCard()->isCardCanEat(nCard,vPair);
}

bool CNewMJRoom::canPlayerEatWith(uint8_t nActPlayerIdx , uint8_t ACard, uint8_t nWithB )
{
	auto pp = (CNewMJRoomPlayer*)getPlayerByIdx(nActPlayerIdx) ;
	if ( !pp )
	{
		CLogMgr::SharedLogMgr()->ErrorLog( "player idx = %u is null how to check Eat with ?",nActPlayerIdx );
		return false ;
	}
	std::vector<uint8_t> vecHold ;
	pp->getMJPeerCard()->getHoldCard(vecHold);
	auto iter = std::find(vecHold.begin(),vecHold.end(),ACard );
	if ( iter == vecHold.end() )
	{
		return false ;
	}

	iter = std::find(vecHold.begin(),vecHold.end(),nWithB );
	if ( iter == vecHold.end() )
	{
		return false ;
	}

	return true ;
}

uint32_t CNewMJRoom::getHuWinCoin(uint8_t nFanXing,uint16_t nFanshu ,bool isSelfHu )
{
	return nFanshu * getBaseBet() ;
}

bool CNewMJRoom::getHuFanxing(CNewMJRoomPlayer* pActor, CNewMJRoomPlayer* pInvoker, uint8_t nTargetCard, bool isRobtGan , uint8_t& nFanxing, uint8_t& nFanshu )
{
	bool isZiMo = pInvoker == pActor ;
	bool isMiaoShou = false ;
	bool isLoaYue = false ;
	bool isGangshangHua = false ;
	bool isQiangGang = false ;
	bool isJueZhang = false ;
	if ( isZiMo )
	{
		isMiaoShou = m_tPoker.getLeftCardCount() == 0 ;
		auto fRom = pActor->getNewRecievedCardFrom() ;
		isGangshangHua = fRom != eMJAct_Mo ;
	}
	else 
	{
		isLoaYue = m_tPoker.getLeftCardCount() == 0 ;
		isQiangGang = isRobtGan ;
	}

	// check juezhang 
	std::vector<uint8_t> vShowAllCard ;
	for ( uint8_t nIdx = 0 ; nIdx < getSeatCount() ; ++nIdx )
	{
		auto pp = (CNewMJRoomPlayer*)getPlayerByIdx(nIdx) ;
		if ( !pp )
		{
			continue;
		}
		std::vector<uint8_t> vCard ;
		pp->getMJPeerCard()->getShowedCard(vCard);
		vShowAllCard.insert(vShowAllCard.begin(),vCard.begin(),vCard.end());
	}
	uint8_t nFindCnt = std::count(vShowAllCard.begin(),vShowAllCard.end(),nTargetCard) ;
	if ( isZiMo )
	{
		isJueZhang = nFindCnt == 3 ;
	}
	else
	{
		isJueZhang = nFindCnt == 4 ;
	}

	// 天胡
	if ( vShowAllCard.size() == 0 && pActor->getIdx() == getBankerIdx() )
	{
		CLogMgr::SharedLogMgr()->SystemLog("this is tian hu") ;
		nFanxing = 1 ;
		nFanshu = 48 ;
		return true ;
	}

	//地胡
	if ( isZiMo && pActor->getIdx() != getBankerIdx() )
	{
		std::vector<uint8_t> v ;
		pActor->getMJPeerCard()->getShowedCard(v);
		if ( v.empty() && pActor->getMJPeerCard()->isBeRobotEmpty() )
		{
			CLogMgr::SharedLogMgr()->SystemLog("this is di hu") ;
			nFanxing = 1 ;
			nFanshu = 32 ;
			return true ;
		}
	}

	//人胡
	if ( isZiMo == false && pActor->getIdx() != getBankerIdx() && pInvoker->getIdx() == getBankerIdx() )
	{
		std::vector<uint8_t> v ;
		pActor->getMJPeerCard()->getShowedCard(v);
		if ( v.empty() && pActor->getMJPeerCard()->isBeRobotEmpty() )
		{
			CLogMgr::SharedLogMgr()->SystemLog("this is ren hu") ;
			nFanxing = 1 ;
			nFanshu = 24 ;
			return true ;
		}
	}
	//天听
	
	m_tCheckInfo.resetInfo( pActor->getMJPeerCard(),nTargetCard, isMiaoShou,isLoaYue,isGangshangHua,isQiangGang,isZiMo,isJueZhang ) ;
	bool bRet = m_tTowBirdFanxingChecker.check(&m_tCheckInfo,nFanxing,nFanshu) ;
	if ( !bRet )
	{
		return  false ;
	}

	if ( nFanshu < 16 )
	{
		if ( pActor->getIdx() != getBankerIdx() && pActor->isTianTing() )
		{
			nFanshu = 16 ;
			nFanxing = 1 ;
			CLogMgr::SharedLogMgr()->PrintLog("this is tian ting ");
			return true ;
		}
	}
	// check 大众番型（4种）
	// check 其他加成
	return true ;
}