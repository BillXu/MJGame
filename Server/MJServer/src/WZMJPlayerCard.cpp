#include "WZMJPlayerCard.h"
#include "MJCard.h"
#include "log4z.h"
bool WZMJPlayerCard::canEatCard(uint8_t nCard, uint8_t& nWithA, uint8_t& withB)
{
	auto eType = card_Type(nCard);

	auto nBlank = make_Card_Num(eCT_Jian,3);
	if (nCard == nBlank)   // blank replace cai shen 
	{
		nCard = m_nCaiShenCard;
	}

	if (eType >= eCT_Max)
	{
		LOGFMTE("canEatCard parse card type error so do not have this card = %u", nCard);
		return false;
	}

	if (eType != eCT_Tiao && eCT_Tong != eType && eCT_Wan != eType)
	{
		LOGFMTD("only wan , tiao , tong can do eat act");
		return false;
	}

	auto lpFunCanEat = [this](uint8_t nA, uint8_t nB, uint8_t nWithA, uint8_t withB)
	{
		auto nBlank = make_Card_Num(eCT_Jian, 3);
		nA = (nA == m_nCaiShenCard) ? nBlank : nA;
		nB = (nB == m_nCaiShenCard) ? nBlank : nB;
		if (isHaveCard(nA) && isHaveCard(nB))
		{
			if (nWithA != 0 && withB != 0)
			{
				if ((nA == nWithA && nB == withB) || (nB == nWithA && nA == withB))
				{
					return true;
				}
			}
			else
			{
				return true;
			}
		}
		return false;
	};

	// ABX ;
	auto nA = nCard - 2;
	auto nB = nCard - 1;
	if (lpFunCanEat(nA, nB, nWithA, withB))
	{
		return true;
	}

	// AXB ;
	nA = nCard - 1;
	nB = nCard + 1;
	if (lpFunCanEat(nA, nB, nWithA, withB))
	{
		return true;
	}

	// XAB
	nA = nCard + 1;
	nB = nCard + 2;
	if (lpFunCanEat(nA, nB, nWithA, withB))
	{
		return true;
	}

	return false;
}

bool WZMJPlayerCard::onEat(uint8_t nCard, uint8_t nWithA, uint8_t withB)
{
	if (!isHaveCard(nWithA) || !isHaveCard(withB))
	{
		LOGFMTE("do not have with card , can not eat a = %u , b = %u, c = %u", nCard, nWithA, withB);
		return false;
	}

	auto typeA = card_Type(nWithA);
	auto typeB = card_Type(withB);
	auto iter = std::find(m_vCards[typeA].begin(), m_vCards[typeA].end(), nWithA);
	m_vCards[typeA].erase(iter);

	iter = std::find(m_vCards[typeB].begin(), m_vCards[typeB].end(), withB);
	m_vCards[typeB].erase(iter);

	// add to eat , should not sort 
	m_vEated.push_back(nCard);
	m_vEated.push_back(nWithA);
	m_vEated.push_back(withB);
	return true;
}

bool WZMJPlayerCard::canHuWitCard(uint8_t nCard)
{
	auto eType = card_Type(nCard);
	if (eType >= eCT_Max)
	{
		LOGFMTE("parse card type error ,canHuWitCard have this card = %u", nCard);
		return false;
	}

	addCardToVecAsc(m_vCards[eType], nCard);
	bool bSelfHu = MJPlayerCard::isHoldCardCanHu();
	auto iter = std::find(m_vCards[eType].begin(), m_vCards[eType].end(), nCard);
	m_vCards[eType].erase(iter);
	//debugCardInfo();
	return bSelfHu;
}

bool WZMJPlayerCard::isHoldCardCanHu()
{
	bool bSelfHu = MJPlayerCard::isHoldCardCanHu();
	if (bSelfHu)
	{
		return true;
	}

	return getCaiShenCnt() >= 3;
}

bool WZMJPlayerCard::isHardHuWithCard( uint8_t ncard )
{
	if (ncard == 0)
	{

	}
	else
	{
		auto eType = card_Type(ncard);
		if (eType >= eCT_Max)
		{
			LOGFMTE("parse card type error ,canHuWitCard have this card = %u", ncard);
			return false;
		}

		addCardToVecAsc(m_vCards[eType], ncard);
	}

	// replace cai shen with blank
	auto nBlankCnt = getBlankCnt();
	for (uint8_t nIdx = 0; nIdx < nBlankCnt; ++nIdx)
	{
		// add card ;
		auto cType = card_Type(m_nCaiShenCard);
		addCardToVecAsc(m_vCards[cType], m_nCaiShenCard);
		// remove blank
		auto iter = std::find(m_vCards[eCT_Jian].begin(),m_vCards[eCT_Jian].end(),make_Card_Num(eCT_Jian,3));
		m_vCards[eCT_Jian].erase(iter);
	}

	bool isHard = false;
	if (m_vPenged.empty() == false || false == m_vGanged.empty() || false == m_vEated.empty())
	{
		isHard = MJPlayerCard::getMiniQueCnt(m_vCards) == 0;
	}
	else if (MJPlayerCard::get7PairQueCnt(m_vCards) == 0)
	{
		isHard = true;
	}
	else
	{
		isHard = MJPlayerCard::getMiniQueCnt(m_vCards) == 0;
	}

	if (ncard)
	{
		auto eType = card_Type(ncard);
		auto iter = std::find(m_vCards[eType].begin(), m_vCards[eType].end(), ncard);
		m_vCards[eType].erase(iter);
	}

	// roll back card 
	for (uint8_t nIdx = 0; nIdx < nBlankCnt; ++nIdx)
	{
		// add blank card back ;
		addCardToVecAsc(m_vCards[eCT_Jian], make_Card_Num(eCT_Jian, 3));

		// remove cai shen by from blank 
		auto cType = card_Type(m_nCaiShenCard);
		auto iter = std::find(m_vCards[cType].begin(), m_vCards[cType].end(), m_nCaiShenCard);
		m_vCards[cType].erase(iter);
	}

	return isHard;
}

uint8_t WZMJPlayerCard::getBlankCnt()
{
	auto& vec = m_vCards[eCT_Jian];
	auto ncnt = std::count(vec.begin(), vec.end(), make_Card_Num(eCT_Jian,3));
	return ncnt;
}

uint8_t WZMJPlayerCard::getMustChuFisrtCards(VEC_CARD& vCardMustChu)
{
	auto pfnc = [](VEC_CARD& v, eMJCardType ntype, VEC_CARD& vCardMustChu)
	{
		for (uint8_t nV = 1; nV <= 4; ++nV)
		{
			auto nCardNum = make_Card_Num(ntype, nV);
			auto nct = std::count(v.begin(), v.end(), nCardNum);
			if (nct == 1)
			{
				vCardMustChu.push_back(nCardNum);
			}
		}
	};

	pfnc(m_vCards[eCT_Feng], eCT_Feng, vCardMustChu);
	pfnc(m_vCards[eCT_Jian], eCT_Jian, vCardMustChu);

	auto iter = std::find(vCardMustChu.begin(), vCardMustChu.end(), m_nCaiShenCard);
	if (iter != vCardMustChu.end())
	{
		vCardMustChu.erase(iter);
	}

	auto nCaiType = card_Type(m_nCaiShenCard);
	if ( nCaiType != eCT_Feng && eCT_Jian != nCaiType)
	{
		auto iter = std::find(vCardMustChu.begin(), vCardMustChu.end(), make_Card_Num(eCT_Jian,3));
		if ( iter != vCardMustChu.end() )
		{
			vCardMustChu.erase(iter);
		}
	}

	return vCardMustChu.size();
}

void WZMJPlayerCard::setCaiShenCard(uint8_t nCaiShenCard)
{
	m_nCaiShenCard = nCaiShenCard;
}

uint8_t WZMJPlayerCard::getMiniQueCnt(VEC_CARD vCards[eCT_Max])
{
	uint8_t nCaishenCnt = getCaiShenCnt();

	// replace cai shen with blank
	auto nBlankCnt = getBlankCnt();
	for (uint8_t nIdx = 0; nIdx < nBlankCnt; ++nIdx)
	{
		// add card ;
		auto cType = card_Type(m_nCaiShenCard);
		addCardToVecAsc(m_vCards[cType], m_nCaiShenCard);
		// remove blank
		auto iter = std::find(m_vCards[eCT_Jian].begin(), m_vCards[eCT_Jian].end(), make_Card_Num(eCT_Jian, 3));
		m_vCards[eCT_Jian].erase(iter);
	}

	auto prooBackCaiFun = [this]( uint8_t nBlankCnt )
	{
		// roll back card 
		for (uint8_t nIdx = 0; nIdx < nBlankCnt; ++nIdx)
		{
			// add blank card back ;
			addCardToVecAsc(m_vCards[eCT_Jian], make_Card_Num(eCT_Jian, 3));

			// remove cai shen by from blank 
			auto cType = card_Type(m_nCaiShenCard);
			auto iter = std::find(m_vCards[cType].begin(), m_vCards[cType].end(), m_nCaiShenCard);
			m_vCards[cType].erase(iter);
		}
	
	};

	if ( nCaishenCnt == 0 )
	{
		auto nQueCnt = MJPlayerCard::getMiniQueCnt(vCards);
		prooBackCaiFun(nBlankCnt);
		return nQueCnt;
	}

	uint8_t nCaiShenType = card_Type(m_nCaiShenCard);
	if (nCaiShenType >= eCT_Max)
	{
		LOGFMTE("invalid type = %u from caishen card = %u", nCaiShenType, m_nCaiShenCard);
		prooBackCaiFun(nBlankCnt);
		return 100;
	}

	VEC_CARD vBackUpJian;
	vBackUpJian.assign(vCards[nCaiShenType].begin(), vCards[nCaiShenType].end());
	auto caiShen = m_nCaiShenCard;
	auto iter = std::find(vCards[nCaiShenType].begin(), vCards[nCaiShenType].end(), caiShen);
	while (iter != vCards[nCaiShenType].end())
	{
		vCards[nCaiShenType].erase(iter);
		iter = std::find(vCards[nCaiShenType].begin(), vCards[nCaiShenType].end(), caiShen);
	}

	uint8_t nCnt = MJPlayerCard::getMiniQueCnt(vCards);
	// rollback ;
	vCards[nCaiShenType].swap(vBackUpJian);
	prooBackCaiFun(nBlankCnt);

	if (nCaishenCnt >= nCnt) // hu le 
	{
		if (m_nJIang == 0)
		{
			m_nJIang = caiShen;
			m_nDanDiao = caiShen;
		}
		nCaishenCnt -= nCnt;
		if (nCaishenCnt != 0 && nCaishenCnt != 3)
		{
			LOGFMTE("can not be here !! nCaishenCnt != 0 && nCaishenCnt != 3 ");
		}
		return 0;
	}
	nCnt -= nCaishenCnt;
	return nCnt;
}

uint8_t WZMJPlayerCard::getCaiShenCnt()
{
	uint8_t nType = card_Type(m_nCaiShenCard);
	if (nType >= eCT_Max)
	{
		LOGFMTE("invalid type = %u from caishen card = %u",nType,m_nCaiShenCard);
		return 0;
	}

	auto& v = m_vCards[nType];
	auto n = std::count(v.begin(),v.end(),m_nCaiShenCard);
	return n;
}

uint8_t WZMJPlayerCard::get7PairQueCnt(VEC_CARD vCards[eCT_Max])
{
	uint8_t nCaishenCnt = getCaiShenCnt();

	// replace cai shen with blank
	auto nBlankCnt = getBlankCnt();
	for (uint8_t nIdx = 0; nIdx < nBlankCnt; ++nIdx)
	{
		// add card ;
		auto cType = card_Type(m_nCaiShenCard);
		addCardToVecAsc(m_vCards[cType], m_nCaiShenCard);
		// remove blank
		auto iter = std::find(m_vCards[eCT_Jian].begin(), m_vCards[eCT_Jian].end(), make_Card_Num(eCT_Jian, 3));
		m_vCards[eCT_Jian].erase(iter);
	}

	auto prooBackCaiFun = [this](uint8_t nBlankCnt)
	{
		// roll back card 
		for (uint8_t nIdx = 0; nIdx < nBlankCnt; ++nIdx)
		{
			// add blank card back ;
			addCardToVecAsc(m_vCards[eCT_Jian], make_Card_Num(eCT_Jian, 3));

			// remove cai shen by from blank 
			auto cType = card_Type(m_nCaiShenCard);
			auto iter = std::find(m_vCards[cType].begin(), m_vCards[cType].end(), m_nCaiShenCard);
			m_vCards[cType].erase(iter);
		}

	};

	if (nCaishenCnt == 0)
	{
		auto nQueCnt = MJPlayerCard::get7PairQueCnt(vCards);
		prooBackCaiFun(nBlankCnt);
		return nQueCnt;
	}

	uint8_t nCaiShenType = card_Type(m_nCaiShenCard);
	if (nCaiShenType >= eCT_Max)
	{
		LOGFMTE("invalid type = %u from caishen card = %u", nCaiShenType, m_nCaiShenCard);
		prooBackCaiFun(nBlankCnt);
		return 100;
	}

	VEC_CARD vBackUpJian;
	vBackUpJian.assign(vCards[nCaiShenType].begin(), vCards[nCaiShenType].end());
	auto caiShen = m_nCaiShenCard;
	auto iter = std::find(vCards[nCaiShenType].begin(), vCards[nCaiShenType].end(), caiShen);
	while (iter != vCards[nCaiShenType].end())
	{
		vCards[nCaiShenType].erase(iter);
		iter = std::find(vCards[nCaiShenType].begin(), vCards[nCaiShenType].end(), caiShen);
	}
	auto nCnt = MJPlayerCard::get7PairQueCnt(vCards);
	// rollback ;
	vCards[nCaiShenType].swap(vBackUpJian);
	prooBackCaiFun(nBlankCnt);

	if (nCaishenCnt >= nCnt) // hu le 
	{
		if (m_nJIang == 0)
		{
			m_nJIang = caiShen;
			m_nDanDiao = caiShen;
		}
		nCaishenCnt -= nCnt;
		return nCaishenCnt % 2;
	}
	nCnt -= nCaishenCnt;
	return nCnt;
}

bool WZMJPlayerCard::onDoHu(bool isZiMo, uint8_t nCard, uint8_t& nHuType, uint8_t& nBeiShu )
{
	// if not zi mo , must add to fo check hu ;
	if (!isZiMo)
	{
		auto type = card_Type(nCard);
		if (type >= eCT_Max)
		{
			LOGFMTE("invalid card type for card = %u", nCard);
			return false;
		}
		addCardToVecAsc(m_vCards[type], nCard);
	}

	auto funRemoveAddToCard = [this](uint8_t nCard)
	{
		auto type = card_Type(nCard);
		auto iter = std::find(m_vCards[type].begin(), m_vCards[type].end(), nCard);
		if (iter == m_vCards[type].end())
		{
			LOGFMTE("hu this card should already addto hold card , but can not remove  whay card = %u", nCard);
			return;
		}
		m_vCards[type].erase(iter);
	};

	if (isHoldCardCanHu() == false)
	{
		LOGFMTE("do hu act , but can not hu ? why ? bug card = %u ", nCard);
		debugCardInfo();
		if (!isZiMo)
		{
			funRemoveAddToCard(nCard);
		}
		return false;
	}

	if (!isZiMo)
	{
		funRemoveAddToCard(nCard);
	}
	getFanxing(nHuType,nBeiShu);
	return true;
}

bool WZMJPlayerCard::getFanxing(uint8_t& eFanxing, uint8_t& beiShu)
{
	VEC_CARD vCard;
	getHoldCard(vCard);
	if (vCard.size() == 2)
	{
		eFanxing = eFangxing_DanDiao;
		beiShu = 4;
		return true;
	}

	if ( canHoldCard7PairHu() )
	{
		if ( isHardHuWithCard() )
		{
			eFanxing = eFanxing_YingBaDui;
			beiShu = 4;
			return true;
		}

		eFanxing = eFanxing_RuanBaDui;
		beiShu = 2;
		return true;
	}

	if ( getCaiShenCnt() == 3)
	{
		if (MJPlayerCard::isHoldCardCanHu())
		{
			eFanxing = eFanxing_SanCaiShen;
			beiShu = 4;
			return true;
		}

		eFanxing = eFanxing_CaiShenHui;
		beiShu = 2;
		return true;
	}

	if (isHardHuWithCard())
	{
		eFanxing = eFanxing_YingPai;
		beiShu = 2;
		return true;
	}

	eFanxing = eFanxing_RuanPai;
	beiShu = 1;
	return true;
}