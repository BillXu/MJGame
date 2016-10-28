#include "PlayerBag.h"
#include <ctime>
#include "log4z.h"
#include "Player.h"
#include "GameServerApp.h"
#include "AsyncRequestQuene.h"
#include "ServerCommon.h"
#include <iostream>
#include <sstream>
CPlayerBag::CPlayerBag(CPlayer* pPlayer):IPlayerComponent(pPlayer)
{
	m_eType = ePlayerComponet_Bag ;
	clearItems();
	m_isReading = false ;
}

CPlayerBag::~CPlayerBag()
{
	clearItems();
}

bool CPlayerBag::OnMessage( Json::Value& recvValue , uint16_t nmsgType, eMsgPort eSenderPort)
{
	switch ( nmsgType )
	{
	case MSG_REQ_MY_BAG:
		{
			Json::Value jsmsg ;
			Json::Value jsItems ;
			uint32_t tNow = time(nullptr) ;
			for ( auto pItem : m_vAllItems )
			{
				Json::Value jsItem ;
				jsItem["itemID"] = pItem->nItemID;
				jsItem["cnt"] = pItem->nCnt ;
				jsItem["buyTime"] = pItem->nBuyTime ;
				
				uint32_t nLeftTime = 0 ;
				if ( tNow <  pItem->nDeadTime  )
				{
					nLeftTime = pItem->nDeadTime - tNow ;
				}
				jsItem["leftTime"] = nLeftTime ;
				jsItems[jsItems.size()] = jsItem;
			}
			jsmsg["items"] = jsItems ;
			SendMsg(jsmsg,nmsgType);
			LOGFMTD("send bag msg to players uid = %u size = %u",GetPlayer()->GetUserUID(),jsItems.size()) ;
		}
		break ;
	default:
		return false;
	}
	return true ;
}

void CPlayerBag::Reset()
{
	clearItems();
	readItems();
}

void CPlayerBag::Init()
{
	clearItems();
	readItems();
}

void CPlayerBag::readItems()
{
	m_isReading = true ;
	Json::Value jsReq ;
	char pBuffer[256] = { 0 } ;
	sprintf_s(pBuffer,sizeof(pBuffer),"select itemID,deadTime,buyTime from playeritems where userUID = '%u' limit 20 offset %u; ",GetPlayer()->GetUserUID(),m_vAllItems.size() ) ;
	jsReq["sql"] = pBuffer;
	CGameServerApp::SharedGameServerApp()->getAsynReqQueue()->pushAsyncRequest(ID_MSG_PORT_DB,eAsync_DB_Select,jsReq,[this](uint16_t nReqType ,const Json::Value& retContent,Json::Value& jsUserData){
	
		uint16_t nfact = retContent["afctRow"].asUInt() ;
		auto jsData = retContent["data"] ;
		for ( uint16_t nIdx = 0 ; nIdx < jsData.size() ; ++nIdx )
		{
			auto jsRow = jsData[nIdx] ;
			auto pItem = new stPlayerItem ;
			pItem->nBuyTime = jsRow["buyTime"].asUInt();
			pItem->nCnt = 1 ;
			pItem->nDeadTime = jsRow["deadTime"].asUInt() ;
			pItem->nItemID = jsRow["itemID"].asUInt() ;
			m_vAllItems.push_back(pItem) ;
		}

		if ( nfact >= 20 )
		{
			readItems();
		}
		else
		{
			m_isReading = false ;
			LOGFMTD("uid = %u finished read items = %u",GetPlayer()->GetUserUID(),m_vAllItems.size()) ;
		}
	});

}

void CPlayerBag::addPlayerItem(uint32_t nItemID , bool isStack , uint32_t nCnt ,bool isNewAdd )
{
	uint32_t nNow = (uint32_t)time(nullptr) ;
	stPlayerItem* pItem = nullptr ;
	bool isUpdate = false ;
	pItem = getPlayerItem(nItemID) ;
	if ( pItem == nullptr )
	{
		pItem = new CPlayerBag::stPlayerItem;
		pItem->nBuyTime = (uint32_t)time(nullptr) ;
		pItem->nCnt = 1 ;
		pItem->nItemID = nItemID ;
		pItem->nDeadTime = nCnt * 60*60*24 + nNow; 
		m_vAllItems.push_back(pItem) ;

		// save to db ;
	}
	else
	{
		pItem->nCnt = 1 ;
		if ( pItem->nDeadTime < nNow )
		{
			pItem->nDeadTime = nCnt * 60*60*24 + nNow; 
		}
		else
		{
			pItem->nDeadTime += ( nCnt * 60*60*24 ) ;
		}

		isUpdate = true ;
		// save to db ;
	}

	//if ( isStack )
	//{
	//	pItem = getPlayerItem(nItemID) ;
	//	if ( pItem == nullptr )
	//	{
	//		pItem = new CPlayerBag::stPlayerItem;
	//		pItem->nBuyTime = (uint32_t)time(nullptr) ;
	//		pItem->nCnt = 1 ;
	//		pItem->nItemID = nItemID ;
	//		pItem->nDeadTime = nCnt * 60*60*24 + nNow; 
	//		m_vAllItems.push_back(pItem) ;

	//		// save to db ;
	//	}
	//	else
	//	{
	//		pItem->nCnt = 1 ;
	//		if ( pItem->nDeadTime < nNow )
	//		{
	//			pItem->nDeadTime = nCnt * 60*60*24 + nNow; 
	//		}
	//		else
	//		{
	//			pItem->nDeadTime += ( nCnt * 60*60*24 ) ;
	//		}

	//		isUpdate = true ;
	//		// save to db ;
	//	}
	//}
	//else
	//{
	//	pItem = new CPlayerBag::stPlayerItem;
	//	pItem->nBuyTime = nNow ;
	//	pItem->nCnt = 1 ;
	//	pItem->nItemID = nItemID ;
	//	pItem->nDeadTime = nCnt * 60*60*24 + nNow; 
	//	m_vAllItems.push_back(pItem) ;

	//	// save to db ;
	//}

	if ( !isNewAdd )
	{
		LOGFMTD("not new add so do no do db operate") ;
		return ;
	}

	if ( !isUpdate )
	{
		Json::Value jsSql ;
		char pBuffer[256] = { 0 } ;
		sprintf_s(pBuffer,"insert into playeritems (userUID,itemID,buyTime,deadTime) values ( '%u','%u','%u','%u');",GetPlayer()->GetUserUID(),pItem->nItemID,pItem->nBuyTime,pItem->nDeadTime);
		
		jsSql["sql"] = pBuffer;
		CGameServerApp::SharedGameServerApp()->getAsynReqQueue()->pushAsyncRequest(ID_MSG_PORT_DB,eAsync_DB_Add,jsSql);
		LOGFMTD("add item sql = %s",jsSql["sql"].asCString()) ;
	}
	else
	{
		Json::Value jsSql ;
		std::ostringstream strStream ;
		strStream << "update playeritems set deadTime = ' " << pItem->nDeadTime << " ' where userUID = ' " << GetPlayer()->GetUserUID() << " ' and itemID = ' " << pItem->nItemID << " ' ;"  ;
		jsSql["sql"] = strStream.str().c_str();
		LOGFMTD("update item sql = %s",jsSql["sql"].asCString()) ;
		CGameServerApp::SharedGameServerApp()->getAsynReqQueue()->pushAsyncRequest(ID_MSG_PORT_DB,eAsync_DB_Update,jsSql);
	}
}

CPlayerBag::stPlayerItem* CPlayerBag::getPlayerItem(uint32_t nItemID )
{
	for ( auto item : m_vAllItems )
	{
		if ( item->nItemID == nItemID )
		{
			return item ;
		}
	}
	return nullptr ;
}

uint32_t CPlayerBag::getPlayerItemCnt(uint32_t nItemID )
{
	uint32_t nCnt = 0 ;
	for ( auto item : m_vAllItems )
	{
		if ( item->nItemID == nItemID )
		{
			nCnt += item->nCnt ;
		}
	}
	return nCnt ;
}

bool CPlayerBag::removePlayerItem(uint32_t nItemID  , uint32_t nCnt )
{
	if ( nCnt > getPlayerItemCnt(nItemID) )
	{
		return false ;
	}

	uint32_t needRemove = nCnt ;
	for ( auto& item : m_vAllItems )
	{
		if ( item->nItemID == nItemID )
		{
			if ( item->nCnt <= needRemove )
			{
				needRemove -= item->nCnt ;
				item->nCnt = 0 ;
			}
			else
			{
				needRemove = 0 ;
				item->nCnt -= needRemove ;
			}

			if ( needRemove == 0 )
			{
				break; ;
			}
		}
	}

	auto iter = m_vAllItems.begin();
	while ( iter != m_vAllItems.end() )
	{
		if ( (*iter)->nCnt <= 0 )
		{
			delete (*iter) ;
			(*iter) = nullptr ;
			iter = m_vAllItems.begin();
			continue;
		}
		++iter ;
	}

	return false ;
}

void CPlayerBag::SendListToClient()
{

}

void CPlayerBag::clearItems()
{
	for ( auto item : m_vAllItems )
	{
		delete item ;
		item = nullptr ;
	}
	m_vAllItems.clear() ;
}