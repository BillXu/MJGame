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
	void resetInfo( CMJPeerCardNew* peerCard, uint8_t nHuCard, bool isMiaoshou,bool isLoaoYue,bool isGangShangHua,bool isQaingGang, bool isZiMo,bool isJueZhang );
	bool isTargetCardJiang()override ;
	bool isMiaoShouHuiChun(){ return this->m_isMiaoShou ;}  // 妙手回春
	bool isHaiDiLaoYue(){ return this->m_isLaoYue ; };  // 海底捞月
	bool isGangShangHua(){ return this->m_isGangshangHua ; }; // 杠上花
	bool isQiangGangHu(){ return this->m_isQiangGang ;};   // 抢杠胡
	bool isZiMo(){ return this->m_isZiMo ;} ; // 是否自摸；
	bool isHuJueZhang(){ return this->m_isJueZhang ; }; // 是否是胡绝张
public:
	CMJPeerCardNew* m_pPeerCard ;
	uint8_t m_nHuCard ;
	bool m_isMiaoShou ;
	bool m_isLaoYue;
	bool m_isGangshangHua, m_isQiangGang,m_isZiMo,m_isJueZhang ;
};

class C88FanXing ;
class CNewMJRoom ;
class CTwoBirdFanxingChecker
{
public:
	CTwoBirdFanxingChecker();
	~CTwoBirdFanxingChecker();
	bool check( CTowBirdGodCheckFanxingInfo*pInfo, uint8_t& nFanXing, uint8_t& nFanShu );
protected:
	IMJNewCheckFanXing* m_pFanxingChecker ;
};

class CTwoBirdFanxingGroup
	:public IMJNewCheckFanXing
{
public:
	CTwoBirdFanxingGroup(){ m_pNexGroup = nullptr ;}
	bool checkFanxing( ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing , uint8_t& nFanShu )override
	{
		if ( IMJNewCheckFanXing::checkFanxing(pCheckInfo,nFanXing,nFanShu) )
		{
			return true ;
		}

		if ( m_pNexGroup )
		{
			return m_pNexGroup->checkFanxing(pCheckInfo,nFanXing,nFanShu) ;
		}
		return false ;
	}
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override { return false ;}
	void setNexFanxingGroup( IMJNewCheckFanXing* pFanxing ){ m_pNexGroup = pFanxing ;}
private:
	IMJNewCheckFanXing* m_pNexGroup ; 
};

// --------- 清一色fanxing
class CQingYiSeFanxing
	:public IMJNewCheckFanXing	
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};
///---------------
// 88番牌型
class C88FanXing
	:public CTwoBirdFanxingGroup
{
public:
	C88FanXing();

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
	:public CTwoBirdFanxingGroup
{
public:
	C64Fanxing();
};

//  小四喜
class CXiaoSiXiFanxing
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//  小三元
class CXiaoSanYuanFanxing
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//  字一色
class CZiYiSeFanxing
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//  四暗刻
class CSiAnKeFanxing
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// 一色双龙会
class CYiSeShuangLongHuiFanxing
	:public IMJNewCheckFanXing
{
public:
	CYiSeShuangLongHuiFanxing( CQingYiSeFanxing* pqing ){ m_pqingyiSeChecker = pqing ; }
	~CYiSeShuangLongHuiFanxing(){ delete m_pqingyiSeChecker ;}
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
protected:
	IMJNewCheckFanXing* m_pqingyiSeChecker ;
};

// 48番牌型
class C48Fanxing
	:public CTwoBirdFanxingGroup
{
public:
	C48Fanxing();
};

// 一色四同顺 
class CYiSeSiTongShunFanxing
	:public IMJNewCheckFanXing
{
public:
	CYiSeSiTongShunFanxing( CQingYiSeFanxing* pqing ){ m_pqingyiSeChecker = pqing ; }
	~CYiSeSiTongShunFanxing(){ delete m_pqingyiSeChecker ;}
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
protected:
	IMJNewCheckFanXing* m_pqingyiSeChecker ;
};

// 一色四节高
class CYiSeSiJieGaoFanxing
	:public IMJNewCheckFanXing
{
public:
	CYiSeSiJieGaoFanxing( CQingYiSeFanxing* pqing ){ m_pqingyiSeChecker = pqing ; }
	~CYiSeSiJieGaoFanxing(){ delete m_pqingyiSeChecker ;}
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
protected:
	IMJNewCheckFanXing* m_pqingyiSeChecker ;
};

// 32番牌型（3种）
class C32Fanxing
	:public CTwoBirdFanxingGroup
{
public:
	C32Fanxing();
};

// 一色四步高
class CYiSeSiBuGaoFanxing
	:public IMJNewCheckFanXing
{
public:
	CYiSeSiBuGaoFanxing( CQingYiSeFanxing* pqing ){ m_pqingyiSeChecker = pqing ; }
	~CYiSeSiBuGaoFanxing(){ delete m_pqingyiSeChecker ;}
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
protected:
	IMJNewCheckFanXing* m_pqingyiSeChecker ;
};

// 三杠
class CSanGangFanxing
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//混幺九
class CHunYaoJiuFanxing
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// 24番牌型（4种）
class C24Fanxing
	:public CTwoBirdFanxingGroup
{
public:
	C24Fanxing();
};

// 七对
class CQiDui
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// 清一色 
class CQingYiSe
	:public IMJNewCheckFanXing
{
public:
	CQingYiSe( CQingYiSeFanxing* pqing ){ m_pqingyiSeChecker = pqing ; }
	~CQingYiSe(){ delete m_pqingyiSeChecker ;}
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
protected:
	IMJNewCheckFanXing* m_pqingyiSeChecker ;
};

//一色三同顺
class CYiSeTongSanShun
	:public IMJNewCheckFanXing
{
public:
	CYiSeTongSanShun( CQingYiSeFanxing* pqing ){ m_pqingyiSeChecker = pqing ; }
	~CYiSeTongSanShun(){ delete m_pqingyiSeChecker ;}
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
protected:
	IMJNewCheckFanXing* m_pqingyiSeChecker ;
};

//一色三节高
class CYiSeSanJieGao
	:public IMJNewCheckFanXing
{
public:
	CYiSeSanJieGao( CQingYiSeFanxing* pqing ){ m_pqingyiSeChecker = pqing ; }
	~CYiSeSanJieGao(){ delete m_pqingyiSeChecker ;}
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
protected:
	IMJNewCheckFanXing* m_pqingyiSeChecker ;
};

// 16番牌型（3种）
class C16Fanxing
	:public CTwoBirdFanxingGroup
{
public:
	C16Fanxing();
};

// 清龙
class CQingLong
	:public IMJNewCheckFanXing
{
public:
	CQingLong( CQingYiSeFanxing* pqing ){ m_pqingyiSeChecker = pqing ; }
	~CQingLong(){ delete m_pqingyiSeChecker ;}
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
protected:
	IMJNewCheckFanXing* m_pqingyiSeChecker ;
};

//一色三步高
class CYiSeSanBuGao
	:public IMJNewCheckFanXing
{
public:
	CYiSeSanBuGao( CQingYiSeFanxing* pqing ){ m_pqingyiSeChecker = pqing ; }
	~CYiSeSanBuGao(){ delete m_pqingyiSeChecker ;}
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
protected:
	IMJNewCheckFanXing* m_pqingyiSeChecker ;
};

//三暗刻
class CSanAnKe
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// 12番牌型（3种）
class C12Fanxing
	:public CTwoBirdFanxingGroup
{
public:
	C12Fanxing();
};

//大于5
class CDaYu5
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//小于5
class CXiaoYu5
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//三风刻
class CSanFengKe
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// 8番牌型（4种）
class C8Fanxing
	:public CTwoBirdFanxingGroup
{
public:
	C8Fanxing();
};

// 妙手回春
class CMiaoShouHuiChun
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// 海底捞月 
class CHaiDiLaoYue
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// 杠上开花
class CGangShangHua
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// 抢杠胡
class QiangGangHu
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//6番牌型（5种）
class C6Fanxing
	:public CTwoBirdFanxingGroup
{
public:
	C6Fanxing();
};

// 碰碰胡
class CPengPengHu
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//混一色
class CHunYiSe
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// 全求人
class CQuanQiuRen
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// 双暗杠
class CShuangAnGang
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// 双箭刻
class CShuangJianKe
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//4番牌型（4种）
class C4Fanxing
	:public CTwoBirdFanxingGroup
{
public:
	C4Fanxing();
};

// 全带幺
class CQuanDaiYao
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// 不求人
class CBuQiuRen
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// 双明杠
class CShuangMingGang
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//胡绝张
class CHuJueZhang
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//2番牌型（7种）
class C2Fanxing
	:public CTwoBirdFanxingGroup
{
public:
	C2Fanxing();
};

// 箭刻
class CJianKe
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//门前清
class CMengQianQing
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//平和
class CPingHu
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//四归一
class CSiGuiYi
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//双暗刻
class CShuangAnKe
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// 暗杠
class CAnGang
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//断幺
class CDuanYao
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};
