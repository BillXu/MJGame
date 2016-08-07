#include "MJNewCheckFanXing.h"
#include <cassert>
IMJNewCheckFanXing::~IMJNewCheckFanXing()
{
	for ( auto& ref : m_vSubFanxing )
	{
		delete ref ;
		ref = nullptr ;
	}
	m_vSubFanxing.clear() ;
}

bool IMJNewCheckFanXing::checkFanxing( ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	if ( checkSelfFanxing(pCheckInfo,nFanXing,nFanShu ) )
	{
		return true ;
	}

	return checkSubFanxing(pCheckInfo,nFanXing,nFanShu);
}

bool IMJNewCheckFanXing::checkSubFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu)
{
	for ( auto& pref : m_vSubFanxing )
	{
		if ( pref->checkFanxing(pCheckInfo,nFanXing,nFanShu) )
		{
			return true ;
		}
	}
	
	return false ;
}

void IMJNewCheckFanXing::addSubFanxing(IMJNewCheckFanXing* pSubFanxing )
{
#ifdef _DEBUG
	for ( auto& ref : m_vSubFanxing )
	{
		assert(ref != pSubFanxing && "do not add twice" );
	}
#endif
	m_vSubFanxing.push_back(pSubFanxing) ;
}