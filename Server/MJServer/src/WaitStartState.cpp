#include "WaitStartState.h"
#include "ISitableRoom.h"
#include "log4z.h"
void CWaitStartState::update(float fDeta)
{
	if ( m_pRoom->canStartGame() )
	{
		LOGFMTD("enter start game state") ;
		m_pRoom->goToState(eRoomState_StartGame);
	}
}