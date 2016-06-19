#include "MJServerRoomManger.h"
#include "MJServer.h"
#include "LogManager.h"
#define ROOM_LIST_ITEM_CNT_PER_PAGE 5 
#include "AutoBuffer.h"
#include "SystemRoom.h"
#include "PrivateRoom.h"
#include "MJRoom.h"
#include "MJRoomBattleEnd.h"

void CMJRoomManager::init(IServerApp* svrApp)
{
	IRoomManager::init(svrApp);
	auto iter = m_pConfigMgr->GetBeginIter() ;
	for ( ; iter != m_pConfigMgr->GetEndIter(); ++iter )
	{
		uint8_t nCreaeCnt = 3 ;
		while ( nCreaeCnt-- )
		{
			Json::Value vDefault ;
			IRoomInterface* pRoom = doCreateInitedRoomObject(++m_nMaxRoomID,false,(*iter)->nConfigID,(eRoomType)(*iter)->nGameType,vDefault);
			addRoomToSystem(pRoom) ;
		}
		CLogMgr::SharedLogMgr()->PrintLog("system crate five room config id = %u , roomType = %u",(*iter)->nConfigID,(*iter)->nGameType ) ;
	}
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
	switch (reqSubRoomType)
	{
	case eRoom_MJ_Blood_River:
		if ( !isPrivateRoom )
		{

			pRoom = new CMJRoom ;
		}
		else
		{
			pRoom = new CPrivateRoom<CMJRoom> ;
		}
		break;
	case eRoom_MJ_Blood_End:
		if ( !isPrivateRoom )
		{

			pRoom = new CMJRoomBattleEnd ;
		}
		else
		{
			pRoom = new CPrivateRoom<CMJRoomBattleEnd> ;
		}
		break;
	default:
		CLogMgr::SharedLogMgr()->ErrorLog("unknown room type = %u, so can not creat room ",reqSubRoomType);
	}
	return pRoom ;
}
