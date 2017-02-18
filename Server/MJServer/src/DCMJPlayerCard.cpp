#include "DCMJPlayerCard.h"
#include "log4z.h"
#include "MJCard.h"
DCMJPlayerCard::DCMJPlayerCard()
{
	reset();
	m_isEnableYao = false;
}

void DCMJPlayerCard::reset()
{
	MJPlayerCard::reset();
	m_tCheckHuInfo.reset();
	m_isHaveTianHuBao = false;
}

void DCMJPlayerCard::setEnableYao(bool isEanbleYao)
{
	m_isEnableYao = m_isEnableYao;
}

void DCMJPlayerCard::setIsHaveTianHuBao(bool isHaveTianHuBao)
{
	m_isHaveTianHuBao = isHaveTianHuBao;
}

bool DCMJPlayerCard::isCommonHoldCardCanHu()
{
	if (canHoldCard7PairHu())
	{
		return true;
	}

	if (getMiniQueCnt(m_vCards, false) == 0)
	{
		return true;
	}
	return false;
}

bool DCMJPlayerCard::isTingPai()
{
	if (is7PairTing())
	{
		return true;
	}

	auto nct = getMiniQueCnt(m_vCards, false);
	if ( nct <= 1)
	{
		if (0 == nct)
		{
			LOGFMTE("this is already hu ? why you check ting ?");
		}
		return true;
	}
	return false;
}

bool DCMJPlayerCard::isHoldCardCanHu()
{
	bool isUsedBao = false;
	bool isDanDiao = false;
	m_tCheckHuInfo.eHuType = eFanxing_Max;
	if (isDuiDuiHu(isUsedBao, isDanDiao))
	{
		m_tCheckHuInfo.isUsedBao = isUsedBao;
		m_tCheckHuInfo.eHuType = eFanxing_DC_DuiDuiHu;
		if (isDanDiao)
		{
			m_tCheckHuInfo.eHuType = eFanxing_DC_DuiDuiHu_DanDiao;
		}
		return true;
	}

	// check qi zi quan ;
	bool isQiZiQuan = false;
	isUsedBao = false;
	if (isNanHu(isHaveGangBao() || isHaveTianHuBao(), isUsedBao, isQiZiQuan) )
	{
		m_tCheckHuInfo.isUsedBao = isUsedBao;
		m_tCheckHuInfo.eHuType = eFanxing_DC_NanHu;
		if (isDanDiao)
		{
			m_tCheckHuInfo.eHuType = eFanxing_DC_NanHu_7ZiQuan;
		}
		return true;
	}

	// check yao hu 
	isUsedBao = false;
	if ( m_isEnableYao && isYaoHu(isHaveGangBao() || isHaveTianHuBao(), isUsedBao) )
	{
		m_tCheckHuInfo.isUsedBao = isUsedBao;
		m_tCheckHuInfo.eHuType = eFanxing_DC_YaoHu;
		return true;
	}

	// check la ji hu 
	if (isCommonHoldCardCanHu())
	{
		m_tCheckHuInfo.isUsedBao = false;
		m_tCheckHuInfo.eHuType = eFanxing_DC_PingHu;
		if (canHoldCard7PairHu())
		{
			m_tCheckHuInfo.eHuType = eFanxing_DC_QiDui;
		}
		return true;
	}

	if ( (isHaveGangBao() == false)  && (false == isHaveTianHuBao() ) )
	{
		return false;
	}

	auto pfunCheckTing = [this](uint8_t nCheckCard)->bool
	{
		auto nType = card_Type(nCheckCard);
		auto iter = std::find(m_vCards[nType].begin(), m_vCards[nType].end(), nCheckCard);
		if (iter == m_vCards[nType].end())
		{
			LOGFMTE("why hold card not have this card ? %u", nCheckCard);
			return false;
		}

		// remove this card ;
		m_vCards[nType].erase(iter);
		bool bRet = isTingPai();
		// give back card ;
		addCardToVecAsc(m_vCards[nType], nCheckCard);
		return bRet;
	};

	VEC_CARD vHold;
	getHoldCard(vHold);
	for (auto& nCheckCard : vHold)
	{
		if (pfunCheckTing(nCheckCard))
		{
			m_tCheckHuInfo.isUsedBao = true;
			m_tCheckHuInfo.eHuType = eFanxing_DC_PingHu;
			return true;
		}
	}

	return false;
}

bool DCMJPlayerCard::onChuCard(uint8_t nChuCard)
{
	MJPlayerCard::onChuCard(nChuCard);
	m_tCheckHuInfo.reset();
	setIsHaveTianHuBao(false);
	return true;
}

bool DCMJPlayerCard::onMingGang(uint8_t nCard, uint8_t nGangGetCard)
{
	MJPlayerCard::onMingGang(nCard, nGangGetCard);
	++m_tCheckHuInfo.nContinueGangTime;
	return true;
}

bool DCMJPlayerCard::onAnGang(uint8_t nCard, uint8_t nGangGetCard)
{
	MJPlayerCard::onAnGang(nCard, nGangGetCard);
	++m_tCheckHuInfo.nContinueGangTime;
	return true;
}

bool DCMJPlayerCard::onBuGang(uint8_t nCard, uint8_t nGangGetCard)
{
	MJPlayerCard::onBuGang(nCard, nGangGetCard);
	++m_tCheckHuInfo.nContinueGangTime;
	return true;
}

bool DCMJPlayerCard::isDuiDuiHu(bool& isUsedBao, bool& isDanDiao)
{
	isUsedBao = false;
	isDanDiao = false;
	VEC_CARD vHoldCard;
	getHoldCard(vHoldCard);
	if (vHoldCard.size() == 2) // dan diao ;
	{
		isDanDiao = true;
		isUsedBao = (vHoldCard[0] != vHoldCard[1]);
		return true;
	}

	// remove all ke zi from hold card 
	for (uint8_t nIdx = 0; (nIdx + 2) < vHoldCard.size(); )
	{
		if (vHoldCard[nIdx] == vHoldCard[nIdx + 2])
		{
			vHoldCard[nIdx] = 0;
			vHoldCard[nIdx + 1] = 0;
			vHoldCard[nIdx + 2] = 0;
			nIdx += 3;
		}
		else
		{
			++nIdx;
		}
	}

	// remove zero ;
	do
	{
		auto iter = std::find(vHoldCard.begin(),vHoldCard.end(),0);
		if (iter == vHoldCard.end())
		{
			break;
		}
		vHoldCard.erase(iter);
	} while (1);

	if (vHoldCard.size() == 2)
	{
		isUsedBao = (vHoldCard[0] != vHoldCard[1]);
		return true;
	}

	if (vHoldCard.size() != 5) // not duidui hu ge shi 
	{
		return false;
	}
	
	// remove the pair 
	for (uint8_t nIdx = 0; (nIdx + 1) < vHoldCard.size();)
	{
		if (vHoldCard[nIdx] == vHoldCard[nIdx + 1])
		{
			vHoldCard[nIdx] = 0;
			vHoldCard[nIdx + 1] = 0;
			nIdx += 2;
		}
		else
		{
			++nIdx;
		}
	}

	// remove zero ;
	do
	{
		auto iter = std::find(vHoldCard.begin(), vHoldCard.end(), 0);
		if (iter == vHoldCard.end())
		{
			break;
		}
		vHoldCard.erase(iter);
	} while (1);

	// must used bao ;
	isUsedBao = true;
	return (vHoldCard.size() == 1);
}

bool DCMJPlayerCard::isNanHu(bool isHaveBao, bool& isUsedBao, bool& isQiZiQuan)
{
	isUsedBao = false;
	// get peng ,
	VEC_CARD vtemp;
	getPengedCard(vtemp);
	if (!vtemp.empty())
	{
		return false;
	}

	// get an gang 
	vtemp.clear();
	getAnGangedCard(vtemp);
	if (!vtemp.empty())
	{
		return false;
	}

	// get gang 
	vtemp.clear();
	getGangedCard(vtemp);
	if (!vtemp.empty())
	{
		return false;
	}

	// check qi zi quan ;
	isQiZiQuan = false;
	do
	{
		auto& vFeng = m_vCards[eCT_Feng];
		auto vJian = m_vCards[eCT_Jian];
		if (vFeng.size() != 4 || vJian.size() != 3)
		{
			break;
		}

		for (uint8_t nIdx = 0; nIdx < 2; ++nIdx)
		{
			if ((vFeng[nIdx] + 1) != vFeng[nIdx + 1] || (vJian[nIdx] + 1) != vJian[nIdx + 1])
			{
				break;
			}
		}

		if ((vFeng[2] + 1) != vFeng[3])
		{
			break;
		}

		isQiZiQuan = true;
	} while (0);

	if (isHaveBao == false)
	{
		isHaveBao = true;
	}
	// check nan pai 
	for (auto& vCard : m_vCards)
	{
		if (vCard.empty())
		{
			continue;
		}

		auto nType = card_Type(vCard.front());
		bool isZiPai = (nType == eCT_Feng) || (eCT_Jian == nType );

		for (uint8_t nIdx = 0; (nIdx + 1) < vCard.size(); ++nIdx)
		{
			if ( !isZiPai )
			{
				if ( (vCard[nIdx] + 2) >= vCard[nIdx + 1])
				{
					if (isHaveBao )
					{
						isUsedBao = true;
						isHaveBao = false;
						continue;
					}

					return false;
				}
			}
			else
			{
				if ( isQiZiQuan )  // 7 zi quan , skip check zi pai ;
				{
					break;
				}

				if ( vCard[nIdx] == vCard[nIdx + 1])
				{
					if (isHaveBao)
					{
						isUsedBao = true;
						isHaveBao = false;
						continue;
					}
					return false;
				}
			}
		}
	}

	return true;
}

bool DCMJPlayerCard::isYaoHu(bool isHaveBao, bool& bIsUsedBao)
{
	bIsUsedBao = false;
	// get peng ,
	VEC_CARD vAllCard, vtemp;
	getPengedCard(vAllCard);

	// get an gang 
	getAnGangedCard(vtemp);
	vAllCard.insert(vAllCard.end(), vtemp.begin(), vtemp.end());
	vtemp.clear();

	// get gang 
	getGangedCard(vtemp);
	vAllCard.insert(vAllCard.end(), vtemp.begin(), vtemp.end());
	vtemp.clear();

	// get hold
	getHoldCard(vtemp);
	vAllCard.insert(vAllCard.end(), vtemp.begin(), vtemp.end());
	vtemp.clear();

	// check hold card ;
	for (auto& nCard : vAllCard)
	{
		auto type = card_Type(nCard);
		auto nValue = card_Value(nCard);
		bool isZi = (type == eCT_Feng) || (eCT_Jian == type);
		if (nValue != 1 && 9 != nValue && ( isZi == false ))
		{
			if (isHaveBao)  // bao only have once 
			{
				isHaveBao = false;
				bIsUsedBao = true;
				continue;
			}

			return false;
		}
	}

	return true;
}

bool DCMJPlayerCard::canEatCard(uint8_t nCard, uint8_t& nWithA, uint8_t& withB)
{
	return false;
}

bool DCMJPlayerCard::canHuWitCard(uint8_t nCard)
{
	return false;
}

bool DCMJPlayerCard::onDoHu(eFanxingType& nType, bool& isUsedBao, uint8_t& nBeishu, uint8_t& nGangCnt)
{
	if (m_tCheckHuInfo.isCanHu() == false)
	{
		return false;
	}

	nGangCnt = m_tCheckHuInfo.nContinueGangTime;
	nType = m_tCheckHuInfo.eHuType;
	isUsedBao = m_tCheckHuInfo.isUsedBao;
	nBeishu = 1;
	switch (m_tCheckHuInfo.eHuType)
	{
	case eFanxing_DC_NanHu:
	case eFanxing_DC_PingHu:
	case eFanxing_DC_YaoHu:
	{
		nBeishu = 1;
	}
	break;
	case eFanxing_DC_DuiDuiHu:
	{
		nBeishu = 2;
	}
	break;
	case eFanxing_DC_DuiDuiHu_DanDiao:
	{
		nBeishu = 4;
	}
	break;
	case eFanxing_DC_NanHu_7ZiQuan:
	{
		nBeishu = 2;
	}
	break;
	case eFanxing_DC_QiDui:
	{
		nBeishu = 4;
	}
	break;
	default:
		LOGFMTE("unknown hu type = %u",m_tCheckHuInfo.eHuType);
		return false;
	}

	for (uint8_t nGangCnt = 0; nGangCnt < m_tCheckHuInfo.nContinueGangTime; ++nGangCnt)
	{
		nBeishu *= 2;
	}

	if (isUsedBao == false)
	{
		return true;
	}

	if (!isHaveTianHuBao())
	{
		if (nBeishu < 2)
		{
			LOGFMTE("how you hu , no tian hu bao , no gang bao ? ");
			nBeishu = 1;
			return false;
		}

		nBeishu /= 2;
	}
	return true;
}

bool DCMJPlayerCard::isHaveGangBao()
{
	return m_tCheckHuInfo.nContinueGangTime > 0;
}

bool DCMJPlayerCard::isHaveTianHuBao()
{
	return m_isHaveTianHuBao;
}