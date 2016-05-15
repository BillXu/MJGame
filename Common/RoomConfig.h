#pragma once
#include "NativeTypes.h"
#include "IConfigFile.h"
#include <list>
#include <map>
#include "CommonDefine.h"
struct stBaseRoomConfig
{
	uint16_t nConfigID ;
	uint8_t nGameType ;
	uint32_t nDeskFee;
	uint16_t nMaxSeat ;
	uint32_t nEnterTopLimit;
	uint32_t nEnterLowLimit ;
};

struct stMJRoomConfig
	:public stBaseRoomConfig
{
	uint32_t nBaseBet ;
};

class CRoomConfigMgr 
	:public IConfigFile
{
public:
	typedef std::list<stBaseRoomConfig*> LIST_ROOM_CONFIG ;
	typedef LIST_ROOM_CONFIG::iterator LIST_ITER ;
public:
	CRoomConfigMgr(){ Clear();}
	~CRoomConfigMgr(){Clear();}
	bool OnPaser(CReaderRow& refReaderRow );
	LIST_ITER GetBeginIter(){ return m_vAllConfig.begin(); }
	LIST_ITER GetEndIter(){ return m_vAllConfig.end();}
	virtual void OnFinishPaseFile();
	int GetConfigCnt();
	stBaseRoomConfig* GetConfigByConfigID( uint16_t nConfigID );
protected:
	void Clear() ;
protected:
	LIST_ROOM_CONFIG m_vAllConfig ;
};