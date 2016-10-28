#include "PlateConfig.h"
#include "log4z.h"
bool CPlateConfigMgr::OnPaser(CReaderRow& refReaderRow )
{
	stPlateItem* pItem = new stPlateItem ;
	pItem->ePlateItemType = (eShopItemType)refReaderRow["plateType"]->IntValue() ;
	pItem->nConfigID = refReaderRow["configID"]->IntValue() ;

	bool isCharge = refReaderRow["isCharge"]->IntValue();

	if ( GetPlateItem(pItem->nConfigID,!isCharge ) )
	{
		delete pItem ;
		pItem = NULL ;
		LOGFMTE("have two shop id the same") ;
		return false;
	}

	if ( isCharge )
	{
		m_vChargePlateItems.push_back(pItem) ;
	}
	else
	{
		m_vFreePlateItems.push_back(pItem) ;
	}
	
	pItem->nCount = refReaderRow["count"]->IntValue() ;
	pItem->nItemID = refReaderRow["itemID"]->IntValue() ;
	pItem->nRate = refReaderRow["rate"]->IntValue() ;
	return true ;
}

stPlateItem* CPlateConfigMgr::GetPlateItem(unsigned int nConfigID , bool isFree )
{
	auto vPlateItems = isFree ? m_vFreePlateItems : m_vChargePlateItems ;
	for ( auto pitem : vPlateItems )
	{
		if ( pitem->nConfigID == nConfigID )
		{
			return pitem ;
		}
	}
	return nullptr ;
}

stPlateItem* CPlateConfigMgr::randPlateItem( bool isFree )
{
	auto vPlateItems = isFree ? m_vFreePlateItems : m_vChargePlateItems ;
	uint32_t nRand = rand() % 10000 + 1 ;
	uint16_t nCheckRate = 0 ;
	uint16_t nIdx = rand() % vPlateItems.size() ;
	for ( ; nIdx < vPlateItems.size() * 2 ; ++nIdx )
	{
		uint16_t nRealIdx = nIdx % vPlateItems.size() ;
		auto pItem = vPlateItems[nRealIdx] ;
		nCheckRate += pItem->nRate ;
		if ( nRand <= nCheckRate )
		{
			return pItem ;
		}
	}
	LOGFMTE("why no rand target plate nRand = %u",nRand) ;
	return vPlateItems.front() ;
}

void CPlateConfigMgr::Clear()
{
	for ( auto pitem : m_vFreePlateItems )
	{
		delete pitem ;
		pitem = nullptr ;
	}
	m_vFreePlateItems.clear() ;

	for ( auto pitem : m_vChargePlateItems )
	{
		delete pitem ;
		pitem = nullptr ;
	}
	m_vChargePlateItems.clear() ;
}