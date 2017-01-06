#pragma once
#include "MJRoomStateStartGame.h"
class WZMJRoomStateStartGame
	:public MJRoomStateStartGame
{
public:
	void onStateTimeUp()
	{
		Json::Value jsValue;
		jsValue["idx"] = getRoom()->getBankerIdx();
		getRoom()->goToState(eRoomState_WaitBankInviteBuyDi, &jsValue);
	}
};