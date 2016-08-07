#pragma once
#include "MJNewCheckFanXing.h"
#include <vector>
class CMJPeerCardNew ;
class CTowBirdGodCheckFanxingInfo
	:public ICheckFanxingInfo
{
public:
	typedef std::vector<uint8_t> VEC_CARD ;
public:
	void resetInfo( CMJPeerCardNew* peerCard, uint8_t nHuCard );
	bool isTargetCardJiang()override ;
public:
	CMJPeerCardNew* m_pPeerCard ;
	uint8_t m_nHuCard ;
};

class C88FanXing ;
class CNewMJRoom ;
class CTwoBirdFanxingChecker
{
public:
	CTwoBirdFanxingChecker();
	~CTwoBirdFanxingChecker();
	bool check( CNewMJRoom* pRoom ,CMJPeerCardNew* peerCard, uint8_t nHuCard, uint8_t& nFanXing, uint8_t& nFanShu );
protected:
	CTowBirdGodCheckFanxingInfo m_tCheckFanxingInfo ;
	C88FanXing* m_pFanxingChecker ;
};

// 88番牌型
class C88FanXing
	:public IMJNewCheckFanXing
{
public:
	C88FanXing();
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//  大四喜 
class CDaSiXiFanxing
	:public IMJNewCheckFanXing	
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// 大三元 
class CDaSanYuanFanxing
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// 九莲宝灯 
class CJiuBaoLianDengFanxing
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// 四杠
class CSiGangFanxing
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// 连七对 
class CLianQiDuiFanxing
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// 64番牌型
class C64Fanxing
	:public IMJNewCheckFanXing
{
public:
	C64Fanxing();
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override { return false ; }
};