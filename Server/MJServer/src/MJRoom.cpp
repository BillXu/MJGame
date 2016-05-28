#include "MJRoom.h"
#include "MJRoomPlayer.h"
#include "MJServer.h"
#include "MessageIdentifer.h"
#include "ServerMessageDefine.h"
#include "LogManager.h"
#include "AutoBuffer.h"
#include "ServerStringTable.h"
#include <algorithm>
#include <json/json.h>
#include "MJWaitPlayerActState.h"
#include "MJWaitDecideQueState.h"
#include "MJExchangeCardState.h"
#include "MJGameStartState.h"
CMJRoom::CMJRoom()
{
	
}

bool CMJRoom::onFirstBeCreated(IRoomManager* pRoomMgr,stBaseRoomConfig* pConfig, uint32_t nRoomID, Json::Value& vJsValue )
{
	ISitableRoom::onFirstBeCreated(pRoomMgr,pConfig,nRoomID,vJsValue) ;
	m_nBaseBet = ((stMJRoomConfig*)pConfig)->nBaseBet;
	m_tPoker.initAllCard(eMJ_BloodRiver);
	m_nBankerIdx = 0 ;
	return true ;
}

void CMJRoom::prepareState()
{
	ISitableRoom::prepareState();
	// create room state ;
	IRoomState* vState[] = {
		new IRoomStateWaitPlayerReady(),new CMJGameStartState(),new CMJWaitExchangeCardState(),new CMJDoExchangeCardState()
		,new CMJWaitDecideQueState(),new CMJDoDecideQueState(),new CMJWaitPlayerActState(),new CMJDoPlayerActState()
		,new CMJWaitOtherActState(),new CMJDoOtherPlayerActState(), new IRoomStateGameEnd()
	};
	for ( uint8_t nIdx = 0 ; nIdx < sizeof(vState) / sizeof(IRoomState*); ++nIdx )
	{
		addRoomState(vState[nIdx]) ;
	}
	setInitState(vState[0]);
}

void CMJRoom::serializationFromDB(IRoomManager* pRoomMgr,stBaseRoomConfig* pConfig,uint32_t nRoomID , Json::Value& vJsValue )
{
	ISitableRoom::serializationFromDB(pRoomMgr,pConfig,nRoomID,vJsValue);
	m_nBaseBet = ((stMJRoomConfig*)pConfig)->nBaseBet;
	m_tPoker.initAllCard(eMJ_BloodRiver);
	m_nBankerIdx = 0 ;
}

void CMJRoom::willSerializtionToDB(Json::Value& vOutJsValue)
{
	ISitableRoom::willSerializtionToDB(vOutJsValue);
}

void CMJRoom::roomItemDetailVisitor(Json::Value& vOutJsValue)
{
	ISitableRoom::roomItemDetailVisitor(vOutJsValue) ;
	//vOutJsValue["baseBet"] = getBaseBet() ;
}

ISitableRoomPlayer* CMJRoom::doCreateSitableRoomPlayer()
{
	return new CMJRoomPlayer();
}

void CMJRoom::onPlayerWillStandUp( ISitableRoomPlayer* pPlayer )
{
	// always can stand up now ;
	if ( 0 && pPlayer->isHaveState(eRoomPeer_CanAct) )
	{
		 ISitableRoom::onPlayerWillStandUp(pPlayer) ;
	}
	else
	{
		playerDoStandUp(pPlayer);
	}
}

uint32_t CMJRoom::getLeastCoinNeedForCurrentGameRound(ISitableRoomPlayer* pp)
{
	CMJRoomPlayer* pPlayer = (CMJRoomPlayer*)pp ;
	return pPlayer->getCoin() ;
}

void CMJRoom::roomInfoVisitor(Json::Value& vOutJsValue)
{
	vOutJsValue["bankIdx"] = m_nBankerIdx;
	vOutJsValue["baseBet"] = getBaseBet();
}

void CMJRoom::sendRoomPlayersCardInfo(uint32_t nSessionID)
{
	if ( getCurRoomState()->getStateID() == eRoomSate_WaitReady )
	{
		CLogMgr::SharedLogMgr()->PrintLog("current room not start game , so need not send runtime info msg") ;
		return ;
	}
	Json::Value jsmsg ;
	Json::Value vPeerCards ;
	for ( uint8_t nIdx = 0 ; nIdx < getSeatCount(); ++nIdx )
	{
		auto pp  = (CMJRoomPlayer*)getPlayerByIdx(nIdx) ;
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
	sendMsgToPlayer(jsmsg,MSG_ROOM_PLAYER_CARD_INFO,nSessionID) ;
}


uint32_t CMJRoom::getBaseBet()
{
	return m_nBaseBet ;
}

void CMJRoom::onGameWillBegin()
{
	ISitableRoom::onGameWillBegin();
	++m_nBankerIdx ;
	m_nBankerIdx = m_nBankerIdx % getSeatCount() ;
	CLogMgr::SharedLogMgr()->PrintLog("room game begin");
	m_tPoker.shuffle();
	prepareCards();
}

void CMJRoom::onGameDidEnd()
{
	caculateGameResult();
	ISitableRoom::onGameDidEnd();
	CLogMgr::SharedLogMgr()->PrintLog("room game End");
}

void CMJRoom::prepareCards()
{
	Json::Value msg ;
	Json::Value peerCards[4];

	uint8_t nDice = rand() % getSeatCount() ;
	for ( uint8_t nIdx = 0; nIdx < getSeatCount() ; ++nIdx,++nDice )
	{
		uint8_t nRealIdx = nDice % getSeatCount() ;
		auto pPlayer = (CMJRoomPlayer*)getPlayerByIdx(nRealIdx);
		CLogMgr::SharedLogMgr()->PrintLog("card player idx = %d",nRealIdx);
		for (uint8_t nCardIdx = 0 ; nCardIdx < 13 ; ++nCardIdx )
		{
			uint8_t nCard = m_tPoker.getCard();
			pPlayer->addDistributeCard(nCard) ;
			peerCards[nRealIdx][(uint32_t)nCardIdx] = nCard ;
			CLogMgr::SharedLogMgr()->PrintLog("card number = %u",nCard);
		}
	}

	// banker fetch card 
	uint8_t nFetchCard = m_tPoker.getCard() ;
	auto pBankerPlayer = (CMJRoomPlayer*)getPlayerByIdx(m_nBankerIdx);
	pBankerPlayer->fetchCard(nFetchCard) ;
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

uint32_t CMJRoom::coinNeededToSitDown()
{
	return getBaseBet()* 1 ;
}

void CMJRoom::caculateGameResult()
{
	std::vector<CMJRoomPlayer*> vecHuaZhu ;
	std::vector<CMJRoomPlayer*> vecNotHuaZhu ;
	std::vector<CMJRoomPlayer*> vecNotTingPai ;
	std::vector<CMJRoomPlayer*> vecTingPai ;
	for ( uint8_t nIdx = 0 ; nIdx < getSeatCount() ; ++nIdx )
	{
		auto pPlayer = (CMJRoomPlayer*)getPlayerByIdx(nIdx) ;
		if ( pPlayer->isHuaZhu() )
		{
			vecHuaZhu.push_back(pPlayer) ;
			continue;
		}

		vecNotHuaZhu.push_back(pPlayer);

		if ( pPlayer->isHaveState(eRoomPeer_CanAct) )
		{
			if ( pPlayer->isTingPai() )
			{
				vecTingPai.push_back(pPlayer);
			}
			else
			{
				vecNotTingPai.push_back(pPlayer) ;
			}
		}
	}

	// process hua zhu ;
	uint8_t nHuaZhuCoin = getBaseBet() ;
	for ( auto pHuaZhu : vecHuaZhu )
	{
		for ( auto pNotHuaZhu : vecNotHuaZhu )
		{
			uint8_t nAddcoin = nHuaZhuCoin ;
			if ( pHuaZhu->getCoin() < nHuaZhuCoin )
			{
				nAddcoin = pHuaZhu->getCoin();
			}

			pNotHuaZhu->setCoin(pNotHuaZhu->getCoin() + nAddcoin );
			pHuaZhu->setCoin(pHuaZhu->getCoin() - nAddcoin );
			if ( pHuaZhu->getCoin() <= 0 )
			{
				break; 
			}
		}
	}

	// process da jiao 
	for ( auto pNotTing : vecNotTingPai )
	{
		onPlayerRallBackWindRain( pNotTing );
		for ( auto pTing : vecTingPai )
		{
			if ( pNotTing->getCoin() <= 0 )
			{
				break; 
			}

			uint8_t nGenshu = 0 ;
			uint8_t nFanShu = pTing->getMaxCanHuFanShu(nGenshu) ;
			uint8_t nAddcoin = getCacualteCoin(nFanShu,nGenshu) ;
			if ( pNotTing->getCoin() < nAddcoin )
			{
				nAddcoin = pNotTing->getCoin();
			}

			pTing->setCoin(pTing->getCoin() + nAddcoin );
			pNotTing->setCoin(pNotTing->getCoin() - nAddcoin );
		}
	}

	// send msg 
	Json::Value msg ;
	Json::Value msgArray ;
	for ( uint8_t nIdx = 0 ; nIdx < getSeatCount() ; ++nIdx )
	{
		auto pPlayer = getPlayerByIdx(nIdx) ;
		Json::Value info ;
		info["idx"] = nIdx ;
		info["coin"] = pPlayer->getCoin() ; 
		msgArray[(uint32_t)nIdx] = info ;
	}
	msg["players"] = msgArray ;
	sendRoomMsg(msg,MSG_ROOM_GAME_OVER);
}

void CMJRoom::onPlayerHuPai( uint8_t nActIdx )
{
	auto pPlayerWiner = (CMJRoomPlayer*)getPlayerByIdx(nActIdx) ;

	uint8_t nGenshu = 0 ;
	uint8_t nFanShu = pPlayerWiner->doHuPaiFanshu(0,nGenshu);

	
	if ( pPlayerWiner->getNewFetchedFrom() != eMJAct_Mo )
	{
		// gang shang hua 
		nFanShu += 1 ;
	}
	else
	{
		 // zi mo 
		 CLogMgr::SharedLogMgr()->ErrorLog("zi mo xu yao jia fan ma ?");
	}

	uint32_t nWinCoin = getCacualteCoin(nFanShu,nGenshu) ;
	
	uint32_t ntotalWin = 0 ;
	// kou qian ;
	for ( uint8_t nIdx = 0 ; nIdx < getSeatCount() ; ++nIdx )
	{
		if ( nIdx == nActIdx )
		{
			continue; 
		}

		auto pp = getPlayerByIdx(nIdx) ;
		if ( nWinCoin <= pp->getCoin() )
		{
			ntotalWin += nWinCoin ;
			pp->setCoin(pp->getCoin() - nWinCoin) ;
			continue;
		}

		ntotalWin += pp->getCoin() ;
		pp->setCoin(0) ;
	}

	// add coin 
	pPlayerWiner->setCoin(pPlayerWiner->getCoin() + ntotalWin ) ;
	// send msg 
	Json::Value msg ;
	msg["idx"] = pPlayerWiner->getIdx() ;
	msg["actType"] = eMJAct_Hu ;
	msg["card"] = pPlayerWiner->getNewFetchCard() ;
	sendRoomMsg(msg,MSG_ROOM_ACT) ;
}

void CMJRoom::onPlayerHuPai(uint8_t nActIdx , uint8_t nCardNumber, uint8_t nInvokerIdx ,bool isGangPai )
{
	auto pPlayerWiner = (CMJRoomPlayer*)getPlayerByIdx(nActIdx) ;
	auto pLosePlayer = (CMJRoomPlayer*)getPlayerByIdx(nInvokerIdx) ;
	
	uint8_t nGenshu = 0 ;
	uint8_t nFanShu = pPlayerWiner->doHuPaiFanshu(nCardNumber,nGenshu);

	if ( isGangPai )  // qiang gang hu 
	{
		CLogMgr::SharedLogMgr()->PrintLog("uid = %u qiang gang hu jia 1 fan , invoker = %u",pPlayerWiner->getUserUID() ,pLosePlayer->getUserUID() ) ;
		nFanShu += 1 ;
		pLosePlayer->beRobotGang(nCardNumber) ;
	}
	else if ( pLosePlayer->getNewFetchedFrom() != eMJAct_Mo )  // gang shang pao ;
	{
		CLogMgr::SharedLogMgr()->ErrorLog("process rollback wind and rain , gang shang Pao") ;
		nFanShu += 1 ;
		CLogMgr::SharedLogMgr()->PrintLog("uid = %u Gang shang pao jia 1 fan , invoker = %u",pPlayerWiner->getUserUID() ,pLosePlayer->getUserUID() ) ;
	}
	
	uint32_t nWinCoin = getCacualteCoin(nFanShu,nGenshu) ;

	uint32_t ntotalWin = 0 ;
	if ( pLosePlayer->getCoin() >= nWinCoin )
	{
		ntotalWin = nWinCoin ;
		pLosePlayer->setCoin(pLosePlayer->getCoin() - nWinCoin ) ;
	}
	else 
	{
		ntotalWin = pLosePlayer->getCoin();
		pLosePlayer->setCoin(0) ;
	}
	pPlayerWiner->setCoin(pPlayerWiner->getCoin() + ntotalWin ) ;
	if ( !isGangPai )
	{
		pLosePlayer->onChuedPaiBePengOrGang(nCardNumber);
	}
	
	// send msg 
	Json::Value msg ;
	msg["idx"] = pPlayerWiner->getIdx() ;
	msg["actType"] = eMJAct_Hu ;
	msg["card"] = nCardNumber ;
	sendRoomMsg(msg,MSG_ROOM_ACT) ;
}

void CMJRoom::onPlayerGangPai( uint8_t nActIdx ,uint8_t nCardNumber, bool isBuGang, uint8_t nInvokeIdx )
{
	auto pPlayer = (CMJRoomPlayer*)getPlayerByIdx(nActIdx) ;
	auto pLosePlayer = (CMJRoomPlayer*)getPlayerByIdx(nInvokeIdx) ;

	eMJActType eGangType = eMJAct_AnGang ;
	if ( isBuGang )
	{
		//pPlayer->addDistributeCard(nCardNumber) ;  // specail situation ;  // give back the card 
		eGangType = eMJAct_BuGang ;
	}

	if ( nInvokeIdx != nActIdx )
	{
		eGangType = eMJAct_MingGang ;
		pLosePlayer->onChuedPaiBePengOrGang(nCardNumber);
	}
	
	uint8_t nGangPai = m_tPoker.getCard() ;
	pPlayer->gangPai(nCardNumber,eGangType,nGangPai) ;

	// caculate wind and rain 
	uint8_t nWinCoin = getBaseBet() ;
	uint8_t ntotalWin  = 0 ;
	stBillWin* pBill = new stBillWin ;
	pBill->eType = stBillWin::eBill_GangWin ;
	if ( nInvokeIdx == nActIdx )
	{
		for ( uint8_t nIdx = 0 ; nIdx < getSeatCount() ; ++nIdx )
		{
			if ( nIdx == nActIdx )
			{
				continue; 
			}

			stBillLose* pLoseBill = new stBillLose ;
			pLoseBill->eType = stBill::eBill_GangLose ;
			pLoseBill->nWinnerIdx = nActIdx ;
			auto pp = getPlayerByIdx(nIdx) ;
			if ( nWinCoin <= pp->getCoin() )
			{
				ntotalWin += nWinCoin ;
				pp->setCoin(pp->getCoin() - nWinCoin) ;
				pBill->vLoseIdxAndCoin[pp->getIdx()] = nWinCoin ;

				pLoseBill->nOffset = nWinCoin ;
				
				((CMJRoomPlayer*)pp)->addBill(pLoseBill) ;
				continue;
			}

			ntotalWin += pp->getCoin() ;
			pBill->vLoseIdxAndCoin[pp->getIdx()] = pp->getCoin() ;
			pLoseBill->nOffset = pp->getCoin() ;
			pp->setCoin(0) ;
			((CMJRoomPlayer*)pp)->addBill(pLoseBill) ;
		}

		pBill->nOffset = ntotalWin ;
		pPlayer->setCoin(pPlayer->getCoin() + ntotalWin ) ;
		pPlayer->addBill(pBill);
	}
	else
	{
		uint32_t ntotalWin = nWinCoin ;

		stBillLose* pLoseBill = new stBillLose ;
		pLoseBill->eType = stBill::eBill_GangLose ;
		pLoseBill->nWinnerIdx = nActIdx ;

		if ( pLosePlayer->getCoin() >= nWinCoin )
		{
			ntotalWin = nWinCoin ;
			pLosePlayer->setCoin(pLosePlayer->getCoin() - nWinCoin ) ;
		}
		else 
		{
			ntotalWin = pLosePlayer->getCoin();
			pLosePlayer->setCoin(0) ;
		}

		pLoseBill->nOffset = ntotalWin ;
		pLosePlayer->addBill(pLoseBill);

		pPlayer->setCoin(pPlayer->getCoin() + ntotalWin ) ;

		pBill->vLoseIdxAndCoin[pLosePlayer->getIdx()] = ntotalWin ;
		pBill->nOffset = ntotalWin ;
		pPlayer->addBill(pBill);
	}

	// send msg 
	Json::Value msg ;
	msg["idx"] = pPlayer->getIdx() ;
	if ( isBuGang )
	{
		msg["actType"] = eMJAct_BuGang_Done ;
	}
	else if ( nInvokeIdx == nActIdx )
	{
		msg["actType"] = eMJAct_AnGang ;
	}
	else
	{
		msg["actType"] = eMJAct_MingGang ;
	}

	msg["card"] = nCardNumber ;
	msg[ "gangCard" ] = nGangPai ;
	sendRoomMsg(msg,MSG_ROOM_ACT) ;
}

bool CMJRoom::checkPlayersNeedTheCard( uint8_t nCardNumber ,std::vector<uint8_t>& nNeedCardPlayerIdxs, uint8_t nExptPlayerIdx )
{
	for ( uint8_t nIdx = 0 ; nIdx < getSeatCount() ; ++nIdx )
	{
		if ( nIdx == nExptPlayerIdx )
		{
			continue; 
		}

		auto pPlayer = (CMJRoomPlayer*)getPlayerByIdx(nIdx) ;
		if ( pPlayer->isCardBeWanted(nCardNumber,false) )
		{
			nNeedCardPlayerIdxs.push_back(nIdx) ;
			CLogMgr::SharedLogMgr()->PrintLog("player idx = %u , need the card : %u",nIdx,nCardNumber) ;
		}
	}

	return nNeedCardPlayerIdxs.empty() == false ;
}

void CMJRoom::onPlayerBuGangPre(uint8_t nPlayerIdx , uint8_t nCardNumber )
{
	auto pPlayer = (CMJRoomPlayer*)getPlayerByIdx(nPlayerIdx) ;
	pPlayer->declareBuGang(nCardNumber) ;

	Json::Value msg ;
	msg["idx"] = pPlayer->getIdx() ;
	msg["actType"] = eMJAct_BuGang_Pre ;
	msg["card"] = nCardNumber ;
	sendRoomMsg(msg,MSG_ROOM_ACT) ;
}

void CMJRoom::onPlayerChuPai(uint8_t nPlayerIdx , uint8_t nCardNumber )
{
	auto pPlayer = (CMJRoomPlayer*)getPlayerByIdx(nPlayerIdx) ;
	pPlayer->onChuCard(nCardNumber);

	Json::Value msg ;
	msg["idx"] = pPlayer->getIdx() ;
	msg["actType"] = eMJAct_Chu ;
	msg["card"] = nCardNumber ;
	sendRoomMsg(msg,MSG_ROOM_ACT) ;
}

uint8_t CMJRoom::getLeftCardCnt()
{
	return m_tPoker.getLeftCardCount();
}

uint8_t CMJRoom::getNextActPlayerIdx( uint8_t nCurActIdx )
{
	++nCurActIdx ;
	nCurActIdx %= getSeatCount() ;
	return nCurActIdx ;
}

void CMJRoom::onPlayerMoPai( uint8_t nIdx )
{
	auto pPlayer = (CMJRoomPlayer*)getPlayerByIdx(nIdx) ;
	uint8_t nCard = m_tPoker.getCard() ;
	pPlayer->fetchCard(nCard);

	Json::Value msg ;
	msg["idx"] = pPlayer->getIdx() ;
	msg["actType"] = eMJAct_Mo ;
	msg["card"] = nCard ;
	sendRoomMsg(msg,MSG_ROOM_ACT) ;
}

void CMJRoom::onPlayerPeng(uint8_t nPlayerIdx ,uint8_t nCardNumber, uint8_t nInvokerIdx )
{
	auto pPlayer = (CMJRoomPlayer*)getPlayerByIdx(nPlayerIdx) ;
	pPlayer->onPengCard(nCardNumber) ;

	auto pPlayerInvoker = (CMJRoomPlayer*)getPlayerByIdx(nInvokerIdx) ;
	pPlayerInvoker->onChuedPaiBePengOrGang(nCardNumber) ;

	Json::Value msg ;
	msg["idx"] = pPlayer->getIdx() ;
	msg["actType"] = eMJAct_Peng ;
	msg["card"] = nCardNumber ;
	sendRoomMsg(msg,MSG_ROOM_ACT) ;
}

bool CMJRoom::canPlayerGangWithCard(uint8_t nPlayerIdx , uint8_t nCardNumber, bool bCardFromSelf )
{
	auto pPlayer = (CMJRoomPlayer*)getPlayerByIdx(nPlayerIdx) ;
	return pPlayer->canGangWithCard(nCardNumber,bCardFromSelf) ;
}

bool CMJRoom::canPlayerHuPai( uint8_t nPlayerIdx , uint8_t nCardNumber )
{
	auto pPlayer = (CMJRoomPlayer*)getPlayerByIdx(nPlayerIdx) ;
	return pPlayer->canHuPai(nCardNumber) ;
}

bool CMJRoom::canPlayerPengPai(uint8_t nPlayerIdx , uint8_t nCardNumber)
{
	auto pPlayer = (CMJRoomPlayer*)getPlayerByIdx(nPlayerIdx) ;
	return pPlayer->canPengWithCard(nCardNumber) ;
}

uint8_t CMJRoom::getPlayerAutoChuCardWhenTimeOut(uint8_t nPlayerIdx)
{
	auto pPlayer = (CMJRoomPlayer*)getPlayerByIdx(nPlayerIdx) ;
	uint8_t nCard = pPlayer->getNewFetchCard();
	if ( nCard == 0 )
	{
		nCard = pPlayer->getCardByIdx(0) ;
		CLogMgr::SharedLogMgr()->ErrorLog("why uid = %u not fetch new card but need to chu pai ",pPlayer->getUserUID()) ;
	}
	return nCard ;
}

void CMJRoom::onPlayerRallBackWindRain(CMJRoomPlayer* pPlayer )
{
	std::vector<stBill*> vecGangWin ;
	pPlayer->getGangWinBill(vecGangWin) ;
	if ( vecGangWin.empty() )
	{
		return ;
	}

	for ( auto ref : vecGangWin )
	{
		stBillWin* pWinBill = (stBillWin*)ref ;
		for ( auto tPlayer : pWinBill->vLoseIdxAndCoin )
		{
			if ( pPlayer->getCoin() <= 0 )
			{
				return ;
			}

			uint8_t nBackCoin = tPlayer.second ;
			if ( pPlayer->getCoin() < tPlayer.second )
			{
				nBackCoin = pPlayer->getCoin() ;
			}

			auto pPlayerBack = getPlayerByIdx(tPlayer.first) ;
			pPlayerBack->setCoin(pPlayerBack->getCoin() + nBackCoin ) ;

			pPlayer->setCoin(pPlayer->getCoin() - nBackCoin ) ;
		}
		
	}
}

uint32_t CMJRoom::getCacualteCoin( uint8_t nFanshu , uint8_t nGenShu )
{
	return getBaseBet() * nFanshu + nGenShu * getBaseBet() ;
}

bool CMJRoom::onInformActAboutCard( uint8_t nPlayerIdx , uint8_t nCardNum, uint8_t cardProviderIdx )
{
	assert(nPlayerIdx != cardProviderIdx && "self can not need self card in this situation" );

	auto pp = getPlayerByIdx(nPlayerIdx) ;
	Json::Value jsmsg ;
	jsmsg["cardNum"] = nCardNum ;

	Json::Value actArray ;
	if ( canPlayerHuPai(nPlayerIdx,nCardNum) )
	{
		actArray[actArray.size()] = eMJAct_Hu ;
		CLogMgr::SharedLogMgr()->PrintLog("uid = %u ,need card for act = %u",pp->getUserUID(),eMJAct_Hu) ;
	}

	if ( canPlayerPengPai(nPlayerIdx,nCardNum) )
	{
		actArray[actArray.size()] = eMJAct_Peng ;
		CLogMgr::SharedLogMgr()->PrintLog("uid = %u ,need card for act = %u",pp->getUserUID(),eMJAct_Peng) ;
	}

	if ( getLeftCardCnt() > 1 && canPlayerGangWithCard(nPlayerIdx,nCardNum,false) )
	{
		actArray[actArray.size()] = eMJAct_MingGang ;
		CLogMgr::SharedLogMgr()->PrintLog("uid = %u ,need card for act = %u",pp->getUserUID(),eMJAct_MingGang) ;
	}

	if ( actArray.empty() )
	{
		CLogMgr::SharedLogMgr()->PrintLog("this situation no need send act inform ") ;
		return false;
	}
	actArray[actArray.size()] = eMJAct_Pass ;
	jsmsg["acts"] = actArray ;


	sendMsgToPlayer(jsmsg,MSG_PLAYER_WAIT_ACT_ABOUT_OTHER_CARD,pp->getSessionID()) ;
	return true ;
}

bool CMJRoom::onInformSelfCanActWithCard( uint8_t nPlayerIdx )
{
	auto pp = (CMJRoomPlayer*)getPlayerByIdx(nPlayerIdx) ;
	Json::Value jsActArray ;
	if ( !pp->getOperateListJoson(jsActArray) )
	{
		return false ;
	}

	Json::Value passAct ;
	passAct["act"] = eMJAct_Pass ;
	passAct["cardNum"] = pp->getNewFetchCard();
	jsActArray[jsActArray.size()] = passAct ;

	Json::Value jsmsg ;
	jsmsg["acts"] = jsActArray ;
	sendMsgToPlayer(jsmsg,MSG_PLAYER_WAIT_ACT_AFTER_RECEIVED_CARD,pp->getSessionID()) ;
	return true ;
}

