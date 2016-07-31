#include "NewMJRoom.h"
#include "WaitStartState.h"
#include "StartGameState.h"
#include "WaitPlayerActState.h"
#include "WaitActWithChuedCard.h"
#include "GameOverState.h"
#include "RoomConfig.h"
#include "NewMJRoomPlayer.h"
#include "LogManager.h"
CNewMJRoom::CNewMJRoom()
{

}

bool CNewMJRoom::onFirstBeCreated(IRoomManager* pRoomMgr,stBaseRoomConfig* pConfig, uint32_t nRoomID , Json::Value& vJsValue)
{
	m_pRoomConfig = (stMJRoomConfig*)pConfig ;
	ISitableRoom::onFirstBeCreated(pRoomMgr,pConfig,nRoomID,vJsValue) ;
	m_tPoker.initAllCard(eMJ_COMMON);
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

void CNewMJRoom::doStartGame()
{
	ISitableRoom::doStartGame();

}

void CNewMJRoom::onGameOver()
{
	ISitableRoom::onGameOver() ;
}

void CNewMJRoom::onGameWillBegin()
{
	ISitableRoom::onGameWillBegin();
}

void CNewMJRoom::onGameDidEnd()
{
	ISitableRoom::onGameDidEnd() ;
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

void CNewMJRoom::sendActListToPlayerAboutCard( uint8_t nPlayerIdx , std::list<eMJActType>& vList , uint8_t nCard )
{
	auto pp = getPlayerByIdx(nPlayerIdx) ;
	if ( !pp )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("player idx = %u , not in room , how to send act list",nPlayerIdx ) ;
		return ;
	}

	Json::Value jsmsg ;
	jsmsg["cardNum"] = nCard ;

	Json::Value jsActList ;
	for ( auto& ref : vList )
	{
		jsActList[jsActList.size()] = ref ;
	}

	jsmsg["acts"] = jsActList ;
	sendMsgToPlayer(jsmsg,MSG_PLAYER_WAIT_ACT_ABOUT_OTHER_CARD,pp->getSessionID());
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
	}

	// send msg 
	Json::Value msg ;
	msg["idx"] = nActPlayerIdx ;
	msg["actType"] = eMJAct_Peng ;
	msg["card"] = nTargetCard ;
	sendRoomMsg(msg,MSG_ROOM_ACT) ;

	return bRet ;
}

bool CNewMJRoom::onPlayerHu( uint8_t nActPlayerIdx, uint8_t nInvokerPlayerIdx , uint8_t nTargetCard )
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

	uint8_t nFanType = 0 ; uint16_t nFanShu = 0 ;
	bool bRet = ppHu->getMJPeerCard()->onHu(nTargetCard,nFanType,nFanShu);
	if ( !bRet )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("say you hu , but you donot hu, idx = %u,card = %u",nActPlayerIdx,nTargetCard) ;
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

	// send msg 
	Json::Value msg ;
	msg["idx"] = nActPlayerIdx ;
	msg["actType"] = eMJAct_MingGang ;
	msg["card"] = nTargetCard ;
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
	auto pNewCard = m_tPoker.getCard();
	pp->onRecievedCard(eMJAct_AnGang,pNewCard) ;
	return pp->getMJPeerCard()->onAnGang(nTargetCard,pNewCard);
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
	return pp->getMJPeerCard()->onChuCard(nCard);
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

uint32_t CNewMJRoom::getHuWinCoin(uint8_t nFanXing,uint16_t nFanshu ,bool isSelfHu )
{
	return nFanshu * getBaseBet() ;
}