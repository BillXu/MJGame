#pragma once ;
#include "IRoomState.h"
#include "MJRoom.h"
#include "MJRoomPlayer.h"

struct stPlayerActTypeActionItem
	:public stActionItem
{
	uint8_t nActType;
	uint8_t nCardNumber ;
	eMJActType eCardFrom ;
};

class CMJWaitPlayerActState
	:public IWaitingState
{
public:
	void onWaitEnd( bool bTimeOut )override ;
	bool onMsg(Json::Value& prealMsg ,uint16_t nMsgType, eMsgPort eSenderPort , uint32_t nSessionID);
	uint16_t getStateID(){ return eRoomState_WaitPlayerAct ; }
};

class CMJDoPlayerActState
	:public IExecuingState
{
public:
	void onExecuteOver()override;
	void doExecuteAct( stActionItem* pAct);
	uint16_t getStateID(){ return eRoomState_DoPlayerAct ; }
protected:
	eMJActType m_edoAct ;
	uint8_t m_nCurIdx ;
	std::vector<uint8_t> m_vecCardPlayerIdxs ;
};

// other player 
class CMJWaitOtherActState
	:public IWaitingState
{
public:
	void onWaitEnd( bool bTimeOut )override ;
	bool onMsg(Json::Value& prealMsg ,uint16_t nMsgType, eMsgPort eSenderPort , uint32_t nSessionID);
	uint16_t getStateID(){ return eRoomState_WaitOtherPlayerAct ; }
};

class CMJDoOtherPlayerActState
	:public IExecuingState
{
public:
	void onExecuteOver()override;
	void doExecuteAct( stActionItem* pAct);
	uint16_t getStateID(){ return eRoomState_DoOtherPlayerAct ; }
};

