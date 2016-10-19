#include "MJPlayerCard.h"
#include "MJCard.h"
#include "LogManager.h"

MJPlayerCard::stNotShunCard::stNotShunCard(){ vCards.clear(); }
bool MJPlayerCard::stNotShunCard::operator != (const stNotShunCard& v)
{
	if (*this == v)
	{
		return false;
	}
	return true;
}

MJPlayerCard::stNotShunCard& MJPlayerCard::stNotShunCard::operator = (const stNotShunCard& v)
{
	vCards.clear();
	vCards.assign(v.vCards.begin(), v.vCards.end());
	return *this;
}

bool MJPlayerCard::stNotShunCard::operator == (const stNotShunCard& v)
{
	if (vCards.size() != v.vCards.size())
	{
		return false;
	}

	if (vCards.empty())
	{
		// will never come to here ;
		return true;
	}
	stNotShunCard self = *this;
	stNotShunCard nonConstObj = v;
	std::sort(self.vCards.begin(), self.vCards.end());
	std::sort(nonConstObj.vCards.begin(), nonConstObj.vCards.end());
	for (uint8_t nIdx = 0; nIdx < self.vCards.size(); ++nIdx)
	{
		if (self.vCards[nIdx] != nonConstObj.vCards[nIdx])
		{
			return false;
		}
	}
	return true;
}

// mj player card ;
void MJPlayerCard::reset()
{

}

void MJPlayerCard::addDistributeCard(uint8_t nCardNum)
{
	auto eType = card_Type(nCardNum);
	addCardToVecAsc(m_vCards[eType],nCardNum);
}

bool MJPlayerCard::onGangCardBeRobot(uint8_t nCardNum)
{
	// must test vCard is orig vector ;
	auto eType = card_Type(nCardNum);
	if (eType >= eCT_Max)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("parse card type error ,gang be robot have this card = %u", nCardNum);
		return false;
	}
	auto& vCard = m_vCards[eType];
	auto iter = std::find(vCard.begin(),vCard.end(),nCardNum);
	if (iter != vCard.end())
	{
		vCard.erase(iter);
		return true;
	}
	CLogMgr::SharedLogMgr()->ErrorLog("robot the gang card but player do not have card = %u",nCardNum);
	return false;
}

bool MJPlayerCard::onCardBeGangPengEat(uint8_t nCardNum)
{
	auto eType = card_Type(nCardNum);
	auto& vCard = m_vChuedCard;
	auto iter = std::find(vCard.begin(), vCard.end(), nCardNum);
	if (iter != vCard.end())
	{
		vCard.erase(iter);
		return true;
	}
	CLogMgr::SharedLogMgr()->ErrorLog("gang eat peng card but player do not have card = %u", nCardNum);
	return false;
}

bool MJPlayerCard::isHaveCard(uint8_t nCardNum)
{
	auto eType = card_Type(nCardNum);
	if (eType >= eCT_Max)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("parse card type error so do not have this card = %u",nCardNum);
		return false;
	}

	auto& vCard = m_vCards[eType];
	auto iter = std::find(vCard.begin(), vCard.end(), nCardNum);
	return iter != vCard.end();
}

bool MJPlayerCard::canMingGangWithCard(uint8_t nCard)
{
	auto eType = card_Type(nCard);
	if (eType >= eCT_Max)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("canMingGangWithCard parse card type error so do not have this card = %u", nCard);
		return false;
	}
	auto& vCard = m_vCards[eType];
	auto nCnt = std::count(vCard.begin(), vCard.end(), nCard);
	return nCard == 3;
}

bool MJPlayerCard::canPengWithCard(uint8_t nCard)
{
	auto eType = card_Type(nCard);
	if (eType >= eCT_Max)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("canPengWithCard parse card type error so do not have this card = %u", nCard);
		return false;
	}
	auto& vCard = m_vCards[eType];
	auto nCnt = std::count(vCard.begin(), vCard.end(), nCard);
	return nCard >= 2;
}

bool MJPlayerCard::canEatCard(uint8_t nCard, uint8_t& nWithA, uint8_t& withB)
{
	auto eType = card_Type(nCard);
	if (eType >= eCT_Max)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("canEatCard parse card type error so do not have this card = %u", nCard);
		return false;
	}

	if (eType != eCT_Tiao && eCT_Tong != eType && eCT_Wan != eType)
	{
		CLogMgr::SharedLogMgr()->PrintLog("only wan , tiao , tong can do eat act");
		return false;
	}

	// ABX ;
	nWithA = nCard - 2;
	withB = nCard - 1;
	if (isHaveCard(nWithA) && isHaveCard(withB))
	{
		return true;
	}
	// AXB ;
	nWithA = nCard - 1;
	withB = nCard + 1;
	if (isHaveCard(nWithA) && isHaveCard(withB))
	{
		return true;
	}
	// XAB
	nWithA = nCard + 1;
	withB = nCard + 2;
	if (isHaveCard(nWithA) && isHaveCard(withB))
	{
		return true;
	}
	return false;
}

bool MJPlayerCard::canHuWitCard(uint8_t nCard)
{
	return true;
}

bool MJPlayerCard::isTingPai()
{
	bool bMayting = false;
	VEC_NOT_SHUN vNotShun;
	vNotShun.clear();
	for (auto& vCard : m_vCards)
	{
		if (vCard.empty())
		{
			continue;
		}
		VEC_NOT_SHUN tTemp;
		getNotShuns(vCard, tTemp);
		if ( tTemp.empty() )
		{
			continue;
		}

		if (!vNotShun.empty())
		{
			return false;
		}
		else
		{
			vNotShun.swap(tTemp);
		}
	}

	if (vNotShun.empty())
	{
		CLogMgr::SharedLogMgr()->ErrorLog("why no card type not shun , all card type is shun ? bug !!!");
		return false;
	}

	for (auto& refNotShun : vNotShun)
	{
		if (refNotShun.getLackCardCntForShun() == 1)
		{
			return true;
		}
	}

	return false;
}

bool MJPlayerCard::getHoldCardThatCanAnGang(VEC_CARD& vGangCards)
{
	for (auto& vCard : m_vCards)
	{
		if ( vCard.size() < 4 )
		{
			continue;
		}

		for (uint8_t nIdx = 0; (uint8_t)(nIdx + 3) < vCard.size();)
		{
			if (vCard[nIdx] == vCard[nIdx + 3])
			{
				vGangCards.push_back(vCard[nIdx]);
				nIdx += 4;
			}
			else
			{
				++nIdx;
			}
		}
	}
	return !vGangCards.empty();
}

bool MJPlayerCard::getHoldCardThatCanBuGang(VEC_CARD& vGangCards)
{
	for (auto& ref : m_vPenged)
	{
		if (isHaveCard(ref))
		{
			vGangCards.push_back(ref);
		}
	}

	return !vGangCards.empty();
}

bool MJPlayerCard::isHoldCardCanHu()
{
	return true;
}

void MJPlayerCard::onMoCard(uint8_t nMoCard)
{
	auto eType = card_Type(nMoCard);
	if (eType >= eCT_Max)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("onMoCard parse card type error so do not have this card = %u", nMoCard);
		return ;
	}
	addCardToVecAsc(m_vCards[eType], nMoCard);
	m_nNesetFetchedCard = nMoCard;
}

bool MJPlayerCard::onPeng(uint8_t nCard)
{
	auto eType = card_Type(nCard);
	if (eType >= eCT_Max)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("onPeng parse card type error so do not have this card = %u", nCard);
		return false;
	}
	
	auto& vCard = m_vCards[eType];
	auto nEraseCnt = 2;
	while (nEraseCnt-- < 0)
	{
		auto iter = std::find(vCard.begin(), vCard.end(), nCard);
		vCard.erase(iter);
	}

	addCardToVecAsc(m_vPenged,nCard);
	return true;
}

bool MJPlayerCard::onMingGang(uint8_t nCard, uint8_t nGangGetCard)
{
	auto eType = card_Type(nCard);
	if (eType >= eCT_Max)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("onMingGang parse card type error so do not have this card = %u", nCard);
		return false;
	}

	auto& vCard = m_vCards[eType];
	auto nEraseCnt = 3;
	while (nEraseCnt-- < 0)
	{
		auto iter = std::find(vCard.begin(), vCard.end(), nCard);
		vCard.erase(iter);
	}

	addCardToVecAsc(m_vGanged, nCard);
	
	// new get card ;
	auto eGetType = card_Type(nGangGetCard);
	addCardToVecAsc(m_vCards[eGetType],nGangGetCard);
	m_nNesetFetchedCard = nGangGetCard;
	return true;
}

bool MJPlayerCard::onAnGang(uint8_t nCard, uint8_t nGangGetCard)
{
	auto eType = card_Type(nCard);
	if (eType >= eCT_Max)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("onAnGang parse card type error so do not have this card = %u", nCard);
		return false;
	}

	auto& vCard = m_vCards[eType];
	auto nEraseCnt = 4;
	while (nEraseCnt-- < 0)
	{
		auto iter = std::find(vCard.begin(), vCard.end(), nCard);
		vCard.erase(iter);
	}

	addCardToVecAsc(m_vGanged, nCard);

	// new get card ;
	auto eGetType = card_Type(nGangGetCard);
	addCardToVecAsc(m_vCards[eGetType], nGangGetCard);
	m_nNesetFetchedCard = nGangGetCard;
	return true;
}

bool MJPlayerCard::onBuGang(uint8_t nCard, uint8_t nGangGetCard)
{
	return onMingGang(nCard, nGangGetCard);
}

bool MJPlayerCard::onEat(uint8_t nCard, uint8_t nWithA, uint8_t withB)
{
	if (!isHaveCard(nWithA) || !isHaveCard(withB))
	{
		CLogMgr::SharedLogMgr()->ErrorLog("do not have with card , can not eat a = %u , b = %u, c = %u",nCard,nWithA,withB);
		return false;
	}

	auto eT = card_Type(nCard);
	if (card_Type(nCard) != card_Type(nWithA))
	{
		CLogMgr::SharedLogMgr()->ErrorLog("not the same type , can not eat a = %u , b = %u, c = %u", nCard, nWithA, withB);
		return false;
	}

	auto iter = std::find(m_vCards[eT].begin(), m_vCards[eT].end(), nWithA);
	m_vCards[eT].erase(iter);
	iter = std::find(m_vCards[eT].begin(), m_vCards[eT].end(), withB);
	m_vCards[eT].erase(iter);

	// add to eat , should not sort 
	m_vEated.push_back(nCard);
	m_vEated.push_back(nWithA);
	m_vEated.push_back(withB);
	return true;
}

bool MJPlayerCard::onChuCard(uint8_t nChuCard)
{
	if (!isHaveCard(nChuCard))
	{
		CLogMgr::SharedLogMgr()->ErrorLog("you don't have this card , how can chu ?  = %u",nChuCard);
		return false;
	}
	auto eT = card_Type(nChuCard);
	auto iter = std::find(m_vCards[eT].begin(), m_vCards[eT].end(), nChuCard);
	m_vCards[eT].erase(iter);
	m_vChuedCard.push_back(nChuCard);
	return true;
}

bool MJPlayerCard::getHoldCard(VEC_CARD& vHoldCard)
{
	for (auto& vCards : m_vCards)
	{
		if (vCards.empty())
		{
			continue;
		}
		vHoldCard.insert(vHoldCard.end(),vCards.begin(),vCards.end());
	}
	return !vHoldCard.empty();
}

bool MJPlayerCard::getChuedCard(VEC_CARD& vChuedCard)
{
	vChuedCard.insert(vChuedCard.end(),m_vChuedCard.begin(),m_vChuedCard.end());
	return vChuedCard.empty() == false;
}

bool MJPlayerCard::getGangedCard(VEC_CARD& vGangCard)
{
	vGangCard.insert(m_vGanged.end(), m_vGanged.begin(), m_vGanged.end());
	return false == vGangCard.empty();
}

bool MJPlayerCard::getPengedCard(VEC_CARD& vPengedCard)
{
	vPengedCard.insert(m_vPenged.end(), m_vPenged.begin(), m_vPenged.end());
	return false == vPengedCard.empty();
}

bool MJPlayerCard::getEatedCard(VEC_CARD& vEatedCard)
{
	vEatedCard.insert(m_vEated.end(), m_vEated.begin(), m_vEated.end());
	return false == vEatedCard.empty();
}

uint32_t MJPlayerCard::getNewestFetchedCard()
{
	return m_nNesetFetchedCard;
}

void MJPlayerCard::addCardToVecAsc(VEC_CARD& vec, uint8_t nCard)
{
	auto iter = vec.begin();
	for (; iter < vec.end(); ++iter)
	{
		if ((*iter) >= nCard)
		{
			vec.insert(iter, nCard);
			return;
		}
	}
	vec.push_back(nCard);
}

void MJPlayerCard::getNotShuns(VEC_CARD vCard, VEC_NOT_SHUN& vNotShun)
{

}

bool MJPlayerCard::pickKeZiOut(VEC_CARD vCards, VEC_CARD& vKeZi, VEC_CARD& vLeftCard)
{
	vKeZi.clear();
	for (uint8_t nIdx = 0; (uint8_t)(nIdx + 2) < vCards.size();)
	{
		if (vCards[nIdx] == vCards[nIdx + 2])
		{
			vKeZi.push_back(vCards[nIdx]);
			// mark find result to 0 , for remove later 
			vCards[nIdx] = 0;
			vCards[nIdx + 1] = 0;
			vCards[nIdx + 2] = 0;

			nIdx = nIdx + 3;
		}
		else
		{
			++nIdx;
		}
	}

	if (vKeZi.empty())
	{
		return false;
	}

	auto iter = std::find(vCards.begin(), vCards.end(), 0);
	while (iter != vCards.end())
	{
		vCards.erase(iter);
		iter = std::find(vCards.begin(), vCards.end(), 0);
	}
	vLeftCard.clear();
	vLeftCard.swap(vCards);
	return true;
}

bool MJPlayerCard::pickNotShunZiOut(VEC_CARD vCardIgnorKeZi, VEC_NOT_SHUN& vNotShun)
{
	VEC_CARD vLeftToRight , vRightToLeft;
	vLeftToRight.assign(vCardIgnorKeZi.begin(),vCardIgnorKeZi.end());
	vRightToLeft.swap(vCardIgnorKeZi);

	auto pfn = [](VEC_CARD& vec, uint8_t nSeekValue, uint8_t& nFindIdx)->bool{
		for (uint8_t nIdx = 0; nIdx < vec.size(); ++nIdx)
		{
			if (vec[nIdx] == nSeekValue)
			{
				nFindIdx = nIdx;
				return true;
			}
		}
		return false;
	};


	auto pfnErase = [pfn](VEC_CARD& vec)
	{
		for (uint8_t nIdx = 0; nIdx < vec.size(); ++nIdx)
		{
			uint8_t nValue = vec[nIdx];
			if (0 == nValue)
			{
				continue;
			}
			uint8_t nValue1 = nValue + 1; uint8_t nIdx1 = 0;
			uint8_t nValue2 = nValue1 + 1; uint8_t nIdx2 = 0;
			if (pfn(vec, nValue1, nIdx1) && pfn(vec, nValue2, nIdx2))
			{
				vec[nIdx] = 0;
				vec[nIdx1] = 0;
				vec[nIdx2] = 0;
			}
		}

		// erase zero ;
		auto iter = std::find(vec.begin(), vec.end(), 0);
		while (iter != vec.end())
		{
			vec.erase(iter);
			iter = std::find(vec.begin(), vec.end(), 0);
		}
	};

	//-------left to right
	pfnErase(vLeftToRight);
	pfnErase(vRightToLeft);
	//---right to left 
	if (vLeftToRight.empty() == false)
	{
		stNotShunCard st;
		st.vCards.clear();
		st.vCards.swap(vLeftToRight);
		vNotShun.push_back(st);
	}

	if (vRightToLeft.empty() == false)
	{
		stNotShunCard st;
		st.vCards.clear();
		st.vCards.swap(vRightToLeft);
		vNotShun.push_back(st);
	}

	return (vLeftToRight.empty() == false) || (vRightToLeft.empty() == false);
}
