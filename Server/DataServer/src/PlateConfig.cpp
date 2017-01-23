#include "PlateConfig.h"
#include "log4z.h"
bool CPlateConfigMgr::OnPaser(CReaderRow& refReaderRow )
{
	stPlateItem* pItem = new stPlateItem ;
	pItem->ePlateItemType = (eShopItemType)refReaderRow["plateType"]->IntValue() ;
	pItem->nConfigID = refReaderRow["configID"]->IntValue() ;

	uint32_t nPrice = refReaderRow["isCharge"]->IntValue();
	uint8_t nTimes = refReaderRow["times"]->IntValue();

	pItem->nCount = refReaderRow["count"]->IntValue();
	pItem->nItemID = refReaderRow["itemID"]->IntValue();
	pItem->nRate = refReaderRow["rate"]->IntValue();

	if (GetPlateItem(pItem->nConfigID, nTimes))
	{
		delete pItem ;
		pItem = NULL ;
		LOGFMTE("have two shop id the same") ;
		return false;
	}

	auto iter = m_vPlateGroup.find(nTimes);
	if (iter == m_vPlateGroup.end())
	{
		stPlateGroup st;
		st.nPrice = nPrice;
		st.nTimes = nTimes;
		st.vPlateItems.push_back(pItem);
		m_vPlateGroup[nTimes] = st;
	}
	else
	{
		iter->second.vPlateItems.push_back(pItem);
	}

	return true ;
}

stPlateItem* CPlateConfigMgr::GetPlateItem(unsigned int nConfigID , uint8_t nTimes )
{
	auto iterGroup = m_vPlateGroup.find(nTimes);
	if (iterGroup == m_vPlateGroup.end())
	{
		return nullptr;
	}

	auto& vPlateItems = iterGroup->second.vPlateItems;
	for ( auto pitem : vPlateItems )
	{
		if ( pitem->nConfigID == nConfigID )
		{
			return pitem ;
		}
	}
	return nullptr ;
}

stPlateItem* CPlateConfigMgr::randPlateItem( uint8_t nTimes )
{
	auto iterGroup = m_vPlateGroup.find(nTimes);
	if (iterGroup == m_vPlateGroup.end())
	{
		return nullptr;
	}

	auto& vPlateItems = iterGroup->second.vPlateItems;

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

uint32_t CPlateConfigMgr::getCostByRollTimes(uint8_t nTimes)
{
	auto iterGroup = m_vPlateGroup.find(nTimes);
	if (iterGroup == m_vPlateGroup.end())
	{
		LOGFMTE("not congfig for times = %u",nTimes);
		return -1;
	}
	
	return iterGroup->second.nPrice;
}

void CPlateConfigMgr::Clear()
{
	for (auto& ref : m_vPlateGroup)
	{
		for (auto& pp : ref.second.vPlateItems)
		{
			delete pp;
			pp = nullptr;
		}
		ref.second.vPlateItems.clear();
	}
	m_vPlateGroup.clear();
}