#pragma once
#include "IRoomState.h"
class CWaitStartState
	:public IRoomState
{
public:
	void update(float fDeta)override ;
	uint16_t getStateID(){ return eRoomSate_WaitReady ;}
};