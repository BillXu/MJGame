#pragma once
#include "IRoomManager.h"
class CMJRoomManager
	:public IRoomManager
{
public:
	CMJRoomManager(CRoomConfigMgr* pCongig ):IRoomManager(pCongig){ }
	eRoomType getMgrRoomType()override{ return eRoom_NiuNiu ;}
protected:
	IRoomInterface* doCreateInitedRoomObject(uint32_t nRoomID ,bool isPrivateRoom, uint16_t nRoomConfigID ,eRoomType reqSubRoomType, Json::Value& vJsValue)override ;
	IRoomInterface* doCreateRoomObject(eRoomType reqSubRoomType,bool isPrivateRoom)override ;
};