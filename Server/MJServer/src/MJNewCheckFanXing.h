#pragma once
#include "NativeTypes.h"
#include <vector>
class ICheckFanxingInfo
{
public:
	virtual ~ICheckFanxingInfo(){}
	virtual bool isTargetCardJiang() = 0 ;
};

class IMJNewCheckFanXing
{
public:
	virtual ~IMJNewCheckFanXing();
	virtual bool checkFanxing( ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing , uint8_t& nFanShu );
protected:
	virtual bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu ) = 0 ;
	bool checkSubFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu );
	void addSubFanxing(IMJNewCheckFanXing* pSubFanxing );
protected:
	std::vector<IMJNewCheckFanXing*> m_vSubFanxing ;
};