#pragma once 
#include "IMJRoomState.h"
#include "IMJRoom.h"
class MJRoomStateGameEnd
	:public IMJRoomState
{
public:
	uint32_t getStateID(){ return eRoomState_GameEnd; }
	void enterRoom(IMJRoom* pmjRoom, Json::Value& jsTranData)
	{
		IMJRoomState::enterRoom(pmjRoom, jsTranData);
		getRoom()->onGameEnd();
		setStateDuringTime(eTime_GameOver);
	}

	void onStateTimeUp()
	{
		getRoom()->onGameDidEnd();
		getRoom()->goToState(eRoomSate_WaitReady);
	}
};




