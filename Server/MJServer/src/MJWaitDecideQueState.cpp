#include "MJWaitDecideQueState.h"
#include "LogManager.h"
// wait 

void CMJWaitDecideQueState::enterState(IRoom* pRoom)
{
	IWaitingState::enterState(pRoom) ;
	Json::Value msg ;
	pRoom->sendRoomMsg(msg,MSG_ROOM_WAIT_DECIDE_QUE);
}

void CMJWaitDecideQueState::onWaitEnd( bool bTimeOut )
{
	auto pSitRoom =  (ISitableRoom*)m_pRoom ;
	if ( bTimeOut )
	{
		VEC_WAIT_IDX vWait ( m_vWaitIdxs ) ;
		for ( auto ref : vWait )
		{
			auto pp = (CMJRoomPlayer*)pSitRoom->getPlayerByIdx(ref.nIdx) ;

			stQueTypeActionItem* p = new stQueTypeActionItem ;
			p->nActIdx = pp->getIdx() ;
			p->nType = rand() % 3 + 1 ;
			pp->setMustQueType(p->nType) ;
			responeWaitAct(p->nActIdx,p) ;
		}
	}
	else
	{
		CLogMgr::SharedLogMgr()->PrintLog("enter to do decide que ") ;
		auto pTargeState = (IExecuingState*)m_pRoom->getRoomStateByID(eRoomState_DoDecideQue) ;
		pTargeState->setExecuteTime(eTime_DoDecideQue) ;
		pTargeState->setExecuteActs(m_vActList) ;
		m_pRoom->goToState(pTargeState) ;
	}
}

bool CMJWaitDecideQueState::onMsg(Json::Value& prealMsg ,uint16_t nMsgType, eMsgPort eSenderPort , uint32_t nSessionID)
{
	if ( MSG_PLAYER_DECIDE_QUE != nMsgType )
	{
		return false ;
	}

	auto pp = (CMJRoomPlayer*)((ISitableRoom*)m_pRoom)->getSitdownPlayerBySessionID(nSessionID) ; 
	if ( pp && isIdxInWaitList(pp->getIdx()) )
	{
		CLogMgr::SharedLogMgr()->PrintLog("player decide que id = %u",pp->getIdx()) ;
		stQueTypeActionItem* pE = new stQueTypeActionItem ;
		pE->nActIdx = pp->getIdx();
		pE->nType = prealMsg["type"].asUInt();

		if ( pE->nType <= eCT_None || pE->nType > eCT_Tiao )
		{
			CLogMgr::SharedLogMgr()->ErrorLog("idx = %u ,decide invalid que type = %u",pp->getIdx(),pE->nType) ;
			delete pE ;
			pE = nullptr ;
			return true ;
		}
		pp->setMustQueType(pE->nType) ;
		CLogMgr::SharedLogMgr()->PrintLog("idx = %u , que Type : %u",pp->getIdx(),pE->nType);
		responeWaitAct(pE->nActIdx,pE) ;
	}
	else
	{
		CLogMgr::SharedLogMgr()->ErrorLog("you are not in wait act list for decide que session id = %u",nSessionID) ;
	}
	return true ;
}

// do act 
void CMJDoDecideQueState::enterState(IRoom* pRoom)
{
	CLogMgr::SharedLogMgr()->PrintLog("finish Que : ") ;
	Json::Value msg ;
	Json::Value arrayType ;
	for ( auto ref : m_vActList )
	{
		Json::Value jsPlayer ;
		stQueTypeActionItem* pD = (stQueTypeActionItem*)ref ;
		jsPlayer["idx"] = pD->nActIdx ;
		jsPlayer["type"] = pD->nType ;
		arrayType[ref->nActIdx] = jsPlayer;
		CLogMgr::SharedLogMgr()->PrintLog("idx = %u , que Type : %u",pD->nActIdx,pD->nType);
	}

	msg["ret"] = arrayType ;
	pRoom->sendRoomMsg(msg,MSG_ROOM_FINISH_DECIDE_QUE) ;
	IExecuingState::enterState(pRoom) ;
}

void CMJDoDecideQueState::onExecuteOver()
{
	auto pTargeState = (IWaitingState*)m_pRoom->getRoomStateByID(eRoomState_WaitPlayerAct) ;
	pTargeState->setWaitTime(eTime_WaitPlayerAct) ;
	auto pRoom = (CMJRoom*)m_pRoom ;
	CLogMgr::SharedLogMgr()->PrintLog("after decide que , enter wait player act banker id = %u",pRoom->getBankerIdx()) ;
	pTargeState->addWaitingTarget(pRoom->getBankerIdx()) ;
	m_pRoom->goToState(pTargeState) ;
}

void CMJDoDecideQueState::doExecuteAct( stActionItem* pAct)
{
	// do nothing ;
}
