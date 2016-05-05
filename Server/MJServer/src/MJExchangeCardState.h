#pragma once ;
#include "IRoomState.h"
#include "MJRoom.h"
#include "MJRoomPlayer.h"

struct stExchangeCardActionItem
	:public stActionItem
{
	uint8_t vExchangeCard[3] ;
};

class CMJWaitExchangeCardState
	:public IWaitingState
{
public:
	void enterState(IRoom* pRoom);
	void onWaitEnd( bool bTimeOut )override ;
	bool onMsg(Json::Value& prealMsg ,uint16_t nMsgType, eMsgPort eSenderPort , uint32_t nSessionID);
	uint16_t getStateID(){ return eRoomState_WaitExchangeCards ; }
};

class CMJDoExchangeCardState
	:public IExecuingState
{
public:
	void enterState(IRoom* pRoom)override ;
	void onExecuteOver()override;
	void doExecuteAct( stActionItem* pAct)override{ }
	uint16_t getStateID(){ return eRoomState_DoExchangeCards ; }
};
