#pragma once
#include "IRoomState.h"
#include "MJRoom.h"
#include "MJRoomPlayer.h"
#include "MessageIdentifer.h"
#include "MJExchangeCardState.h"
class CMJGameStartState
	:public IRoomState
{
public:
	void enterState(IRoom* pRoom)
	{
		IRoomState::enterState(pRoom) ; 
		m_pRoom->onGameWillBegin();
		setStateDuringTime(eTime_ExeGameStart) ;
	} 

	uint16_t getStateID() override { return eRoomState_StartGame ; }
	void onStateDuringTimeUp()override
	{
		auto pTargetState = (CMJWaitExchangeCardState*)m_pRoom->getRoomStateByID(eRoomState_WaitExchangeCards);

		pTargetState->setWaitTime(eTime_WaitChoseExchangeCard);
		pTargetState->addWaitingTarget(0);
		pTargetState->addWaitingTarget(1);
		pTargetState->addWaitingTarget(2);
		pTargetState->addWaitingTarget(3);

		m_pRoom->goToState(pTargetState) ;
	}
};