#pragma once 
#include "IConfigFile.h"
#include "NativeTypes.h"
#include <vector>
#include "ShopConfg.h"
#include <map>
struct stPlateItem
{
	uint32_t nConfigID ;
	eShopItemType ePlateItemType;
	uint32_t nItemID ;  // meanning deponed on eType ; 
	uint32_t nCount ;
	uint32_t nRate ;
};

class CPlateConfigMgr
	:public IConfigFile
{
public:
	typedef std::vector<stPlateItem*> VEC_PLATE_ITEMS ;
public:
	struct stPlateGroup
	{
		uint8_t nTimes;
		uint32_t nPrice;
		VEC_PLATE_ITEMS vPlateItems;
	};
	typedef std::map<uint8_t, stPlateGroup> VEC_TIMES_PLATE_GROUP;
public:
	CPlateConfigMgr(){ Clear();}
	~CPlateConfigMgr() { Clear() ;}
	bool OnPaser(CReaderRow& refReaderRow );
	stPlateItem* GetPlateItem(unsigned int nConfigID, uint8_t nTimes );
	stPlateItem* randPlateItem( uint8_t nTimes );
	uint32_t getCostByRollTimes( uint8_t nTimes );
protected:
	void Clear();
protected:
	VEC_TIMES_PLATE_GROUP m_vPlateGroup;
};