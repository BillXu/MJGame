#pragma once 
#include "IConfigFile.h"
#include "NativeTypes.h"
#include <vector>
#include "ShopConfg.h"
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
	CPlateConfigMgr(){ Clear();}
	~CPlateConfigMgr() { Clear() ;}
	bool OnPaser(CReaderRow& refReaderRow );
	stPlateItem* GetPlateItem(unsigned int nConfigID );
	stPlateItem* randPlateItem();
protected:
	void Clear();
protected:
	VEC_PLATE_ITEMS m_vAllPlateItems ;
};