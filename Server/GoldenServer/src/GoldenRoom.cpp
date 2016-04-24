#include "GoldenRoom.h"
#include "RoomConfig.h"
#include "GoldenRoomPlayer.h"

#include <json/json.h>
#include "GoldenMessageDefine.h"
#include "AutoBuffer.h"
#include "LogManager.h"
#include "GoldenBetState.h"
#include "GoldenGameResultState.h"
#include "GoldenPKState.h"
#include "GoldenStartGameState.h"
#include "GoldenWaitReadyState.h"
#include <cassert>
CGoldenRoom::CGoldenRoom()
{
	m_nBankerIdx = 0;
	m_nCurBet = 0;
	m_nBaseBet = 0;
	m_nMailPool = 0;
	m_nCurActIdx = m_nBankerIdx ;
	m_nBetRound = 0 ;
	getPoker()->InitTaxasPoker() ;
}

bool CGoldenRoom::onFirstBeCreated(IRoomManager* pRoomMgr,stBaseRoomConfig* pConfig, uint32_t nRoomID , Json::Value& vJsValue)
{
	ISitableRoom::onFirstBeCreated(pRoomMgr,pConfig,nRoomID,vJsValue) ;
	m_nBaseBet = ((stNiuNiuRoomConfig*)pConfig)->nBaseBet;

	return true ;
}

void CGoldenRoom::serializationFromDB(IRoomManager* pRoomMgr,stBaseRoomConfig* pConfig,uint32_t nRoomID , Json::Value& vJsValue )
{
	ISitableRoom::serializationFromDB(pRoomMgr,pConfig,nRoomID,vJsValue) ;
	m_nBaseBet = ((stNiuNiuRoomConfig*)pConfig)->nBaseBet;
}

void CGoldenRoom::prepareState()
{
	IRoomState* vState[] = {
		new IRoomStateWaitReadyState(),new IRoomStateClosed(),new IRoomStateDidGameOver()
		,new CGoldenStartGameState(), new CGoldenPKState(), new CGoldenGameResultState(),new CGoldenBetState()
	};

	for ( uint8_t nIdx = 0 ; nIdx < sizeof(vState) / sizeof(IRoomState*); ++nIdx )
	{
		addRoomState(vState[nIdx]) ;
	}
	setInitState(vState[0]);
}

bool CGoldenRoom::onMessage( stMsg* prealMsg , eMsgPort eSenderPort , uint32_t nPlayerSessionID )
{
	if ( ISitableRoom::onMessage(prealMsg,eSenderPort,nPlayerSessionID) )
	{
		return true ;
	}
	
	switch ( prealMsg->usMsgType )
	{
	case MSG_GOLDEN_PLAYER_ACT:
		{
			stMsgGoldenPlayerActRet msgBack ;
			stMsgGoldenPlayerAct* ppmsg = (stMsgGoldenPlayerAct*)prealMsg ;
			if ( ppmsg->nPlayerAct == eRoomPeerAction_Ready )
			{
				auto pp = getSitdownPlayerBySessionID(nPlayerSessionID) ;
				if ( pp )
				{
					if ( pp->isHaveState(eRoomPeer_CanAct) == false )
					{
						pp->setState(eRoomPeer_Ready) ;
						stMsgGoldenRoomAct msgR ;
						msgR.nPlayerAct = ppmsg->nPlayerAct ;
						msgR.nValue = 0 ;
						msgR.nPlayerIdx = pp->getIdx() ;
						sendRoomMsg(&msgR,sizeof(msgR)) ;
					}
					else
					{
						CLogMgr::SharedLogMgr()->ErrorLog("you are playing game , why set ready sate ?");
						msgBack.nRet = 3 ;
						sendMsgToPlayer(&msgBack,sizeof(msgBack),nPlayerSessionID) ;
					}
				}
				else
				{
					msgBack.nRet = 3 ;
					CLogMgr::SharedLogMgr()->ErrorLog("you are not sit down can not ready session id = %u",nPlayerSessionID);
					sendMsgToPlayer(&msgBack,sizeof(msgBack),nPlayerSessionID) ;

				}
				return true ;
			}
			return false ;
		}
		break;
	default:
		break;
	}
	return false ;
}

void CGoldenRoom::roomInfoVisitor(Json::Value& vOutJsValue)
{
	vOutJsValue["bankIdx"] = m_nBankerIdx;
	vOutJsValue["baseBet"] = getBaseBet();
	vOutJsValue["curBet"] = getCurBet();
	vOutJsValue["mainPool"] = m_nMailPool;
	vOutJsValue["curActIdx"] = m_nCurActIdx ;
	vOutJsValue["betRound"] = m_nBetRound ;
	CLogMgr::SharedLogMgr()->ErrorLog("temp set bet round = 0 ") ;
}

void CGoldenRoom::sendRoomPlayersInfo(uint32_t nSessionID)
{
	stMsgGoldenRoomPlayers msgInfo ;
	msgInfo.nPlayerCnt = (uint8_t)getSitDownPlayerCount();
	CAutoBuffer auBuffer(sizeof(msgInfo) + sizeof(stGoldenRoomInfoPayerItem) * msgInfo.nPlayerCnt);
	auBuffer.addContent(&msgInfo,sizeof(msgInfo));

	uint8_t nSeatCount = (uint8_t)getSeatCount();
	stGoldenRoomInfoPayerItem item ;
	uint8_t nDisCardCnt = GOLDEN_PEER_CARD;
	for ( uint8_t nIdx = 0 ; nIdx < nSeatCount ; ++nIdx )
	{
		CGoldenRoomPlayer* psit = (CGoldenRoomPlayer*)getPlayerByIdx(nIdx) ;
		if ( psit )
		{
			memset(item.vHoldChard,0,sizeof(item.vHoldChard)) ;
			item.nBetCoin = psit->getBetCoin() ;
			item.nCoin = psit->getCoin() ;
			item.nIdx = psit->getIdx() ;
			item.nStateFlag = psit->getState() ;
			item.nUserUID = psit->getUserUID() ;
			for ( uint8_t nCardIdx = 0 ; nCardIdx < nDisCardCnt ; ++nCardIdx )
			{
				item.vHoldChard[nCardIdx] = psit->getCardByIdx(nCardIdx) ;
			}
			auBuffer.addContent(&item,sizeof(item)) ;
			CLogMgr::SharedLogMgr()->PrintLog("send players uid = %u, state = %u",item.nUserUID,item.nStateFlag);
		}
	}

	sendMsgToPlayer((stMsg*)auBuffer.getBufferPtr(),auBuffer.getContentSize(),nSessionID) ;
	CLogMgr::SharedLogMgr()->PrintLog("send room info to session id = %d, player cnt = %d ", nSessionID,msgInfo.nPlayerCnt) ;
}

uint32_t CGoldenRoom::getBaseBet() // ji chu di zhu ;
{
	return m_nBaseBet ;
}

void CGoldenRoom::onGameWillBegin()
{
	IRoom::onGameWillBegin() ;
	m_nCurBet = getBaseBet();
	m_nMailPool = 0;
	m_nBetRound = 0 ;

	uint16_t nSeatCnt = getSeatCount() ;
	for ( uint8_t nIdx = 0; nIdx < nSeatCnt; ++nIdx )
	{
		ISitableRoomPlayer* pp = getPlayerByIdx(nIdx) ;
		if ( pp && pp->isHaveState(eRoomPeer_Ready) )
		{
			pp->setCoin(pp->getCoin() - getDeskFee() ) ;
			addTotoalProfit(getDeskFee());
			pp->onGameBegin();

			// xia di zhu 
			CGoldenRoomPlayer*pRG = (CGoldenRoomPlayer*)pp ;
			pRG->betCoin(getCurBet());
			m_nMailPool += getCurBet() ;
		}
	}


	getPoker()->RestAllPoker() ;

	m_nBankerIdx = GetFirstInvalidIdxWithState(m_nBankerIdx ,eRoomPeer_Ready );
	m_nCurActIdx = m_nBankerIdx ;
}

void CGoldenRoom::onGameDidEnd()
{
	ISitableRoom::onGameDidEnd();
	m_nBetRound = 0 ;
}

void CGoldenRoom::onPlayerWillStandUp( ISitableRoomPlayer* pPlayer )
{
	if ( pPlayer->isHaveState(eRoomPeer_CanAct) )
	{
		ISitableRoom::onPlayerWillStandUp(pPlayer) ;
	}
	else
	{
		playerDoStandUp(pPlayer);
	}
}

uint32_t CGoldenRoom::getLeastCoinNeedForCurrentGameRound(ISitableRoomPlayer* pp)
{
	return 0 ;
}

void CGoldenRoom::prepareCards()
{
	uint8_t nSeatCnt = (uint8_t)getSeatCount() ;
	for ( uint8_t nIdx = 0 ; nIdx < nSeatCnt ; ++nIdx )
	{
		CGoldenRoomPlayer* pRoomPlayer = (CGoldenRoomPlayer*)getPlayerByIdx(nIdx) ;
		if ( pRoomPlayer && pRoomPlayer->isHaveState(eRoomPeer_CanAct))
		{
			uint8_t nCardCount = GOLDEN_PEER_CARD ;
			uint8_t nCardIdx = 0 ;
			while ( nCardIdx < nCardCount )
			{
				pRoomPlayer->onGetCard(nCardIdx,getPoker()->GetCardWithCompositeNum()) ;
				++nCardIdx ;
			}
		}
	}
}

uint32_t CGoldenRoom::coinNeededToSitDown()
{
	return getBaseBet() * 4 ;
}

void CGoldenRoom::caculateGameResult()
{
	stMsgGoldenResult msgResult ;
	msgResult.cWinnerIdx = GetFirstInvalidIdxWithState(1,eRoomPeer_CanAct) ;
	
	auto pPlayer = (CGoldenRoomPlayer*)getPlayerByIdx(msgResult.cWinnerIdx) ;
	assert(pPlayer && "win player can not be null") ;
	if ( pPlayer == nullptr )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("why win player is null ?") ;
		return ;
	}
	
	msgResult.nWinCoin = m_nMailPool - pPlayer->getBetCoin() ;
	uint32_t nTax = (uint32_t)(float(msgResult.nWinCoin) * getChouShuiRate() + 0.5f);
	pPlayer->setCoin(m_nMailPool + pPlayer->getCoin() - nTax );
	pPlayer->addWinCoin(m_nMailPool - nTax );
	setBankerIdx(pPlayer->getIdx());  // winner is next banker 
	msgResult.nFinalCoin = pPlayer->getCoin();
	CLogMgr::SharedLogMgr()->PrintLog("room id = %u uid = %u win game , tax = %u , win = %u , final = %u",getRoomID(),pPlayer->getUserUID(),nTax,msgResult.nWinCoin,msgResult.nFinalCoin);

	sendRoomMsg(&msgResult,sizeof(msgResult)) ;
}

uint8_t CGoldenRoom::onPlayerAction(uint32_t nAct, uint32_t& nValue, ISitableRoomPlayer* pPlayer )
{
	if ( pPlayer->getIdx() != getCurActIdx() )
	{
		if ( nAct != eRoomPeerAction_ViewCard && eRoomPeerAction_GiveUp != nAct )
		{
			return 1 ;
		}
	}

	if ( pPlayer->isHaveState(eRoomPeer_CanAct) == false )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("can not do act , you are not can act uid = %u",pPlayer->getUserUID()) ;
		return 3 ;
	}

	switch (nAct)
	{
	case eRoomPeerAction_Add:
		{
			uint32_t nNeedCoin = nValue + m_nCurBet ;
			if ( pPlayer->isHaveState(eRoomPeer_Looked) )
			{
				nNeedCoin *= 2 ;
			}

			if ( nNeedCoin > pPlayer->getCoin() )
			{
				CLogMgr::SharedLogMgr()->PrintLog("coin not enough can not add , uid = %u",pPlayer->getUserUID()) ;
				return 6 ; 
			}

			m_nCurBet += nValue ;
			((CGoldenRoomPlayer*)pPlayer)->betCoin(nNeedCoin);
			m_nMailPool += nNeedCoin ;
		}
		break;
	case eRoomPeerAction_Follow:
		{
			uint32_t nNeedCoin = m_nCurBet ;
			if ( pPlayer->isHaveState(eRoomPeer_Looked) )
			{
				nNeedCoin *= 2 ;
			}

			if ( nNeedCoin > pPlayer->getCoin() )
			{
				CLogMgr::SharedLogMgr()->PrintLog("coin not enough can not Follow , uid = %u",pPlayer->getUserUID()) ;
				return 6 ; 
			}

			((CGoldenRoomPlayer*)pPlayer)->betCoin(nNeedCoin);
			m_nMailPool += nNeedCoin ;
		}
		break;
	case eRoomPeerAction_ViewCard:
		{
			pPlayer->setState(eRoomPeer_Looked) ;
		}
		break;
	case eRoomPeerAction_GiveUp:
		{
			if ( pPlayer->isDelayStandUp() )
			{
				if ( getDelegate() )
				{
					CGoldenRoomPlayer* pP = (CGoldenRoomPlayer*)pPlayer;
					getDelegate()->onUpdatePlayerGameResult(this,pPlayer->getUserUID(),(int32_t)pP->getBetCoin() * -1) ;
				}
				playerDoStandUp(pPlayer);
			}
			else
			{
				pPlayer->setState(eRoomPeer_GiveUp) ;
			}
		}
		break;
	default:
		return 4;
	}
	return 0 ;
}

uint8_t CGoldenRoom::informPlayerAct( bool bStepNext )
{
	stMsgGoldenRoomWaitPlayerAct msgAct ;
	if ( bStepNext )
	{
		int8_t nPreIdx = m_nCurActIdx ;
		m_nCurActIdx = GetFirstInvalidIdxWithState(m_nCurActIdx + 1 ,eRoomPeer_CanAct) ;
		int8_t nNowIdx = m_nCurActIdx ;
		if ( nPreIdx < nNowIdx )
		{
			if ( nPreIdx < m_nBankerIdx && m_nBankerIdx <= nNowIdx )
			{
				++m_nBetRound ;
			}
		}
		else 
		{
			if ( m_nBankerIdx <= nNowIdx || m_nBankerIdx > nPreIdx )
			{
				++m_nBetRound ;
			}
		}
	}
	msgAct.nActPlayerIdx = m_nCurActIdx ;
	sendRoomMsg(&msgAct,sizeof(msgAct)) ;
	return m_nCurActIdx ;
}

bool CGoldenRoom::onPlayerPK(ISitableRoomPlayer* pActPlayer , ISitableRoomPlayer* pTargetPlayer )
{
	CGoldenRoomPlayer* pP = (CGoldenRoomPlayer*)pActPlayer;
	CGoldenRoomPlayer* pT = (CGoldenRoomPlayer*)pTargetPlayer ;
	uint32_t nNeedCoin = m_nCurBet * 2 ;
	if ( pP->isHaveState(eRoomPeer_Looked) )
	{
		nNeedCoin *= 2 ;
	}

	if ( nNeedCoin > pP->getCoin() )
	{
		nNeedCoin = pP->getCoin();
	}

	pP->betCoin(nNeedCoin);
	m_nMailPool += nNeedCoin ;

	bool bWin = pP->getPeerCard()->pk(pT->getPeerCard()) == IPeerCard::PK_RESULT_WIN;
	if ( bWin )
	{
		pT->setState(eRoomPeer_PK_Failed);
	}
	else
	{
		pP->setState(eRoomPeer_PK_Failed) ;
	}
	return bWin ;
}

ISitableRoomPlayer* CGoldenRoom::doCreateSitableRoomPlayer()
{
	return new CGoldenRoomPlayer();
}