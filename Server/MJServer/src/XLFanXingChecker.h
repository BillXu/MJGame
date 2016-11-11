#pragma once 
#include "NativeTypes.h"
#include "MJPlayerCard.h"
#include "MJCard.h"
#include "log4z.h"
class XLFanXingHelper
{
public:
	void helpGetHoldCardByType(MJPlayerCard::VEC_CARD& vCards, uint8_t nType){}
	void helpGetPengedCard(MJPlayerCard::VEC_CARD& vCards){}
	void helpGetGangCard(MJPlayerCard::VEC_CARD& vCards){}
	bool helpGetIs7PairHu(){ return false; };
	uint8_t helpGetJiang(){ return 0; };
};

class XLFanXingChecker
{
public:
	virtual ~XLFanXingChecker(){}
	virtual bool doCheckFanxing(XLFanXingHelper* pHelper, uint8_t& nBeiShu, uint32_t& nFanXingType){ nBeiShu = 1; nFanXingType = 2;  return true; };
	virtual bool doSelfCheck(XLFanXingHelper* pHelper) { return true; };
};

class XLFanXingQingYiSe
	:public XLFanXingChecker
{
public:
	bool doCheckFanxing(XLFanXingHelper* pHelper, uint8_t& nBeiShu, uint32_t& nFanXingType)override
	{
		if (doSelfCheck(pHelper) )
		{
			nBeiShu = 4;
			nFanXingType = eFanxing_QingYiSe;
			return true;
		}
		return false;
	}

	bool doSelfCheck(XLFanXingHelper* pHelper)override
	{
		MJPlayerCard::VEC_CARD vCards;
		uint8_t nSingleType = 0;
		pHelper->helpGetHoldCardByType(vCards, eCT_Wan);
		if (vCards.empty() == false)
		{
			nSingleType = eCT_Wan;
		}
		vCards.clear();

		pHelper->helpGetHoldCardByType(vCards, eCT_Tiao);
		if (vCards.empty() == false)
		{
			if (0 != nSingleType)
			{
				return false;
			}
			nSingleType = eCT_Tiao;
		}
		vCards.clear();

		pHelper->helpGetHoldCardByType(vCards, eCT_Tong);
		if (vCards.empty() == false)
		{
			if (0 != nSingleType)
			{
				return false;
			}
			nSingleType = eCT_Tong;
		}
		vCards.clear();

		// check peng , and gang ;
		pHelper->helpGetGangCard(vCards);
		for (auto& ref : vCards)
		{
			if (nSingleType != card_Type(ref))
			{
				return false;
			}
		}

		// check peng
		vCards.clear();
		pHelper->helpGetPengedCard(vCards);
		for (auto& ref : vCards)
		{
			if (nSingleType != card_Type(ref))
			{
				return false;
			}
		}
		return true;
	}
};

class XLFanXingQiDui
	:public XLFanXingChecker
{
public:
	bool doCheckFanxing(XLFanXingHelper* pHelper, uint8_t& nBeiShu, uint32_t& nFanXingType)override
	{
		if (doSelfCheck(pHelper))
		{
			nBeiShu = 4;
			nFanXingType = eFanxing_QiDui;
			return true;
		}
		return false;
	}

	bool doSelfCheck(XLFanXingHelper* pHelper)override
	{
		return pHelper->helpGetIs7PairHu();
	}
};

class XLFanXingQingQiDui
	:public XLFanXingChecker
{
public:
	bool doCheckFanxing(XLFanXingHelper* pHelper, uint8_t& nBeiShu, uint32_t& nFanXingType)override
	{
		if (doSelfCheck(pHelper))
		{
			nBeiShu = 16;
			nFanXingType = eFanxing_QingDui;
			return true;
		}
		return false;
	}

	bool doSelfCheck(XLFanXingHelper* pHelper)override
	{
		return m_tQingYiSe.doSelfCheck(pHelper) && m_tQiDui.doSelfCheck(pHelper);
	}

protected:
	XLFanXingQingYiSe m_tQingYiSe;
	XLFanXingQiDui m_tQiDui;
};

class XLFanXingLongQiDui
	:public XLFanXingChecker
{
public:
	bool doCheckFanxing(XLFanXingHelper* pHelper, uint8_t& nBeiShu, uint32_t& nFanXingType)override
	{
		if (doSelfCheck(pHelper))
		{
			nBeiShu = 16;
			nFanXingType = eFanxing_LongQiDui;
			return true;
		}
		return false;
	}

	bool doSelfCheck(XLFanXingHelper* pHelper)override
	{
		if (!pHelper->helpGetIs7PairHu())
		{
			return false;
		}

		auto haveGang = [](MJPlayerCard::VEC_CARD vCards)->bool
		{
			if (vCards.size() < 4)
			{
				return false;
			}

			for (uint8_t nIdx = 0; nIdx + 3u < vCards.size(); ++nIdx)
			{
				if (vCards[nIdx] == vCards[nIdx + 3u])
				{
					return true;
				}
			}

			return false;
		};

		MJPlayerCard::VEC_CARD vCards;
		pHelper->helpGetHoldCardByType(vCards, eCT_Wan);
		if (haveGang(vCards))
		{
			return true;
		}

		vCards.clear();
		pHelper->helpGetHoldCardByType(vCards, eCT_Tiao);
		if (haveGang(vCards))
		{
			return true;
		}

		vCards.clear();
		pHelper->helpGetHoldCardByType(vCards, eCT_Tong);
		if (haveGang(vCards))
		{
			return true;
		}
		return false ;
	}

};

class XLFanXingQingLongQiDui
	:public XLFanXingChecker
{
public:
	bool doCheckFanxing(XLFanXingHelper* pHelper, uint8_t& nBeiShu, uint32_t& nFanXingType)override
	{
		if (doSelfCheck(pHelper))
		{
			nBeiShu = 32;
			nFanXingType = eFanxing_QingLongQiDui;
			return true;
		}
		return false;
	}

	bool doSelfCheck(XLFanXingHelper* pHelper)override
	{
		return m_tQingYiSe.doSelfCheck(pHelper) && m_tLongQiDui.doSelfCheck(pHelper);
	}
protected:
	XLFanXingLongQiDui m_tLongQiDui;
	XLFanXingQingYiSe m_tQingYiSe;
};

class XLFanXingDuiDuiHu
	:public XLFanXingChecker
{
public:
	bool doCheckFanxing(XLFanXingHelper* pHelper, uint8_t& nBeiShu, uint32_t& nFanXingType)override
	{
		if (doSelfCheck(pHelper))
		{
			nBeiShu = 2;
			nFanXingType = eFanxing_DuiDuiHu;
			return true;
		}
		return false;
	}

	bool doSelfCheck(XLFanXingHelper* pHelper)override
	{
		auto pfunCheckKeZi = [](XLFanXingHelper* pHelper, uint8_t nType, uint8_t nJiang )->bool
		{
			MJPlayerCard::VEC_CARD vCards;
			pHelper->helpGetHoldCardByType(vCards, nType);
			if (vCards.empty())
			{
				return true;
			}

			if (card_Type(nJiang) == nType)
			{
				// remove jiang
				auto cnt = std::count(vCards.begin(), vCards.end(), nJiang);
				if (cnt < 2)
				{
					LOGFMTE("check duidui hu fanxing jiang cnt < 2  %u",nJiang );
					return false;
				}

				auto iter = std::find(vCards.begin(),vCards.end(),nJiang);
				vCards.erase(iter);
				iter = std::find(vCards.begin(), vCards.end(), nJiang);
				vCards.erase(iter);
			}


			for (uint8_t nIdx = 0; nIdx + 2u < vCards.size(); nIdx += 3 )
			{
				if (vCards[nIdx] != vCards[nIdx + 2u])
				{
					return false;
				}
			}
			return true;
		};

		auto nJiang = pHelper->helpGetJiang();
		if (pfunCheckKeZi(pHelper, eCT_Tiao, nJiang) == false)
		{
			return false;
		}

		if (pfunCheckKeZi(pHelper, eCT_Wan, nJiang) == false)
		{
			return false;
		}

		if (pfunCheckKeZi(pHelper, eCT_Tong, nJiang) == false)
		{
			return false;
		}
		return true;
	}
};

class XLFanXingQingDuiDuiHu
	:public XLFanXingChecker
{
public:
	bool doCheckFanxing(XLFanXingHelper* pHelper, uint8_t& nBeiShu, uint32_t& nFanXingType)override
	{
		if (doSelfCheck(pHelper))
		{
			nBeiShu = 8;
			nFanXingType = eFanxing_QingDuiDuiHu;
			return true;
		}
		return false;
	}

	bool doSelfCheck(XLFanXingHelper* pHelper)override
	{
		return m_tQingYiSe.doSelfCheck(pHelper) && m_tDuiDuiHu.doSelfCheck(pHelper);
	}
protected:
	XLFanXingDuiDuiHu m_tDuiDuiHu;
	XLFanXingQingYiSe m_tQingYiSe;
};


class XLFanXingJinGouDiao
	:public XLFanXingChecker
{
public:
	bool doCheckFanxing(XLFanXingHelper* pHelper, uint8_t& nBeiShu, uint32_t& nFanXingType)override
	{
		if (doSelfCheck(pHelper))
		{
			nBeiShu = 4;
			nFanXingType = eFanxing_JinGouDiao;
			return true;
		}
		return false;
	}

	bool doSelfCheck(XLFanXingHelper* pHelper)override
	{
		uint8_t nHoldCard = 0;

		MJPlayerCard::VEC_CARD vCards;
		pHelper->helpGetHoldCardByType(vCards, eCT_Wan);
		nHoldCard += vCards.size();
		if (nHoldCard > 2)
		{
			return false;
		}

		vCards.clear();
		pHelper->helpGetHoldCardByType(vCards, eCT_Tiao);
		nHoldCard += vCards.size();
		if (nHoldCard > 2)
		{
			return false;
		}

		vCards.clear();
		pHelper->helpGetHoldCardByType(vCards, eCT_Tong);
		nHoldCard += vCards.size();
		if (nHoldCard > 2)
		{
			return false;
		}
		return true;
	}
};


class XLFanXingQingJinGouDiao
	:public XLFanXingChecker
{
public:
	bool doCheckFanxing(XLFanXingHelper* pHelper, uint8_t& nBeiShu, uint32_t& nFanXingType)override
	{
		if (doSelfCheck(pHelper))
		{
			nBeiShu = 16;
			nFanXingType = eFanxing_QingJinGouDiao;
			return true;
		}
		return false;
	}

	bool doSelfCheck(XLFanXingHelper* pHelper)override
	{
		return m_tQingYiSe.doSelfCheck(pHelper) && m_tJinGouDiao.doSelfCheck(pHelper);
	}
protected:
	XLFanXingJinGouDiao m_tJinGouDiao;
	XLFanXingQingYiSe m_tQingYiSe;
};


class XLFanXingShiBaLuoHan
	:public XLFanXingChecker
{
public:
	bool doCheckFanxing(XLFanXingHelper* pHelper, uint8_t& nBeiShu, uint32_t& nFanXingType)override
	{
		if (doSelfCheck(pHelper))
		{
			nBeiShu = 64;
			nFanXingType = eFanxing_ShiBaLuoHan;
			return true;
		}
		return false;
	}

	bool doSelfCheck(XLFanXingHelper* pHelper)override
	{
		if (m_tJinGouDiao.doSelfCheck(pHelper) == false)
		{
			return false;
		}

		MJPlayerCard::VEC_CARD vCards;
		pHelper->helpGetGangCard(vCards);
		if (vCards.size() < 4 )
		{
			return false;
		}
		return true;
	}
protected:
	XLFanXingJinGouDiao m_tJinGouDiao;
};

class XLFanXingJiangJinGouDiao
	:public XLFanXingChecker
{
public:
	bool doCheckFanxing(XLFanXingHelper* pHelper, uint8_t& nBeiShu, uint32_t& nFanXingType)override
	{
		if (doSelfCheck(pHelper))
		{
			nBeiShu = 16;
			nFanXingType = eFanxing_JiangJinGouDiao;
			return true;
		}
		return false;
	}

	bool doSelfCheck(XLFanXingHelper* pHelper)override
	{
		if (m_tJinGouDiao.doSelfCheck(pHelper) == false)
		{
			return false;
		}

		auto pfunFindNotJiang = [](MJPlayerCard::VEC_CARD& vCards)->bool
		{
			if (vCards.empty())
			{
				return false;
			}

			for (auto& ref : vCards)
			{
				auto v = card_Value(ref);
				if (2 != v && 5 != v && 8 != v)
				{
					return true;
				}
			}
			return false;
		};

		MJPlayerCard::VEC_CARD vCards;
		vCards.push_back(pHelper->helpGetJiang());
		if (pfunFindNotJiang(vCards))
		{
			return false;
		}

		vCards.clear();
		pHelper->helpGetGangCard(vCards);
		if (pfunFindNotJiang(vCards))
		{
			return false;
		}
	 
		vCards.clear();
		pHelper->helpGetPengedCard(vCards);
		if (pfunFindNotJiang(vCards))
		{
			return false;
		}

		return true;
	}
protected:
	XLFanXingJinGouDiao m_tJinGouDiao;
};


//class CBloodFanxingDaiYaoJiu
//	:public IMJCardFanXing
//{
//public:
//	CBloodFanxingDaiYaoJiu();
//	uint8_t getFanRate()override{ return 4; };
//	eFanxingType getType()override{ return eFanxing_DaiYaoJiu; }
//	bool checkType(CMJPeerCard& peerCard)override;
//};
//
//class CBloodFanxingQingDaiYaoJiu
//	:public CBloodQingYiSe
//{
//public:
//	uint8_t getFanRate()override{ return 16; };
//	eFanxingType getType()override{ return eFanxing_QingDaiYaoJiu; }
//};
//
//class CBloodFanxing
//	:public CSingleton<CBloodFanxing>
//{
//public:
//	bool checkHuPai(CMJPeerCard& peerCard, eFanxingType & eHuType, uint8_t& nFanshu);
//	bool checkFanXingWantedCards(CMJPeerCard& peerCard, LIST_WANTED_CARD& vWaited);
//protected:
//	CBloodFanxingPingHu m_tPingHu;
//	CBloodFanxingQiDui m_tQiDui;
//};
