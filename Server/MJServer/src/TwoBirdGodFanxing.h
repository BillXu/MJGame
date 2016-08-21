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
	bool isMiaoShouHuiChun(){ return this->m_isMiaoShou ;}  // ���ֻش�
	bool isHaiDiLaoYue(){ return this->m_isLaoYue ; };  // ��������
	bool isGangShangHua(){ return this->m_isGangshangHua ; }; // ���ϻ�
	bool isQiangGangHu(){ return this->m_isQiangGang ;};   // ���ܺ�
	bool isZiMo(){ return this->m_isZiMo ;} ; // �Ƿ�������
	bool isHuJueZhang(){ return this->m_isJueZhang ; }; // �Ƿ��Ǻ�����
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

// --------- ��һɫfanxing
class CQingYiSeFanxing
	:public IMJNewCheckFanXing	
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};
///---------------
// 88������
class C88FanXing
	:public CTwoBirdFanxingGroup
{
public:
	C88FanXing();

};

//  ����ϲ 
class CDaSiXiFanxing
	:public IMJNewCheckFanXing	
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// ����Ԫ 
class CDaSanYuanFanxing
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// �������� 
class CJiuBaoLianDengFanxing
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// �ĸ�
class CSiGangFanxing
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// ���߶� 
class CLianQiDuiFanxing
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// 64������
class C64Fanxing
	:public CTwoBirdFanxingGroup
{
public:
	C64Fanxing();
};

//  С��ϲ
class CXiaoSiXiFanxing
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//  С��Ԫ
class CXiaoSanYuanFanxing
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//  ��һɫ
class CZiYiSeFanxing
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//  �İ���
class CSiAnKeFanxing
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// һɫ˫����
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

// 48������
class C48Fanxing
	:public CTwoBirdFanxingGroup
{
public:
	C48Fanxing();
};

// һɫ��ͬ˳ 
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

// һɫ�Ľڸ�
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

// 32�����ͣ�3�֣�
class C32Fanxing
	:public CTwoBirdFanxingGroup
{
public:
	C32Fanxing();
};

// һɫ�Ĳ���
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

// ����
class CSanGangFanxing
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//���۾�
class CHunYaoJiuFanxing
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// 24�����ͣ�4�֣�
class C24Fanxing
	:public CTwoBirdFanxingGroup
{
public:
	C24Fanxing();
};

// �߶�
class CQiDui
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// ��һɫ 
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

//һɫ��ͬ˳
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

//һɫ���ڸ�
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

// 16�����ͣ�3�֣�
class C16Fanxing
	:public CTwoBirdFanxingGroup
{
public:
	C16Fanxing();
};

// ����
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

//һɫ������
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

//������
class CSanAnKe
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// 12�����ͣ�3�֣�
class C12Fanxing
	:public CTwoBirdFanxingGroup
{
public:
	C12Fanxing();
};

//����5
class CDaYu5
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//С��5
class CXiaoYu5
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//�����
class CSanFengKe
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// 8�����ͣ�4�֣�
class C8Fanxing
	:public CTwoBirdFanxingGroup
{
public:
	C8Fanxing();
};

// ���ֻش�
class CMiaoShouHuiChun
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// �������� 
class CHaiDiLaoYue
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// ���Ͽ���
class CGangShangHua
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// ���ܺ�
class QiangGangHu
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//6�����ͣ�5�֣�
class C6Fanxing
	:public CTwoBirdFanxingGroup
{
public:
	C6Fanxing();
};

// ������
class CPengPengHu
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//��һɫ
class CHunYiSe
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// ȫ����
class CQuanQiuRen
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// ˫����
class CShuangAnGang
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// ˫����
class CShuangJianKe
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//4�����ͣ�4�֣�
class C4Fanxing
	:public CTwoBirdFanxingGroup
{
public:
	C4Fanxing();
};

// ȫ����
class CQuanDaiYao
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// ������
class CBuQiuRen
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// ˫����
class CShuangMingGang
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//������
class CHuJueZhang
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//2�����ͣ�7�֣�
class C2Fanxing
	:public CTwoBirdFanxingGroup
{
public:
	C2Fanxing();
};

// ����
class CJianKe
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//��ǰ��
class CMengQianQing
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//ƽ��
class CPingHu
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//�Ĺ�һ
class CSiGuiYi
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//˫����
class CShuangAnKe
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

// ����
class CAnGang
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};

//����
class CDuanYao
	:public IMJNewCheckFanXing
{
public:
	bool checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )override;
};
