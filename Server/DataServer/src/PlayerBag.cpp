#include "PlayerBag.h"
#include <ctime>
#include "LogManager.h"
#include "Player.h"
CPlayerBag::CPlayerBag(CPlayer* pPlayer):IPlayerComponent(pPlayer)
{
	m_eType = ePlayerComponet_Bag ;
	clearItems();
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
			for ( auto pItem : m_vAllItems )
			{
				Json::Value jsItem ;
				jsItem["itemID"] = pItem->nItemID;
				jsItem["cnt"] = pItem->nCnt ;
				jsItem["buyTime"] = pItem->nBuyTime ;
				jsItems[jsItems.size()] = jsItem;
			}
			jsmsg["items"] = jsItems ;
			SendMsg(jsmsg,nmsgType);
			CLogMgr::SharedLogMgr()->PrintLog("send bag msg to players uid = %u",GetPlayer()->GetUserUID()) ;
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
}

void CPlayerBag::Init()
{
	clearItems();
}

void CPlayerBag::addPlayerItem(uint32_t nItemID , bool isStack , uint32_t nCnt ,bool isNewAdd )
{
	if ( isStack )
	{
		auto pItem = getPlayerItem(nItemID) ;
		if ( pItem == nullptr )
		{
			pItem = new CPlayerBag::stPlayerItem;
			pItem->nBuyTime = (uint32_t)time(nullptr) ;
			pItem->nCnt = nCnt ;
			pItem->nItemID = nItemID ;
			m_vAllItems.push_back(pItem) ;

			// save to db ;
		}
		else
		{
			pItem->nCnt += nCnt ;
			// save to db ;
		}
	}
	else
	{
		auto pItem = new CPlayerBag::stPlayerItem;
		pItem->nBuyTime = (uint32_t)time(nullptr) ;
		pItem->nCnt = nCnt ;
		pItem->nItemID = nItemID ;
		m_vAllItems.push_back(pItem) ;

		// save to db ;
	}

	if ( isNewAdd )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("you should save to DB") ;
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