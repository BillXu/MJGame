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

bool MJPlayerCard::stNotShunCard::operator < (const stNotShunCard& v)const
{
	if (getSize() < v.getSize())
	{
		return true;
	}

	if (getSize() > v.getSize())
	{
		return false;
	}

	if (vCards.empty())
	{
		return false;
	}

	stNotShunCard self = *this;
	stNotShunCard nonConstObj = v;
	std::sort(self.vCards.begin(), self.vCards.end());
	std::sort(nonConstObj.vCards.begin(), nonConstObj.vCards.end());
	for (uint8_t nIdx = 0; nIdx < self.vCards.size(); ++nIdx)
	{
		if (self.vCards[nIdx] < nonConstObj.vCards[nIdx])
		{
			return true;
		}
	}

	return false;
}

uint8_t MJPlayerCard::stNotShunCard::getLackCardCntForShun()
{
	if (vCards.empty())
	{
		CLogMgr::SharedLogMgr()->ErrorLog("not shun must not be empty ? error ");
		return 0;
	}

	auto type = card_Type(vCards.front());
	auto bMustKe = ( type == eCT_Feng || eCT_Jian == type );

	uint8_t nLackCnt = 0;
	std::sort(vCards.begin(),vCards.end());
	for (uint8_t nIdx = 0; nIdx < vCards.size(); )
	{
		uint8_t nValue = vCards[nIdx];
		if ((uint8_t)(nIdx + 1) >= vCards.size())
		{
			nLackCnt += 2;
			break;
		}

		uint8_t nNextV = vCards[nIdx + 1];
		if (nNextV == nValue)
		{
			nLackCnt += 1;
			nIdx += 2;
			continue;
		}

		if ( bMustKe == false && (nNextV - nValue <= 2))
		{
			nLackCnt += 1;
			nIdx += 2;
			continue;
		}

		nLackCnt += 2;
		nIdx += 1;
	}
	return nLackCnt;
}


// mj player card ;
void MJPlayerCard::reset()
{
	VEC_CARD m_vCards[eCT_Max];
	for (auto& vC : m_vCards)
	{
		vC.clear();
	}
	m_vChuedCard.clear();
	m_vPenged.clear();
	m_vGanged.clear();
	m_vEated.clear();
	m_vEated.clear();
	m_nNesetFetchedCard = 0 ;
	m_nJIang = 0;
	m_nDanDiao = 0;
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
	auto eType = card_Type(nCard);
	if (eType >= eCT_Max)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("parse card type error ,canHuWitCard have this card = %u", nCard);
		return false;
	}

	addCardToVecAsc(m_vCards[eType], nCard);
	bool bSelfHu = isHoldCardCanHu();
	auto iter = std::find(m_vCards[eType].begin(), m_vCards[eType].end(),nCard);
	return bSelfHu;
}

bool MJPlayerCard::canAnGangWithCard(uint8_t nCard)
{
	auto eType = card_Type(nCard);
	if (eType >= eCT_Max)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("canAnGangWithCard parse card type error so do not have this card = %u", nCard);
		return false;
	}
	auto& vCard = m_vCards[eType];
	auto nCnt = std::count(vCard.begin(), vCard.end(), nCard);
	return nCard == 4;
}

bool MJPlayerCard::canBuGangWithCard(uint8_t nCard)
{
	auto eType = card_Type(nCard);
	if (eType >= eCT_Max)
	{
		CLogMgr::SharedLogMgr()->ErrorLog("canAnGangWithCard parse card type error so do not have this card = %u", nCard);
		return false;
	}
	auto& vCard = m_vCards[eType];
	auto nCnt = std::count(vCard.begin(), vCard.end(), nCard);
	auto npeng = std::count(m_vPenged.begin(), m_vPenged.end(), nCard);
	return nCard == 1 && 0 < npeng;
}

//bool MJPlayerCard::isTingPai()
//{
//	if (is7PairTing())
//	{
//		return true;
//	}
//
//	SET_NOT_SHUN vNotShun[eCT_Max];
//	uint8_t nTingType = eCT_None;
//	uint8_t nTingTypeB = eCT_None;
//	for (uint8_t nType = eCT_None; nType < eCT_Max; ++nType)
//	{
//		auto& vCard = m_vCards[nType];
//		if (vCard.empty())
//		{
//			continue;
//		}
//
//		if (!getNotShuns(vCard, vNotShun[nType], eCT_Feng == nType || eCT_Jian == nType))
//		{
//			if (eCT_None != nTingType && nTingTypeB != eCT_None )
//			{
//				return false;
//			}
//
//			if (eCT_None != nTingType)
//			{
//				nTingType = nType;
//			}
//
//			if (eCT_None != nTingTypeB)
//			{
//				nTingTypeB = nType;
//			}
//		}
//	}
//
//	if (eCT_None == nTingType && eCT_None == nTingTypeB )
//	{
//		CLogMgr::SharedLogMgr()->ErrorLog( "why all type hold card is shun zi ?" );
//	}
//
//	auto pfnIsJiang = [](SET_NOT_SHUN& vNot)->bool
//	{
//		auto iter = vNot.begin();
//		for (; iter != vNot.end(); ++iter)
//		{
//			auto refNot = *iter;
//			if (refNot.getLackCardCntForShun() == 1)
//			{
//				if (refNot.vCards[0] == refNot.vCards[1])
//				{
//					return true;
//				}
//			}
//		}
//		return false;
//	};
//
//	auto pfnIsNormalQue = [](SET_NOT_SHUN& vNot)->bool
//	{
//		auto iter = vNot.begin();
//		for (; iter != vNot.end(); ++iter)
//		{
//			auto refNot = *iter;
//			if (refNot.getLackCardCntForShun() == 1)
//			{
//				return true;
//			}
//		}
//		return false;
//	};
//
//	auto pfnIsNormalQueSelfContainJiang = [](SET_NOT_SHUN& vNot)->bool
//	{
//		auto iter = vNot.begin();
//		for (; iter != vNot.end(); ++iter)
//		{
//			auto refNot = *iter;
//			if (refNot.getLackCardCntForShun() == 2 && refNot.getSize() == 1 )
//			{
//				return true;
//			}
//
//			if (refNot.getLackCardCntForShun() == 2 && refNot.getSize() == 4 )
//			{
//				std::sort(refNot.vCards.begin(),refNot.vCards.end());
//				if ((refNot.vCards[0] == refNot.vCards[1]) && (refNot.vCards[3] - refNot.vCards[2] <= 2))
//				{
//					return true;
//				}
//
//				if ((refNot.vCards[2] == refNot.vCards[3]) && (refNot.vCards[1] - refNot.vCards[0] <= 2))
//				{
//					return true;
//				}
//			}
//		}
//		return false;
//	};
//
//	if (eCT_None != nTingType && eCT_None == nTingTypeB)
//	{
//		return pfnIsNormalQueSelfContainJiang(vNotShun[nTingType]);
//	}
//	else if (eCT_None == nTingType && eCT_None != nTingTypeB)
//	{
//		return pfnIsNormalQueSelfContainJiang(vNotShun[nTingTypeB]);
//	}
//	else if (eCT_None != nTingType && eCT_None != nTingTypeB)
//	{
//		return (pfnIsNormalQue(vNotShun[nTingTypeB]) && pfnIsJiang(vNotShun[nTingType])) || (pfnIsNormalQue(vNotShun[nTingType]) && pfnIsJiang(vNotShun[nTingTypeB]));
//	}
//		
//	return false;
//}
bool MJPlayerCard::isTingPai()
{
	if (is7PairTing())
	{
		return true;
	}

	if ( getMiniQueCnt(m_vCards) <= 1 )
	{
		return true;
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

//bool MJPlayerCard::isHoldCardCanHu()
//{
//	if (canHoldCard7PairHu())
//	{
//		return true;
//	}
//
//	SET_NOT_SHUN vNotShun[eCT_Max];
//	uint8_t nTingType = eCT_None;
//	for (uint8_t nType = eCT_None; nType < eCT_Max; ++nType)
//	{
//		auto& vCard = m_vCards[nType];
//		if (vCard.empty())
//		{
//			continue;
//		}
//
//		if (!getNotShuns(vCard, vNotShun[nType], eCT_Feng == nType || eCT_Jian == nType))
//		{
//			if (eCT_None != nTingType )
//			{
//				return false;
//			}
//			nTingType = nType;
//		}
//	}
//
//	if (eCT_None == nTingType )
//	{
//		CLogMgr::SharedLogMgr()->ErrorLog("why all type hold card is shun zi no jiang ? how to hu ?");
//		return false;
//	}
//
//	auto pfnIsJiang = [](SET_NOT_SHUN& vNot)->bool
//	{
//		auto iter = vNot.begin();
//		for (; iter != vNot.end(); ++iter)
//		{
//			auto refNot = *iter;
//			if (refNot.getLackCardCntForShun() == 1)
//			{
//				if (refNot.vCards[0] == refNot.vCards[1])
//				{
//					return true;
//				}
//			}
//		}
//		return false;
//	};
//
//	return pfnIsJiang(vNotShun[nTingType]);
//}
bool MJPlayerCard::isHoldCardCanHu()
{
	if (canHoldCard7PairHu())
	{
		return true;
	}

	if (getMiniQueCnt(m_vCards) == 0 )
	{
		return true;
	}
	return false;
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

bool MJPlayerCard::getNotShuns(VEC_CARD vCard, SET_NOT_SHUN& vNotShun, bool bMustKeZiShun )
{
	if (vCard.empty())
	{
		vNotShun.clear();
		return true;
	}
	// ignore ke zi 
	if (pickNotShunZiOutIgnoreKeZi(vCard, vNotShun))
	{
		vNotShun.clear();
		return true;
	}

	// not ignore ke zi 
	VEC_CARD vKeZi;
	VEC_CARD vLeftCard;
	pickKeZiOut(vCard,vKeZi,vLeftCard);
	if (vLeftCard.empty())
	{
		vNotShun.clear();
		return true;
	}

	if (bMustKeZiShun || vLeftCard.size() < 3 ) // this situation left card  must be  not shun ;
	{
		stNotShunCard stNot;
		stNot.vCards.swap(vLeftCard);
		return false;
	}

	// without kezi ,Left card , that not shun 
	if ( vKeZi.size() > 0 )
	{
		if ( pickNotShunZiOutIgnoreKeZi(vLeftCard, vNotShun))
		{
			vNotShun.clear();
			return true;
		}
	}

	
	// take part keZi into construct shun ;
	if (vKeZi.size() >= 1)
	{
		VEC_CARD vCheck;
		vCheck.assign(vLeftCard.begin(), vLeftCard.end());
		vCheck.push_back(vKeZi[0]);
		vCheck.push_back(vKeZi[0]);
		vCheck.push_back(vKeZi[0]);
		if (pickNotShunZiOutIgnoreKeZi(vCheck, vNotShun))
		{
			vNotShun.clear();
			return true;
		}
	}

	if (vKeZi.size() >= 2)
	{
		VEC_CARD vCheck;
		vCheck.assign(vLeftCard.begin(), vLeftCard.end());
		vCheck.push_back(vKeZi[1]);
		vCheck.push_back(vKeZi[1]);
		vCheck.push_back(vKeZi[1]);
		if (pickNotShunZiOutIgnoreKeZi(vCheck, vNotShun))
		{
			vNotShun.clear();
			return true;
		}
	}

	 if ( vKeZi.size() >= 3 )
	 {
		 VEC_CARD vCheck;
		 vCheck.assign(vLeftCard.begin(), vLeftCard.end());
		 vCheck.push_back(vKeZi[2]);
		 vCheck.push_back(vKeZi[2]);
		 vCheck.push_back(vKeZi[2]);
		 if (pickNotShunZiOutIgnoreKeZi(vCheck, vNotShun))
		 {
			 vNotShun.clear();
			 return true;
		 }

		 // 0,1 ; 1,2; 0,2 ;
		 vCheck.clear();
		 vCheck.assign(vLeftCard.begin(), vLeftCard.end());
		 vCheck.push_back(vKeZi[2]);
		 vCheck.push_back(vKeZi[2]);
		 vCheck.push_back(vKeZi[2]);

		 vCheck.push_back(vKeZi[0]);
		 vCheck.push_back(vKeZi[0]);
		 vCheck.push_back(vKeZi[0]);
		 if (pickNotShunZiOutIgnoreKeZi(vCheck, vNotShun))
		 {
			 vNotShun.clear();
			 return true;
		 }

		 // 0,1 ; 1,2; 0,2 ;
		 vCheck.clear();
		 vCheck.assign(vLeftCard.begin(), vLeftCard.end());
		 vCheck.push_back(vKeZi[2]);
		 vCheck.push_back(vKeZi[2]);
		 vCheck.push_back(vKeZi[2]);

		 vCheck.push_back(vKeZi[1]);
		 vCheck.push_back(vKeZi[1]);
		 vCheck.push_back(vKeZi[1]);
		 if (pickNotShunZiOutIgnoreKeZi(vCheck, vNotShun))
		 {
			 vNotShun.clear();
			 return true;
		 }

		 vCheck.clear();
		 vCheck.assign(vLeftCard.begin(), vLeftCard.end());
		 vCheck.push_back(vKeZi[0]);
		 vCheck.push_back(vKeZi[0]);
		 vCheck.push_back(vKeZi[0]);

		 vCheck.push_back(vKeZi[1]);
		 vCheck.push_back(vKeZi[1]);
		 vCheck.push_back(vKeZi[1]);
		 if (pickNotShunZiOutIgnoreKeZi(vCheck, vNotShun))
		 {
			 vNotShun.clear();
			 return true;
		 }
	 }
	return false;
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
		vLeftCard.swap(vCards);
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

bool MJPlayerCard::pickNotShunZiOutIgnoreKeZi(VEC_CARD vCardIgnorKeZi, SET_NOT_SHUN& vNotShun)
{
	VEC_CARD vAscendSort, vDescendSort;
	vAscendSort.assign(vCardIgnorKeZi.begin(),vCardIgnorKeZi.end());
	std::sort(vAscendSort.begin(), vAscendSort.end());   // < asc

	vDescendSort.swap(vCardIgnorKeZi);  //desc
	std::sort(vDescendSort.begin(), vDescendSort.end(), [](const uint8_t& refLeft, const uint8_t& refLeftRight)->bool{ return refLeft > refLeftRight; });   // < asc

	auto pfn = [](VEC_CARD& vec, uint8_t nSeekValue, uint8_t& nFindIdx, uint8_t nIdxStart )->bool{
		for (uint8_t nIdx = nIdxStart; nIdx < vec.size(); ++nIdx)
		{
			if (vec[nIdx] == nSeekValue)
			{
				nFindIdx = nIdx;
				return true;
			}
		}
		return false;
	};


	auto pfnErase = [pfn](VEC_CARD& vec,bool bAsc )
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
			if (!bAsc)
			{
				nValue1 = nValue - 1;
				nValue2 = nValue1 - 1;
			}
			if (pfn(vec, nValue1, nIdx1,nIdx + 1 ) && pfn(vec, nValue2, nIdx2,nIdx + 1 ))
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
	pfnErase(vAscendSort,true);
	if (vAscendSort.empty())
	{
		return true;
	}

	pfnErase(vDescendSort,false);
	if ( vDescendSort.empty())
	{
		return true;
	}

	//---right to left 
	if (vAscendSort.empty() == false)
	{
		stNotShunCard st;
		st.vCards.clear();
		st.vCards.swap(vAscendSort);
		vNotShun.insert(st);
	}

	if (vDescendSort.empty() == false)
	{
		stNotShunCard st;
		st.vCards.clear();
		st.vCards.swap(vDescendSort);
		vNotShun.insert(st);
	}

	return false;
}

//bool MJPlayerCard::is7PairTing()
//{
//	if (m_vPenged.empty() == false || false == m_vGanged.empty() || false == m_vEated.empty() )
//	{
//		return false;
//	}
//
//	uint8_t nPairCnt = 0;
//	for (auto& vCards : m_vCards)
//	{
//		for (uint8_t nIdx = 0; (uint8_t)(nIdx + 1) < vCards.size(); )
//		{
//			if (vCards[nIdx] == vCards[1 + nIdx])
//			{
//				++nPairCnt;
//				nIdx += 2;
//			}
//			else
//			{
//				++nIdx;
//			}
//		}
//	}
//
//	return nPairCnt == 6;
//}
//
//bool MJPlayerCard::canHoldCard7PairHu()
//{
//	if (m_vPenged.empty() == false || false == m_vGanged.empty() || false == m_vEated.empty())
//	{
//		return false;
//	}
//
//	uint8_t nPairCnt = 0;
//	for (auto& vCards : m_vCards)
//	{
//		for (uint8_t nIdx = 0; (uint8_t)(nIdx + 1) < vCards.size(); ++nIdx)
//		{
//			if (vCards[nIdx] == vCards[1 + nIdx])
//			{
//				++nPairCnt;
//			}
//		}
//	}
//
//	return nPairCnt == 7;
//}

bool MJPlayerCard::is7PairTing()
{
	if (m_vPenged.empty() == false || false == m_vGanged.empty() || false == m_vEated.empty())
	{
		return false;
	}

	if (get7PairQueCnt(m_vCards) <= 1)
	{
		return true;
	}
	return false;
}

bool MJPlayerCard::canHoldCard7PairHu()
{
	if (m_vPenged.empty() == false || false == m_vGanged.empty() || false == m_vEated.empty())
	{
		return false;
	}

	if (get7PairQueCnt(m_vCards) == 0 )
	{
		return true;
	}
	return false;
}

uint8_t MJPlayerCard::getMiniQueCnt( VEC_CARD vCards[eCT_Max] )
{
	// prepare cards without cai shen 
	SET_NOT_SHUN vNotShun[eCT_Max];
	for (uint8_t nType = eCT_None; nType < eCT_Max; ++nType)
	{
		auto& vCard = vCards[nType];
		if (vCard.empty())
		{
			continue;
		}

		getNotShuns(vCard, vNotShun[nType], eCT_Feng == nType || eCT_Jian == nType);
	}

	// when find dandiao 
	uint8_t nQueCnt = 0;
	m_nJIang = 0;
	m_nDanDiao = 0;
	for (auto& vRefNotShun : vNotShun)
	{
		uint8_t nTemp;
		nQueCnt += getLestQue(vRefNotShun, false, m_nDanDiao == 0, nTemp, m_nDanDiao);
	}
	CLogMgr::SharedLogMgr()->SystemLog(" fand dan diao mode que cnt = %u value = %u", nQueCnt, m_nDanDiao);
	if (m_nDanDiao)
	{
		return nQueCnt;
	}

	if (m_nDanDiao == 0)  // can not dandiao 
	{
		uint8_t nJiang = 0;
		uint8_t nWhenJiangQueCnt = 0;
		nQueCnt = 0;
		for (auto& vRefNotShun : vNotShun)
		{
			uint8_t nTemp;
			nQueCnt += getLestQue(vRefNotShun, m_nJIang == 0, false, m_nJIang, nTemp);
		}

		CLogMgr::SharedLogMgr()->SystemLog(" fand jiang mode que cnt = %u value = %u", nQueCnt, m_nJIang);
		if (m_nJIang)
		{
			return nQueCnt;
		}
	}

	CLogMgr::SharedLogMgr()->SystemLog(" no jiang , no dandiao cnt = %u ", nQueCnt);
	// no jiang , no dandiao 
	nQueCnt += 2;
	return nQueCnt;
}

uint8_t MJPlayerCard::get7PairQueCnt( VEC_CARD vCards[eCT_Max])
{
	uint8_t nUnpairCnt = 0;
	for (uint8_t nType = eCT_None; nType < eCT_Max; ++nType)
	{
		auto& vCard = vCards[nType];
		if (vCard.empty())
		{
			continue;
		}

		for (uint8_t nIdx = 0; nIdx < vCard.size();)
		{
			auto nNext = 0; 
			if (uint8_t(1 + nIdx) < vCard.size())
			{
				nNext = vCard[1 + nIdx];
			}

			if (vCard[nIdx] == nNext )
			{
				nIdx += 2;
			}
			else
			{
				++nUnpairCnt;
				++nIdx;
			}
		}
	}
	return nUnpairCnt;
}

uint8_t MJPlayerCard::getLestQue(SET_NOT_SHUN& vNotShun, bool bFindJiang, bool bFindDanDiao, uint8_t& nFiandJiang, uint8_t& nFindDanDiao)
{
	if (vNotShun.empty())
	{
		return 0;
	}

	auto pfunFindQueCnt = [](stNotShunCard& stNotShun, bool bFindJiang, bool bFindDanDiao, uint8_t& nFiandJiang, uint8_t& nFindDanDiao)->uint8_t
	{
		auto& vCards = stNotShun.vCards;
		if (vCards.empty())
		{
			CLogMgr::SharedLogMgr()->ErrorLog("not shun must not be empty ? error ");
			return 0;
		}

		auto type = card_Type(vCards.front());
		auto bMustKe = (type == eCT_Feng || eCT_Jian == type);

		uint8_t nLackCnt = 0;
		std::sort(vCards.begin(), vCards.end());
		for (uint8_t nIdx = 0; nIdx < vCards.size();)
		{
			uint8_t nValue = vCards[nIdx];
			if ((uint8_t)(nIdx + 1) >= vCards.size())  // last single card ;
			{
				nLackCnt += 2;
				if (bFindDanDiao)
				{
					nLackCnt -= 1;
					bFindDanDiao = false;
					nFindDanDiao = nValue;
				}

				break;
			}

			uint8_t nNextV = vCards[nIdx + 1];
			if (nNextV == nValue)
			{
				nLackCnt += 1;
				nIdx += 2;
				if (bFindJiang)
				{
					nLackCnt -= 1;
					bFindJiang = false;
					nFiandJiang = nValue;
				}
				continue;
			}

			if (bMustKe == false && (nNextV - nValue <= 2))
			{
				nLackCnt += 1;
				nIdx += 2;
				continue;
			}

			nLackCnt += 2;
			nIdx += 1;
			if (bFindDanDiao)
			{
				nLackCnt -= 1;
				bFindDanDiao = false;
				nFindDanDiao = nValue;
			}
		}
		return nLackCnt;
	};

	uint8_t nLesetQue = 100;
	uint8_t nFedJIangResult = 0;
	uint8_t nFedDanResult = 0;

	auto iter = vNotShun.begin();
	for (; iter != vNotShun.end(); ++iter)
	{
		uint8_t nFedJIang = 0;
		uint8_t nFedDan = 0;
		stNotShunCard stQ = *iter;
		auto nQuenCnt = pfunFindQueCnt(stQ, bFindJiang, bFindDanDiao, nFedJIang, nFedDan);
		if (nQuenCnt < nLesetQue)
		{
			nLesetQue = nQuenCnt;
			nFedJIangResult = nFedJIang;
			nFedDanResult = nFedDan;
		}
	}

	nFiandJiang = nFedJIangResult;
	nFindDanDiao = nFedDanResult;
	return nLesetQue;
}

