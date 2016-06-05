#include "MJServerRoomManger.h"
#include "MJServer.h"
#include "LogManager.h"
#define ROOM_LIST_ITEM_CNT_PER_PAGE 5 
#include "AutoBuffer.h"
#include "SystemRoom.h"
#include "PrivateRoom.h"
#include "MJRoom.h"
IRoomInterface* CMJRoomManager::doCreateInitedRoomObject(uint32_t nRoomID,bool isPrivateRoom , uint16_t nRoomConfigID ,eRoomType reqSubRoomType, Json::Value& vJsValue ) 
{
	auto pConfig = CMJServerApp::getInstance()->getRoomConfigMgr()->GetConfigByConfigID(nRoomConfigID) ;
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
		pRoom = new CMJRoom ;
	}
	else
	{
		pRoom = new CPrivateRoom<CMJRoom> ;
	}
	return pRoom ;
}
