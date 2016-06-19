#pragma once
#include "IRoomManager.h"
class CMJRoomManager
	:public IRoomManager
{
public:
	CMJRoomManager(CRoomConfigMgr* pCongig ):IRoomManager(pCongig){ }
	void init( IServerApp* svrApp )override; 
	eRoomType getMgrRoomType()override{ return eRoom_MJ ;}
protected:
	IRoomInterface* doCreateInitedRoomObject(uint32_t nRoomID ,bool isPrivateRoom, uint16_t nRoomConfigID ,eRoomType reqSubRoomType, Json::Value& vJsValue)override ;
	IRoomInterface* doCreateRoomObject(eRoomType reqSubRoomType,bool isPrivateRoom)override ;
};