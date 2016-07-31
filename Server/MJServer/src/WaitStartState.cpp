#include "WaitStartState.h"
#include "ISitableRoom.h"
#include "LogManager.h"
void CWaitStartState::update(float fDeta)
{
	if ( m_pRoom->canStartGame() )
	{
		CLogMgr::SharedLogMgr()->PrintLog("enter start game state") ;
		m_pRoom->goToState(eRoomState_StartGame);
	}
}