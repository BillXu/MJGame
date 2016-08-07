#pragma once
#include "IRoomState.h"
class CWaitPlayerAct
	:public IRoomState
{
public:
	void enterState(IRoom* pRoom,Json::Value& jsTransferData);
	void onStateDuringTimeUp()override;
	bool onMsg(Json::Value& prealMsg ,uint16_t nMsgType, eMsgPort eSenderPort , uint32_t nSessionID)override ;
	uint16_t getStateID(){ return eRoomState_WaitPlayerAct ;}
protected:
	bool doExcutingAct( eMJActType eAct, uint8_t nActCard );
	void startWaitChoseAct( uint8_t nPlayerIdx , bool isOnlyCanChu = false );
protected:
	bool m_isWaitingChoseAct ;  // is waiting player chose do act , or  executing m_ePlayerAct
	bool m_isOnlyCanChu ;
	uint8_t m_nCurPlayerIdx ;

	eMJActType m_eExecutingAct ;
	uint8_t m_nActCard ;
};