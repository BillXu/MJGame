#pragma once
#include "ISeverApp.h"
#include "Singleton.h"
#include "RoomConfig.h"
#include "MJServerRoomManger.h"
class CMJServerApp
	:public IServerApp
	,public CSingleton<CMJServerApp>
{
public:
	CMJServerApp(){}
	bool init()override;
	uint16_t getLocalSvrMsgPortType() override ;
	CRoomConfigMgr* getRoomConfigMgr(){ return &m_tMgr ;}
protected:
	CRoomConfigMgr m_tMgr ;
};