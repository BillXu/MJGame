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

	uint8_t nActType = prealMsg["act"].asUInt();
	uint8_t nCard = prealMsg["card"].asUInt() ;
	Json::Value msgBack ;

	auto pPlayer = (CMJRoomPlayer*)m_pRoom->getSitdownPlayerBySessionID(nSessionID);
	if ( pPlayer == nullptr )
	{
		msgBack["ret"] = 4 ;
		m_pRoom->sendMsgToPlayer(msgBack,MSG_PLAYER_ACT,nSessionID) ;
		return true ;
	}

	if ( isIdxInWaitList(pPlayer->getIdx()) == false )
	{
		msgBack["ret"] = 1 ;
		m_pRoom->sendMsgToPlayer(msgBack,MSG_PLAYER_ACT,nSessionID) ;
		return true ;
	}

	switch ( nActType )
	{
	case eMJAct_Pass:
		{
			return ;
		}
		break;
	case eMJAct_Hu:
		{
			if ( ! pPlayer->canHuPai(nCard) )
			{
				msgBack["ret"] = 2 ;
				m_pRoom->sendMsgToPlayer(msgBack,MSG_PLAYER_ACT,nSessionID) ;
				return true ;
			}
		}
		break ;
	case eMJAct_Chu:
		{

		}
		break ;
	case eMJAct_BuGang:
	case eMJAct_AnGang:
		{
			CMJRoom* pRoom = (CMJRoom*)m_pRoom ;
			if ( pRoom->getLeftCardCnt() < 1 || !pPlayer->canGangWithCard(nCard) )
			{
				msgBack["ret"] = 2 ;
				m_pRoom->sendMsgToPlayer(msgBack,MSG_PLAYER_ACT,nSessionID) ;
				return true ;
			}

			if ( nActType == eMJAct_BuGang )
			{
				nActType = eMJAct_BuGang_Pre ;
			}
		}
		break ;
	default:
		msgBack["ret"] = 2 ;
		m_pRoom->sendMsgToPlayer(msgBack,MSG_PLAYER_ACT,nSessionID) ;
		return true ;
	}

	stPlayerActTypeActionItem* pActTypeItem = new stPlayerActTypeActionItem ;
	pActTypeItem->nActType = nActType ;
	pActTypeItem->nActIdx = pPlayer->getIdx() ;
	pActTypeItem->nCardNumber = nCard ;
	pActTypeItem->eCardFrom = pPlayer->getNewFetchedFrom();
	responeWaitAct(pActTypeItem->nActIdx,pActTypeItem);
	return true ;
}

void CMJWaitPlayerActState::onWaitEnd( bool bTimeOut )
{
	if ( bTimeOut )
	{
		uint8_t nIdx = m_vWaitIdxs.front().nIdx ;
		auto pPlayer = (CMJRoomPlayer*)m_pRoom->getPlayerByIdx(nIdx);
		assert(pPlayer && "player must not null" );
		stPlayerActTypeActionItem* pActTypeItem = new stPlayerActTypeActionItem ;
		pActTypeItem->nActType = eMJAct_Chu ;
		pActTypeItem->nActIdx = pPlayer->getIdx() ;
		pActTypeItem->nCardNumber = pPlayer->getNewFetchCard() ;
		pActTypeItem->eCardFrom = pPlayer->getNewFetchedFrom();
		if ( pActTypeItem->nCardNumber == 0 )
		{
			pActTypeItem->nCardNumber = pPlayer->getCardByIdx(0,false) ;
			CLogMgr::SharedLogMgr()->ErrorLog("why new fetch card is null ? not fectch why wait you act ?") ;
		}
		responeWaitAct(pActTypeItem->nActIdx,pActTypeItem);
	}
	else
	{
		auto pTargeState = (IExecuingState*)m_pRoom->getRoomStateByID(eRoomState_DoPlayerAct) ;
		pTargeState->setExecuteTime(eTime_DoPlayerActChuPai) ;
		pTargeState->setExecuteActs(m_vActList) ;
		m_pRoom->goToState(pTargeState) ;
	}
}

// do player act 
void CMJDoPlayerActState::doExecuteAct( stActionItem* pAct)
{
	stPlayerActTypeActionItem* pdoAct = (stPlayerActTypeActionItem*)pAct ;
	m_edoAct = (eMJActType)pdoAct->nActType ;

	m_nCardNumber = pdoAct->nCardNumber ;
	m_eCardFrom = pdoAct->eCardFrom ;
	m_nCurIdx = pdoAct->nActIdx ;

	CMJRoom* pRoom = (CMJRoom*)m_pRoom ;

	switch ( m_edoAct )
	{
	case eMJAct_Mo:
		{
			pRoom->onPlayerMoPai(pdoAct->nActIdx);
		}
		break;
	case eMJAct_Hu:
		{
			pRoom->onPlayerHuPai(pdoAct->nActIdx,pdoAct->nCardNumber,( m_eCardFrom == eMJAct_MingGang || eMJAct_BuGang == m_eCardFrom || eMJAct_AnGang == m_eCardFrom) );
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
			if ( pRoom->checkPlayersNeedTheCard(pdoAct->nCardNumber,m_vecCardPlayerIdxs,pdoAct->nActIdx) )
			{
				pRoom->onPlayerBuGangPre(pdoAct->nActIdx,pdoAct->nCardNumber) ;
			}
			else
			{
				m_edoAct = eMJAct_BuGang_Done ;
				pRoom->onPlayerGangPai(pdoAct->nActIdx,m_nCardNumber,true) ;
			}
		}
		break ;
	case eMJAct_BuGang_Done:
		{
			pRoom->onPlayerGangPai(pdoAct->nActIdx,m_nCardNumber,true) ;
		}
		break;
	case eMJAct_AnGang:
		{
			pRoom->onPlayerGangPai(pdoAct->nActIdx,m_nCardNumber,false) ;
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
			auto pTargeState = (IWaitingState*)m_pRoom->getRoomStateByID(eRoomState_WaitPlayerAct) ;
			pTargeState->setWaitTime(eTime_WaitPlayerAct) ;
			auto pRoom = (CMJRoom*)m_pRoom ;
			pTargeState->addWaitingTarget(m_nCurIdx) ;
			m_pRoom->goToState(pTargeState) ;
		}
		break ;
	case eMJAct_Chu:
		{
			if ( m_vecCardPlayerIdxs.empty() )
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
			else
			{
				auto pTargeState = (CMJWaitOtherActState*)m_pRoom->getRoomStateByID(eRoomState_WaitOtherPlayerAct) ;
				pTargeState->setWaitTime(eTime_WaitPlayerAct) ;
				auto pRoom = (CMJRoom*)m_pRoom ;
				for ( auto ref : m_vecCardPlayerIdxs )
				{
					pTargeState->addWaitingTarget(ref) ;
				}

				stWaitCardInfo info ;
				info.isBuGang = false ;
				info.isCardFromGang = ( m_eCardFrom == eMJAct_MingGang || eMJAct_BuGang == m_eCardFrom || eMJAct_AnGang == m_eCardFrom ) ;
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
			info.isCardFromGang = ( m_eCardFrom == eMJAct_MingGang || eMJAct_BuGang == m_eCardFrom || eMJAct_AnGang == m_eCardFrom ) ;
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
bool CMJWaitOtherActState::onMsg(Json::Value& prealMsg ,uint16_t nMsgType, eMsgPort eSenderPort , uint32_t nSessionID)
{
	if ( nMsgType != MSG_PLAYER_ACT )
	{
		return false ;
	}

	uint8_t nActType = prealMsg["act"].asUInt();
	uint8_t nCard = prealMsg["card"].asUInt() ;
	Json::Value msgBack ;

	auto pPlayer = (CMJRoomPlayer*)m_pRoom->getSitdownPlayerBySessionID(nSessionID);
	if ( pPlayer == nullptr )
	{
		msgBack["ret"] = 4 ;
		m_pRoom->sendMsgToPlayer(msgBack,MSG_PLAYER_ACT,nSessionID) ;
		return true ;
	}

	if ( isIdxInWaitList(pPlayer->getIdx()) == false )
	{
		msgBack["ret"] = 1 ;
		m_pRoom->sendMsgToPlayer(msgBack,MSG_PLAYER_ACT,nSessionID) ;
		return true ;
	}

	switch ( nActType )
	{
	case eMJAct_Hu:
		{
			if ( ! pPlayer->canHuPai(nCard) )
			{
				msgBack["ret"] = 2 ;
				m_pRoom->sendMsgToPlayer(msgBack,MSG_PLAYER_ACT,nSessionID) ;
				return true ;
			}
		}
		break ;
	case eMJAct_Peng:
		{

		}
		break;
	case eMJAct_MingGang:
		{
			CMJRoom* pRoom = (CMJRoom*)m_pRoom ;
			if ( pRoom->getLeftCardCnt() < 1 || !pPlayer->canGangWithCard(nCard) )
			{
				msgBack["ret"] = 2 ;
				m_pRoom->sendMsgToPlayer(msgBack,MSG_PLAYER_ACT,nSessionID) ;
				return true ;
			}
		}
		break ;
	default:
		msgBack["ret"] = 2 ;
		m_pRoom->sendMsgToPlayer(msgBack,MSG_PLAYER_ACT,nSessionID) ;
		return true ;
	}

	stPlayerActTypeActionItem* pActTypeItem = new stPlayerActTypeActionItem ;
	pActTypeItem->nActType = nActType ;
	pActTypeItem->nActIdx = pPlayer->getIdx() ;
	pActTypeItem->nCardNumber = nCard ;
	pActTypeItem->nExePrio = nActType ;
	responeWaitAct(pActTypeItem->nActIdx,pActTypeItem);
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
			pRoom->onPlayerHuPai(pdoAct->nActIdx,m_tInfo.nCardNumber,m_tInfo.nCardProvideIdx,m_tInfo.isBuGang,m_tInfo.isCardFromGang);
		}
		break ;
	case eMJAct_MingGang:
		{
			pRoom->onPlayerGangPai(pdoAct->nActIdx,m_tInfo.nCardNumber,false ) ;
		}
		break ;
	case eMJAct_Peng:
		{
			pRoom->onPlayerPeng(pdoAct->nActIdx,pdoAct->nCardNumber);
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
			pTargeState->addWaitingTarget(m_nCurIdx) ;
			m_pRoom->goToState(pTargeState) ;
		}
		break;
	default:
		break ;
	}
}