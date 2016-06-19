#include "PlateConfig.h"
#include "LogManager.h"
bool CPlateConfigMgr::OnPaser(CReaderRow& refReaderRow )
{
	stPlateItem* pItem = new stPlateItem ;
	pItem->ePlateItemType = (eShopItemType)refReaderRow["plateType"]->IntValue() ;
	pItem->nConfigID = refReaderRow["configID"]->IntValue() ;
	if ( GetPlateItem(pItem->nConfigID) )
	{
		delete pItem ;
		pItem = NULL ;
		CLogMgr::SharedLogMgr()->ErrorLog("have two shop id the same") ;
		return false;
	}
	m_vAllPlateItems.push_back(pItem) ;
	pItem->nCount = refReaderRow["count"]->IntValue() ;
	pItem->nItemID = refReaderRow["itemID"]->IntValue() ;
	pItem->nRate = refReaderRow["rate"]->IntValue() ;
	return true ;
}

stPlateItem* CPlateConfigMgr::GetPlateItem(unsigned int nConfigID )
{
	for ( auto pitem : m_vAllPlateItems )
	{
		if ( pitem->nConfigID == nConfigID )
		{
			return pitem ;
		}
	}
	return nullptr ;
}

stPlateItem* CPlateConfigMgr::randPlateItem()
{
	uint32_t nRand = rand() % 10000 + 1 ;
	uint16_t nCheckRate = 0 ;
	uint16_t nIdx = rand() % m_vAllPlateItems.size() ;
	for ( ; nIdx < m_vAllPlateItems.size() * 2 ; ++nIdx )
	{
		uint16_t nRealIdx = nIdx % m_vAllPlateItems.size() ;
		auto pItem = m_vAllPlateItems[nRealIdx] ;
		nCheckRate += pItem->nRate ;
		if ( nRand <= nCheckRate )
		{
			return pItem ;
		}
	}
	CLogMgr::SharedLogMgr()->ErrorLog("why no rand target plate nRand = %u",nRand) ;
	return m_vAllPlateItems.front() ;
}

void CPlateConfigMgr::Clear()
{
	for ( auto pitem : m_vAllPlateItems )
	{
		delete pitem ;
		pitem = nullptr ;
	}
	m_vAllPlateItems.clear() ;
}