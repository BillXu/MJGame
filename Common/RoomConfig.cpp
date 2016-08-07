#include "RoomConfig.h"
#include "CommonDefine.h"
#include "LogManager.h"
#include <assert.h>
bool CRoomConfigMgr::OnPaser(CReaderRow& refReaderRow )
{
	unsigned char cType = refReaderRow["GameType"]->IntValue() ;
	stBaseRoomConfig* pRoomConfig = NULL ;
	switch ( cType )
	{
	case eRoom_MJ_Blood_End:
	case eRoom_MJ_Blood_River:
	case eRoom_MJ_Two_Bird_God:
		{
			pRoomConfig = new stMJRoomConfig ;
			((stMJRoomConfig*)pRoomConfig)->nBaseBet = refReaderRow["baseBet"]->IntValue() ;
		}
		break ;
	default:
		CLogMgr::SharedLogMgr()->ErrorLog( "unknown room config ,room type = %d",cType ) ;
		return false;
	}

	pRoomConfig->nConfigID = refReaderRow["configID"]->IntValue();
	pRoomConfig->nDeskFee = refReaderRow["DeskFee"]->IntValue();
	pRoomConfig->nEnterLowLimit = refReaderRow["CoinLowLimit"]->IntValue();
	pRoomConfig->nEnterTopLimit = refReaderRow["CoinTopLimit"]->IntValue();
	pRoomConfig->nMaxSeat = refReaderRow["seat"]->IntValue();
	pRoomConfig->nGameType = cType;
	m_vAllConfig.push_back(pRoomConfig) ;
	return true ;
}

void CRoomConfigMgr::Clear()
{

	LIST_ROOM_CONFIG::iterator iter = m_vAllConfig.begin();
	for ( ; iter != m_vAllConfig.end() ; ++iter )
	{
		delete *iter ;
		*iter = NULL ;
	}
	m_vAllConfig.clear() ;
}

void CRoomConfigMgr::OnFinishPaseFile()
{
	IConfigFile::OnFinishPaseFile();
}

int CRoomConfigMgr::GetConfigCnt( )
{
	return (uint32_t)m_vAllConfig.size() ;
}

stBaseRoomConfig* CRoomConfigMgr::GetConfigByConfigID( uint16_t nConfigID )
{
	LIST_ITER iter = GetBeginIter() ;
	for ( ; iter != GetEndIter() ; ++iter )
	{
		if ( (*iter)->nConfigID == nConfigID )
		{
			return (stBaseRoomConfig*)(*iter) ;
		}
	}
	return nullptr ;
}

