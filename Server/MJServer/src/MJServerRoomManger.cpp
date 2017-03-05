#include "MJServerRoomManger.h"
#include "MJServer.h"
#include "log4z.h"
#define ROOM_LIST_ITEM_CNT_PER_PAGE 5 
#include "AutoBuffer.h"
#include "SystemRoom.h"
#include "PrivateRoom.h"
#include "MJRoom.h"
#include "MJRoomBattleEnd.h"
#include "NewMJRoom.h"
void CMJRoomManager::init(IServerApp* svrApp)
{
	IRoomManager::init(svrApp);
}

IRoomInterface* CMJRoomManager::doCreateInitedRoomObject(uint32_t nRoomID,bool isPrivateRoom , uint16_t nRoomConfigID ,eRoomType reqSubRoomType, Json::Value& vJsValue ) 
{
	auto pConfig = CMJServerApp::getInstance()->getRoomConfigMgr()->GetConfigByConfigID(nRoomConfigID) ;
	if ( pConfig == nullptr && isPrivateRoom == false )
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
	//LOGFMTE("always create er ren queshen , temp") ;
	//return pRoom = new CNewMJRoom() ;
	switch (reqSubRoomType)
	{
	case eRoom_MJ_Blood_River:
		if ( !isPrivateRoom )
		{

			pRoom = new CMJRoom() ;
		}
		else
		{
			pRoom = new CPrivateRoom<CMJRoom> ;
		}
		break;
	case eRoom_MJ_Blood_End:
		if ( !isPrivateRoom )
		{

			pRoom = new CMJRoomBattleEnd() ;
		}
		else
		{
			pRoom = new CPrivateRoom<CMJRoomBattleEnd> ;
		}
		break;
	case eRoom_MJ_Two_Bird_God:
		if ( !isPrivateRoom )
		{

			pRoom = new CNewMJRoom() ;
		}
		else
		{
			pRoom = new CPrivateRoom<CNewMJRoom> ;
		}
		break;
	default:
		LOGFMTE("unknown room type = %u, so can not creat room ",reqSubRoomType);
	}
	return pRoom ;
}
