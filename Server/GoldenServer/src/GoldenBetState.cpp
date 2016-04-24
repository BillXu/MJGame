#include "GoldenBetState.h"
#include "GoldenRoom.h"
#include "GoldenMessageDefine.h"
#include "LogManager.h"
#include "GoldenRoomPlayer.h"
#include <cassert>
void CGoldenBetState::enterState(IRoom* pRoom)
{
	m_pRoom = (CGoldenRoom*)pRoom ;

	setStateDuringTime(TIME_GOLDEN_ROOM_WAIT_ACT);
}

void CGoldenBetState::onStateDuringTimeUp() 
{
	// time out 
	stMsgGoldenPlayerAct act ;
	act.nPlayerAct = (uint8_t)eRoomPeerAction_GiveUp ;
	act.nValue = 0 ;
	auto pp = m_pRoom->getPlayerByIdx(m_pRoom->getCurActIdx()) ;
	assert(pp && "why current act player is null ?" );
	CLogMgr::SharedLogMgr()->PrintLog("player not act so give up idx = %u",m_pRoom->getCurActIdx()) ;
	onMessage(&act,ID_MSG_PORT_CLIENT,pp->getSessionID());
}

bool CGoldenBetState::onMessage( stMsg* prealMsg , eMsgPort eSenderPort , uint32_t nPlayerSessionID )
{
	switch (prealMsg->usMsgType)
	{
	case MSG_GOLDEN_PLAYER_PK:
		{
			stMsgGoldenPlayerPK* pRet = (stMsgGoldenPlayerPK*)prealMsg ;
			stMsgGoldenPlayerPKRet msgBack ;
			auto pActPlayer = m_pRoom->getSitdownPlayerBySessionID(nPlayerSessionID) ;
			if ( pActPlayer == nullptr )
			{
				msgBack.cRet = 3 ;
				m_pRoom->sendMsgToPlayer(&msgBack,sizeof(msgBack),nPlayerSessionID) ;
				break ;
			}

			if ( pActPlayer->getIdx() != m_pRoom->getCurActIdx() )
			{
				msgBack.cRet = 1 ;
				m_pRoom->sendMsgToPlayer(&msgBack,sizeof(msgBack),nPlayerSessionID) ;
				break ;
			}

			auto pTargetPlayer = m_pRoom->getPlayerByIdx(pRet->nPkTargetIdx);
			if ( pTargetPlayer == nullptr || pTargetPlayer->isHaveState(eRoomPeer_CanAct) == false )
			{
				msgBack.cRet = 2 ;
				m_pRoom->sendMsgToPlayer(&msgBack,sizeof(msgBack),nPlayerSessionID) ;
				break ;
			}

			stMsgGoldenRoomPK msgPK ;
			msgPK.nActPlayerIdx = pActPlayer->getIdx() ;
			msgPK.nTargetIdx = pTargetPlayer->getIdx() ;
			msgPK.bWin = m_pRoom->onPlayerPK(pActPlayer,pTargetPlayer) ;
			m_pRoom->sendRoomMsg(&msgPK,sizeof(msgPK)) ;

			m_pRoom->goToState(eRoomState_Golden_PK) ;
		}
		break;
	case MSG_GOLDEN_PLAYER_ACT:
		{
			stMsgGoldenPlayerActRet msgBack ;
			msgBack.nRet = 0 ;
			auto pPlayer = m_pRoom->getSitdownPlayerBySessionID(nPlayerSessionID) ;
			if ( pPlayer == nullptr )
			{
				CLogMgr::SharedLogMgr()->ErrorLog("session id = %u not in this room do act room id = %u",nPlayerSessionID,m_pRoom->getRoomID()) ;
				msgBack.nRet = 2 ;
				m_pRoom->sendMsgToPlayer(&msgBack,sizeof(msgBack),nPlayerSessionID) ;
				break ;
			}

			stMsgGoldenPlayerAct* pRet = (stMsgGoldenPlayerAct*)prealMsg ;
			bool bNeedWaitNext = pPlayer->getIdx() == m_pRoom->getCurActIdx() && (eRoomPeerAction_ViewCard != pRet->nPlayerAct) ;
			msgBack.nRet = m_pRoom->onPlayerAction(pRet->nPlayerAct,pRet->nValue,pPlayer);
			m_pRoom->sendMsgToPlayer(&msgBack,sizeof(msgBack),nPlayerSessionID) ;

			if ( msgBack.nRet == 0 )
			{
				// tell other 
				stMsgGoldenRoomAct msgAct ;
				msgAct.nPlayerAct = pRet->nPlayerAct ;
				msgAct.nPlayerIdx = pPlayer->getIdx() ;
				msgAct.nValue = pRet->nValue ;
				m_pRoom->sendRoomMsg(&msgAct,sizeof(msgAct)) ;

				if ( bNeedWaitNext && m_pRoom->getPlayerCntWithState(eRoomPeer_CanAct) > 1 )
				{
					m_pRoom->informPlayerAct(true);
					setStateDuringTime(TIME_GOLDEN_ROOM_WAIT_ACT);
				}
			}
			// game over ?
			if ( m_pRoom->getPlayerCntWithState(eRoomPeer_CanAct) <= 1 )
			{
				m_pRoom->goToState(eRoomState_Golden_GameResult) ;
				break ;
			}
		}
		break;
	default:
		return false;
	}
	return true ;
}