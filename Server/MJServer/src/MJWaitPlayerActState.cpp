#include "MJWaitPlayerActState.h"
#include "MJDefine.h"
#include "LogManager.h"
// wait player act 
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
	responeWaitAct(pActTypeItem->nActIdx,pActTypeItem);
	return true ;
}

// do player act 
void CMJDoPlayerActState::onExecuteOver()
{
	switch ( m_edoAct )
	{
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
			pActTypeItem->nActIdx = pRoom->getNextActPlayerIdx() ;
			pActTypeItem->nCardNumber = 0 ;
			VEC_ACTITEM v ;
			v.push_back(pActTypeItem) ;
			pTargeState->setExecuteActs(v) ;

			m_pRoom->goToState(pTargeState) ;
		}
		break ;
	case eMJAct_BuGang:
		{
			if ( m_vecCardPlayerIdxs.empty() )
			{
				auto pTargeState = (IWaitingState*)m_pRoom->getRoomStateByID(eRoomState_WaitPlayerAct) ;
				pTargeState->setWaitTime(eTime_WaitPlayerAct) ;
				auto pRoom = (CMJRoom*)m_pRoom ;
				pTargeState->addWaitingTarget(m_nCurIdx) ;
				m_pRoom->goToState(pTargeState) ;
			}
			else
			{
				auto pTargeState = (IWaitingState*)m_pRoom->getRoomStateByID(eRoomState_WaitOtherPlayerAct) ;
				pTargeState->setWaitTime(eTime_WaitPlayerAct) ;
				auto pRoom = (CMJRoom*)m_pRoom ;
				for ( auto ref : m_vecCardPlayerIdxs )
				{
					pTargeState->addWaitingTarget(ref) ;
				}
				m_pRoom->goToState(pTargeState) ;
			}
		}
		break ;
	default:
		break ;
	}
}

void CMJDoPlayerActState::doExecuteAct( stActionItem* pAct)
{
	stPlayerActTypeActionItem* pdoAct = (stPlayerActTypeActionItem*)pAct ;
	m_edoAct = (eMJActType)pdoAct->nActType ;

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
			pRoom->onPlayerHuPai(pdoAct->nActIdx,pdoAct->nCardNumber,pdoAct->nActIdx,pdoAct->eCardFrom);
		}
		break ;
	case eMJAct_Chu:
		{
			pRoom->onPlayerGiveCardToTable(pdoAct->nActIdx,eMJAct_Chu,pdoAct->nCardNumber,pdoAct->eCardFrom) ;
			pRoom->checkPlayersNeedTheCard(pdoAct->nCardNumber,m_vecCardPlayerIdxs,pdoAct->nActIdx) ;
		}
		break ;
	case eMJAct_BuGang:
		{
			if ( pRoom->checkPlayersNeedTheCard(pdoAct->nCardNumber,m_vecCardPlayerIdxs,pdoAct->nActIdx) )
			{
				pRoom->onPlayerGiveCardToTable(pdoAct->nActIdx,eMJAct_BuGang,pdoAct->nCardNumber,eMJAct_Mo) ;
			}
			else
			{
				pRoom->onPlayerGangPai(pdoAct->nActIdx,true) ;
			}
		}
		break ;
	case eMJAct_AnGang:
		{
			pRoom->onPlayerGangPai(pdoAct->nActIdx,false ) ;
		}
		break ;
	default:
		break ;
	}
}

// wait other player act 
void CMJWaitOtherActState::onWaitEnd( bool bTimeOut )
{

}	

bool CMJWaitOtherActState::onMsg(Json::Value& prealMsg ,uint16_t nMsgType, eMsgPort eSenderPort , uint32_t nSessionID)
{
	return false ;
}

// do other player act 
void CMJDoOtherPlayerActState::onExecuteOver()
{

}

void CMJDoOtherPlayerActState::doExecuteAct( stActionItem* pAct)
{

}