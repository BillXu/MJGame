#include "MJServerRoomManger.h"
#include "MJServer.h"
#include "LogManager.h"
#define ROOM_LIST_ITEM_CNT_PER_PAGE 5 
#include "AutoBuffer.h"
#include "SystemRoom.h"
#include "PrivateRoom.h"
IRoomInterface* CMJRoomManager::doCreateInitedRoomObject(uint32_t nRoomID,bool isPrivateRoom , uint16_t nRoomConfigID ,eRoomType reqSubRoomType, Json::Value& vJsValue ) 
{
	stSitableRoomConfig* pConfig = (stSitableRoomConfig*)CMJServerApp::getInstance()->getRoomConfigMgr()->GetConfigByConfigID(nRoomConfigID) ;
	if ( pConfig == nullptr )
	{
		return nullptr ;
	}

	IRoomInterface* pRoom = doCreateRoomObject(reqSubRoomType,isPrivateRoom) ;
	pRoom->onFirstBeCreated(this,pConfig,nRoomID,vJsValue);
	return pRoom ;
}

IRoomInterface* CMJRoomManager::doCreateRoomObject(eRoomType reqSubRoomType,bool isPrivateRoom)
{
	IRoomInterface* pRoom = nullptr ;
	if ( !isPrivateRoom )
	{
		//pRoom = new CSystemRoom<CNiuNiuRoom> ;
	}
	else
	{
		//pRoom = new CPrivateRoom<CNiuNiuRoom> ;
	}
	return pRoom ;
}
