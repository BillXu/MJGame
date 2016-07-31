#include "GameOverState.h"
#include "ISitableRoom.h"
#include "LogManager.h"
void CGameOverState::enterState(IRoom* pRoom )
{
	IRoomState::enterState(pRoom) ;
	pRoom->onGameOver();
	setStateDuringTime(eTime_GameOver);
}

void CGameOverState::onStateDuringTimeUp()
{
	CLogMgr::SharedLogMgr()->PrintLog("game end ,did end ") ;
	this->m_pRoom->onGameDidEnd();
	CLogMgr::SharedLogMgr()->PrintLog("game end to wait ready sate") ;
	this->m_pRoom->goToState(eRoomSate_WaitReady) ;
}

uint16_t CGameOverState::getStateID()
{
	return eRoomState_GameEnd;
}