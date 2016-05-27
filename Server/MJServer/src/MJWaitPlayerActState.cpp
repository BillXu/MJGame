#include "MJWaitPlayerActState.h"
#include "MJDefine.h"
#include "LogManager.h"
// wait player act 
bool CMJWaitPlayerActState::onMsg(Json::Value& prealMsg ,uint16_t nMsgType, eMsgPort eSenderPort , uint32_t nSessionID)
{
	if ( nMsgType != MSG_PLAYER_ACT )
	{
		return false ;
	}

	uint8_t nActType = prealMsg["actType"].asUInt();
	Json::Value msgBack ;

	auto pPlayer = (CMJRoomPlayer*)m_pRoom->getSitdownPlayerBySessionID(nSessionID);
	if ( pPlayer == nullptr )
	{
		msgBack["ret"] = 4 ;
		m_pRoom->sendMsgToPlayer(msgBack,nMsgType,nSessionID) ;
		return true ;
	}

	if ( isIdxInWaitList(pPlayer->getIdx()) == false )
	{
		msgBack["ret"] = 1 ;
		m_pRoom->sendMsgToPlayer(msgBack,nMsgType,nSessionID) ;
		return true ;
	}

	bool isAlreadyHu = pPlayer->isHaveState(eRoomPeer_AlreadyHu) ;

	stPlayerActTypeActionItem* pActTypeItem = new stPlayerActTypeActionItem ;
	pActTypeItem->nActType = nActType ;
	pActTypeItem->nActIdx = pPlayer->getIdx() ;
	pActTypeItem->nCardNumber = prealMsg["card"].asUInt() ;

	CLogMgr::SharedLogMgr()->PrintLog("player do act = %u , idx = %u",pActTypeItem->nActType,pActTypeItem->nActIdx) ;
	switch ( nActType )
	{
	case eMJAct_Pass:
		{
			delete pActTypeItem ;
			pActTypeItem = nullptr ;
			setWaitTime(eTime_WaitPlayerAct) ;
			return true;
		}
		break;
	case eMJAct_Hu:
		{
			CMJRoom* pRoom = (CMJRoom*)m_pRoom ;
			if ( !pRoom->canPlayerHuPai(pActTypeItem->nActIdx,0) )
			{
				msgBack["ret"] = 2 ;
				m_pRoom->sendMsgToPlayer(msgBack,nMsgType,nSessionID) ;
				delete pActTypeItem ;
				pActTypeItem = nullptr ;
				return true ;
			}
		}
		break ;
	case eMJAct_Chu:
		{
			if ( isAlreadyHu && pPlayer->getNewFetchCard() != pActTypeItem->nCardNumber )
			{
				CLogMgr::SharedLogMgr()->ErrorLog("idx = %u already hu , can not chu other card , must new fetched card = %u",pPlayer->getIdx(),pPlayer->getNewFetchCard()) ;

				msgBack["ret"] = 4 ;
				m_pRoom->sendMsgToPlayer(msgBack,nMsgType,nSessionID) ;
				delete pActTypeItem ;
				pActTypeItem = nullptr ;
				return true ;
			}
		}
		break ;
	case eMJAct_BuGang_Pre:
	case eMJAct_BuGang:
	case eMJAct_AnGang:
		{
			uint8_t nCard = prealMsg["card"].asUInt() ;
			CMJRoom* pRoom = (CMJRoom*)m_pRoom ;
			if ( pRoom->canPlayerGangWithCard(pActTypeItem->nActIdx,nCard,true) == false )
			{
				msgBack["ret"] = 2 ;
				m_pRoom->sendMsgToPlayer(msgBack,MSG_PLAYER_ACT,nSessionID) ;

				delete pActTypeItem ;
				pActTypeItem = nullptr ;

				return true ;
			}

			if ( nActType == eMJAct_BuGang )
			{
				nActType = eMJAct_BuGang_Pre ;
			}
		}
		break ;
	default:
		delete pActTypeItem ;
		pActTypeItem = nullptr ;
		msgBack["ret"] = 2 ;
		m_pRoom->sendMsgToPlayer(msgBack,MSG_PLAYER_ACT,nSessionID) ;
		return true ;
	}
	responeWaitAct(pActTypeItem->nActIdx,pActTypeItem);
	return true ;
}

void CMJWaitPlayerActState::onWaitEnd( bool bTimeOut )
{
	if ( bTimeOut )
	{
		uint8_t nIdx = m_vWaitIdxs.front().nIdx ;
		auto pPlayer = m_pRoom->getPlayerByIdx(nIdx);
		assert(pPlayer && "player must not null" );

		stPlayerActTypeActionItem* pActTypeItem = new stPlayerActTypeActionItem ;
		pActTypeItem->nActType = eMJAct_Chu ;
		pActTypeItem->nActIdx = pPlayer->getIdx() ;
		CMJRoom* pRoom = (CMJRoom*)m_pRoom ;
		pActTypeItem->nCardNumber = pRoom->getPlayerAutoChuCardWhenTimeOut(pActTypeItem->nActIdx) ;
		if ( pPlayer->isHaveState(eRoomPeer_AlreadyHu) )
		{
			if ( pRoom->canPlayerHuPai(pPlayer->getIdx(),pActTypeItem->nCardNumber) )
			{
				pActTypeItem->nActType = eMJAct_Hu ;
				CLogMgr::SharedLogMgr()->PrintLog("wait time out , already hu , this card can hu , so gon hu , card = %u",pActTypeItem->nCardNumber) ;
			}
		}
		else
		{
			CLogMgr::SharedLogMgr()->PrintLog("wait player act time out sys do act chu idx = %u",pActTypeItem->nActIdx) ;
		}
		
		responeWaitAct(pActTypeItem->nActIdx,pActTypeItem);
	}
	else
	{
		auto pTargeState = (IExecuingState*)m_pRoom->getRoomStateByID(eRoomState_DoPlayerAct) ;
		pTargeState->setExecuteTime(getExecuteTime()) ;
		CLogMgr::SharedLogMgr()->PrintLog("go to do act list size = %u",m_vActList.size()) ;
		pTargeState->setExecuteActs(m_vActList) ;
		m_pRoom->goToState(pTargeState) ;
	}
}

float CMJWaitPlayerActState::getExecuteTime()
{
	return eTime_DoPlayerActChuPai ;
}

// do player act 
void CMJDoPlayerActState::doExecuteAct( stActionItem* pAct)
{
	stPlayerActTypeActionItem* pdoAct = (stPlayerActTypeActionItem*)pAct ;
	m_edoAct = (eMJActType)pdoAct->nActType ;

	m_nCardNumber = pdoAct->nCardNumber ;
	m_nCurIdx = pdoAct->nActIdx ;

	CMJRoom* pRoom = (CMJRoom*)m_pRoom ;
	CLogMgr::SharedLogMgr()->PrintLog("do player act = %u , idx = %u",pdoAct->nActIdx,pdoAct->nActType) ;
	switch ( m_edoAct )
	{
	case eMJAct_Mo:
		{
			pRoom->onPlayerMoPai(pdoAct->nActIdx);
		}
		break;
	case eMJAct_Hu:
		{
			pRoom->onPlayerHuPai(pdoAct->nActIdx);
		}
		break ;
	case eMJAct_Chu:
		{
			pRoom->onPlayerChuPai(pdoAct->nActIdx,pdoAct->nCardNumber) ;
			pRoom->checkPlayersNeedTheCard(pdoAct->nCardNumber,m_vecCardPlayerIdxs,pdoAct->nActIdx) ;
		}
		break ;
	case eMJAct_BuGang_Pre:
		{
			pRoom->onPlayerBuGangPre(pdoAct->nActIdx,pdoAct->nCardNumber) ;
			if ( pRoom->checkPlayersNeedTheCard(pdoAct->nCardNumber,m_vecCardPlayerIdxs,pdoAct->nActIdx) )
			{
				
			}
			else
			{
				m_edoAct = eMJAct_BuGang_Done ;
				pRoom->onPlayerGangPai(pdoAct->nActIdx,m_nCardNumber,true,pdoAct->nActIdx) ;
			}
		}
		break ;
	case eMJAct_BuGang_Done:
		{
			pRoom->onPlayerGangPai(pdoAct->nActIdx,m_nCardNumber,true,pdoAct->nActIdx) ;
		}
		break;
	case eMJAct_AnGang:
		{
			pRoom->onPlayerGangPai(pdoAct->nActIdx,m_nCardNumber,false,pdoAct->nActIdx) ;
		}
		break ;
	default:
		break ;
	}
}

void CMJDoPlayerActState::onExecuteOver()
{
	switch ( m_edoAct )
	{
	case eMJAct_BuGang_Done:
	case eMJAct_AnGang:
	case eMJAct_Mo:
		{
			auto pRoom = (CMJRoom*)m_pRoom ;
			auto ppPlayer = (CMJRoomPlayer*)pRoom->getPlayerByIdx(m_nCurIdx) ;
			uint8_t nNewCard = ppPlayer->getNewFetchCard() ;
			if ( ppPlayer->isCardBeWanted(nNewCard,true) )
			{
				CLogMgr::SharedLogMgr()->PrintLog("idx = %u , self need the card = %u" , m_nCurIdx,nNewCard) ;
				pRoom->onInformActAboutCard(m_nCurIdx,nNewCard,m_nCurIdx);
			}

			auto pTargeState = (IWaitingState*)m_pRoom->getRoomStateByID(eRoomState_WaitPlayerAct) ;

			float fWaitTime = eTime_WaitPlayerAct;
			if ( ppPlayer->isHaveState(eRoomPeer_AlreadyHu) )
			{
				fWaitTime *= 0.5 ;
				CLogMgr::SharedLogMgr()->PrintLog("idx = %u already hu, so act time will be half as before",m_nCurIdx) ;
			}
			pTargeState->setWaitTime(fWaitTime) ;
			
			pTargeState->addWaitingTarget(m_nCurIdx) ;
			m_pRoom->goToState(pTargeState) ;
		}
		break ;
	case eMJAct_Chu:
		{
			if ( m_vecCardPlayerIdxs.empty() )
			{
				CLogMgr::SharedLogMgr()->PrintLog(" no one need the card = %u, from idx = %u ,so wait them",m_nCardNumber,m_nCurIdx) ;

				CMJRoom* pRoom = (CMJRoom*)m_pRoom ;
				if ( pRoom->getLeftCardCnt() < 1 )
				{
					pRoom->goToState(eRoomState_GameEnd) ;
					return ;
				}

				auto pTargeState = (IExecuingState*)m_pRoom->getRoomStateByID(eRoomState_DoPlayerAct) ;
				pTargeState->setExecuteTime(eTime_DoPlayerMoPai) ;

				stPlayerActTypeActionItem* pActTypeItem = new stPlayerActTypeActionItem ;
				pActTypeItem->nActType = eMJAct_Mo ;
				pActTypeItem->nActIdx = pRoom->getNextActPlayerIdx(m_nCurIdx) ;
				pActTypeItem->nCardNumber = 0 ;
				VEC_ACTITEM v ;
				v.push_back(pActTypeItem) ;
				pTargeState->setExecuteActs(v) ;

				m_pRoom->goToState(pTargeState) ;
			}
			else
			{
				CLogMgr::SharedLogMgr()->PrintLog(" %u player need the card = %u, from idx = %u ,so wait them",m_vecCardPlayerIdxs.size(),m_nCardNumber,m_nCurIdx) ;
				auto pTargeState = (CMJWaitOtherActState*)m_pRoom->getRoomStateByID(eRoomState_WaitOtherPlayerAct) ;
				pTargeState->setWaitTime(eTime_WaitPlayerAct) ;
				auto pRoom = (CMJRoom*)m_pRoom ;
				for ( auto ref : m_vecCardPlayerIdxs )
				{
					pTargeState->addWaitingTarget(ref) ;
				}

				stWaitCardInfo info ;
				info.isBuGang = false ;
				info.nCardNumber = m_nCardNumber ;
				info.nCardProvideIdx = m_nCurIdx ;
				pTargeState->setWaitCardInfo(&info) ;

				m_pRoom->goToState(pTargeState) ;
			}
		}
		break;
	case eMJAct_Hu:
		{
			CMJRoom* pRoom = (CMJRoom*)m_pRoom ;
			if ( pRoom->getLeftCardCnt() < 1 )
			{
				pRoom->goToState(eRoomState_GameEnd) ;
				return ;
			}

			auto pTargeState = (IExecuingState*)m_pRoom->getRoomStateByID(eRoomState_DoPlayerAct) ;
			pTargeState->setExecuteTime(eTime_DoPlayerMoPai) ;

			stPlayerActTypeActionItem* pActTypeItem = new stPlayerActTypeActionItem ;
			pActTypeItem->nActType = eMJAct_Mo ;
			pActTypeItem->nActIdx = pRoom->getNextActPlayerIdx(m_nCurIdx) ;
			pActTypeItem->nCardNumber = 0 ;
			VEC_ACTITEM v ;
			v.push_back(pActTypeItem) ;
			pTargeState->setExecuteActs(v) ;

			m_pRoom->goToState(pTargeState) ;
		}
		break ;
	case eMJAct_BuGang_Pre:
		{
			auto pTargeState = (CMJWaitOtherActState*)m_pRoom->getRoomStateByID(eRoomState_WaitOtherPlayerAct) ;
			pTargeState->setWaitTime(eTime_WaitPlayerAct) ;
			auto pRoom = (CMJRoom*)m_pRoom ;
			for ( auto ref : m_vecCardPlayerIdxs )
			{
				pTargeState->addWaitingTarget(ref) ;
			}

			stWaitCardInfo info ;
			info.isBuGang = true ;
			info.nCardNumber = m_nCardNumber ;
			info.nCardProvideIdx = m_nCurIdx ;
			pTargeState->setWaitCardInfo(&info) ;

			m_pRoom->goToState(pTargeState) ;
		}
		break ;
	default:
		break ;
	}
}

// wait other player act 
void CMJWaitOtherActState::enterState(IRoom* pRoom)
{
	IWaitingState::enterState(pRoom) ;
	CMJRoom* pTargetRoom = (CMJRoom*)pRoom ;
	
	for ( auto ret : m_vWaitIdxs )
	{
		pTargetRoom->onInformActAboutCard(ret.nIdx,m_tInfo.nCardNumber,m_tInfo.nCardProvideIdx);
	}
}

bool CMJWaitOtherActState::onMsg(Json::Value& prealMsg ,uint16_t nMsgType, eMsgPort eSenderPort , uint32_t nSessionID)
{
	if ( nMsgType != MSG_PLAYER_ACT )
	{
		return false ;
	}

	uint8_t nActType = prealMsg["actType"].asUInt();
	Json::Value msgBack ;

	auto pPlayer = m_pRoom->getSitdownPlayerBySessionID(nSessionID);
	if ( pPlayer == nullptr )
	{
		msgBack["ret"] = 4 ;
		m_pRoom->sendMsgToPlayer(msgBack,nMsgType,nSessionID) ;
		return true ;
	}

	if ( isIdxInWaitList(pPlayer->getIdx()) == false )
	{
		msgBack["ret"] = 1 ;
		m_pRoom->sendMsgToPlayer(msgBack,nMsgType,nSessionID) ;
		return true ;
	}

	stPlayerActTypeActionItem* pActTypeItem = new stPlayerActTypeActionItem ;
	pActTypeItem->nActType = nActType ;
	pActTypeItem->nActIdx = pPlayer->getIdx() ;
	pActTypeItem->nCardNumber = m_tInfo.nCardNumber;
	pActTypeItem->nExePrio = nActType ;

	CMJRoom* pRoom = (CMJRoom*)m_pRoom ;
	switch ( nActType )
	{
	case eMJAct_Hu:
		{
			if ( !pRoom->canPlayerHuPai(pActTypeItem->nActIdx,m_tInfo.nCardNumber) )
			{
				msgBack["ret"] = 2 ;
				m_pRoom->sendMsgToPlayer(msgBack,nMsgType,nSessionID) ;
				return true ;
			}
		}
		break ;
	case eMJAct_Peng:
		{
			if ( !pRoom->canPlayerPengPai(pActTypeItem->nActIdx,m_tInfo.nCardNumber) )
			{
				msgBack["ret"] = 2 ;
				m_pRoom->sendMsgToPlayer(msgBack,nMsgType,nSessionID) ;
				return true ;
			}
		}
		break;
	case eMJAct_MingGang:
		{
			CMJRoom* pRoom = (CMJRoom*)m_pRoom ;
			if ( pRoom->getLeftCardCnt() < 1 || ! pRoom->canPlayerGangWithCard(pActTypeItem->nActIdx,m_tInfo.nCardNumber,false) )
			{
				msgBack["ret"] = 2 ;
				m_pRoom->sendMsgToPlayer(msgBack,MSG_PLAYER_ACT,nSessionID) ;
				return true ;
			}
		}
		break ;
	case eMJAct_Pass:
		{
			delete pActTypeItem ;
			pActTypeItem = nullptr ;
		}
		break ;
	default:
		delete pActTypeItem ;
		pActTypeItem = nullptr ;
		msgBack["ret"] = 2 ;
		m_pRoom->sendMsgToPlayer(msgBack,MSG_PLAYER_ACT,nSessionID) ;
		return true ;
	}
	responeWaitAct(pPlayer->getIdx(),pActTypeItem);
	return true ;
}

void CMJWaitOtherActState::onWaitEnd( bool bTimeOut )
{
	if ( m_vActList.empty() )
	{
		if ( m_tInfo.isBuGang )
		{
			// go on gang pai 
			auto pTargeState = (IExecuingState*)m_pRoom->getRoomStateByID(eRoomState_DoPlayerAct) ;
			pTargeState->setExecuteTime(eTime_DoPlayerMoPai) ;

			stPlayerActTypeActionItem* pActTypeItem = new stPlayerActTypeActionItem ;
			pActTypeItem->nActType = eMJAct_BuGang_Done ;
			pActTypeItem->nActIdx = m_tInfo.nCardProvideIdx ;
			pActTypeItem->nCardNumber = 0 ;
			VEC_ACTITEM v ;
			v.push_back(pActTypeItem) ;
			pTargeState->setExecuteActs(v) ;

			m_pRoom->goToState(pTargeState) ;
		}
		else
		{
			// go to next player act 
			CMJRoom* pRoom = (CMJRoom*)m_pRoom ;
			if ( pRoom->getLeftCardCnt() < 1 )
			{
				pRoom->goToState(eRoomState_GameEnd) ;
				return ;
			}

			auto pTargeState = (IExecuingState*)m_pRoom->getRoomStateByID(eRoomState_DoPlayerAct) ;
			pTargeState->setExecuteTime(eTime_DoPlayerMoPai) ;

			stPlayerActTypeActionItem* pActTypeItem = new stPlayerActTypeActionItem ;
			pActTypeItem->nActType = eMJAct_Mo ;
			pActTypeItem->nActIdx = pRoom->getNextActPlayerIdx(m_tInfo.nCardProvideIdx) ;
			pActTypeItem->nCardNumber = 0 ;
			VEC_ACTITEM v ;
			v.push_back(pActTypeItem) ;
			pTargeState->setExecuteActs(v) ;

			m_pRoom->goToState(pTargeState) ;
		}
	}
	else
	{
		auto pTargeState = (CMJDoOtherPlayerActState*)m_pRoom->getRoomStateByID(eRoomState_DoOtherPlayerAct) ;
		pTargeState->setExecuteTime(eTime_DoPlayerActChuPai) ;
		pTargeState->setExecuteActs(m_vActList) ;
		pTargeState->setWaitCardInfo(&m_tInfo);
		m_pRoom->goToState(pTargeState) ;
	}
}	

// do other player act 
void CMJDoOtherPlayerActState::doExecuteAct( stActionItem* pAct)
{
	stPlayerActTypeActionItem* pdoAct = (stPlayerActTypeActionItem*)pAct ;
	m_edoAct = (eMJActType)pdoAct->nActType ;

	if ( m_nCurIdx < pdoAct->nActIdx )
	{
		m_nCurIdx = pdoAct->nActIdx ;
	}

	CMJRoom* pRoom = (CMJRoom*)m_pRoom ;

	switch ( m_edoAct )
	{
	case eMJAct_Hu:
		{
			pRoom->onPlayerHuPai(pdoAct->nActIdx,m_tInfo.nCardNumber,m_tInfo.nCardProvideIdx,m_tInfo.isBuGang);
		}
		break ;
	case eMJAct_MingGang:
		{
			pRoom->onPlayerGangPai(pdoAct->nActIdx,m_tInfo.nCardNumber,false,m_tInfo.nCardProvideIdx ) ;
		}
		break ;
	case eMJAct_Peng:
		{
			CLogMgr::SharedLogMgr()->PrintLog("idx = %u , do peng " ,pdoAct->nActIdx) ;
			pRoom->onPlayerPeng(pdoAct->nActIdx,m_tInfo.nCardNumber,m_tInfo.nCardProvideIdx);
		}
		break;
	default:
		break ;
	}
}

void CMJDoOtherPlayerActState::onExecuteOver()
{
	CMJRoom* pRoom = (CMJRoom*)m_pRoom ;
	switch ( m_edoAct )
	{
	case eMJAct_Hu:
		{
			CMJRoom* pRoom = (CMJRoom*)m_pRoom ;
			if ( pRoom->getLeftCardCnt() < 1 )
			{
				pRoom->goToState(eRoomState_GameEnd) ;
				return ;
			}

			auto pTargeState = (IExecuingState*)m_pRoom->getRoomStateByID(eRoomState_DoPlayerAct) ;
			pTargeState->setExecuteTime(eTime_DoPlayerMoPai) ;

			stPlayerActTypeActionItem* pActTypeItem = new stPlayerActTypeActionItem ;
			pActTypeItem->nActType = eMJAct_Mo ;
			pActTypeItem->nActIdx = pRoom->getNextActPlayerIdx(m_nCurIdx) ;
			pActTypeItem->nCardNumber = 0 ;
			VEC_ACTITEM v ;
			v.push_back(pActTypeItem) ;
			pTargeState->setExecuteActs(v) ;

			m_pRoom->goToState(pTargeState) ;
		}
		break ;
	case eMJAct_MingGang:
	case eMJAct_Peng:
		{
			auto pTargeState = (IWaitingState*)m_pRoom->getRoomStateByID(eRoomState_WaitPlayerAct) ;
			pTargeState->setWaitTime(eTime_WaitPlayerAct) ;
			auto pRoom = (CMJRoom*)m_pRoom ;
			CLogMgr::SharedLogMgr()->PrintLog("do peng ok wait idx = %u act " ,m_nCurIdx) ;
			pTargeState->addWaitingTarget(m_nCurIdx) ;
			m_pRoom->goToState(pTargeState) ;
		}
		break;
	default:
		break ;
	}
}