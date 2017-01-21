#include "MJCard.h"
#include <cassert>
#include "log4z.h"
#include "json/json.h"
#include "makeCardConfig.h"
uint8_t CMJCard::getCard()
{
	if ( isEmpty() )
	{
		return 0 ;
	}

	return m_vAllCards[m_nCurCardIdx++] ;
}

uint8_t CMJCard::getLeftCardCount()
{
	if ( m_vAllCards.size() <= m_nCurCardIdx )
	{
		return 0 ;
	}

	return m_vAllCards.size() - m_nCurCardIdx ;
}

void CMJCard::shuffle(bool bMake )
{
	uint16_t n = 0 ;
	for ( uint16_t i = 0 ; i < m_vAllCards.size() - 2 ; ++i )
	{
		n = rand() % ( m_vAllCards.size() - i - 1 ) + i + 1    ;
		m_vAllCards[i] = m_vAllCards[n] + m_vAllCards[i] ;
		m_vAllCards[n] = m_vAllCards[i] - m_vAllCards[n] ;
		m_vAllCards[i] = m_vAllCards[i] - m_vAllCards[n] ;
	}

	m_nCurCardIdx = 0 ;

	//if ( bMake == false)
	//{
	//	debugPokerInfo();
	//	return;
	//}

	//if (CMakeCardConfig::getInstance()->getMakeCardRate() < rand() % 100)
	//{
	//	return ;
	//}

	//LOGFMTD("situation make card ");
	//auto pfuncArraFind = [](VEC_UINT8& vVec, uint8_t nStarIdx , uint8_t nFind)->uint8_t
	//{
	//	for (uint8_t nIdx = nStarIdx; nIdx < vVec.size(); ++nIdx )
	//	{
	//		auto nt = card_Type(vVec[nIdx]);
	//		if ( nt == nFind)
	//		{
	//			return nIdx;
	//		}
	//	}
	//	return (uint8_t)-1;
	//};

	//auto pPrePareCard = [this, pfuncArraFind](uint8_t nPlayerIdx)
	//{
	//	uint8_t nCardStartIdx = nPlayerIdx * 13;
	//	uint8_t ntype = rand() % eCT_Tiao + 1 ;
	//	uint8_t nCardCnt = 0;
	//	uint8_t nRate = rand() % 100;

	//	uint8_t nRate9 = CMakeCardConfig::getInstance()->get9CardRate();
	//	uint8_t nRate8 = CMakeCardConfig::getInstance()->get8CardRate();
	//	uint8_t nRate7 = CMakeCardConfig::getInstance()->get7CardRate();
	//	int8_t nRateNone = 100 - nRate9 - nRate8 - nRate7;
	//	if (nRateNone < 0)
	//	{
	//		nRateNone = 0;
	//	}

	//	if ( nRate > (nRate8 + nRate7 + nRateNone) )
	//	{
	//		nCardCnt = 9;
	//	}
	//	else if (nRate > (nRate7 + nRateNone) )
	//	{
	//		nCardCnt = 8;
	//	}
	//	else if (nRate > nRateNone )
	//	{
	//		nCardCnt = 7;
	//	}

	//	if (0 == nCardCnt)
	//	{
	//		return;
	//	}

	//	for (uint8_t nCnt = 0; nCnt < nCardCnt;)
	//	{
	//		//uint8_t nFindCard = make_Card_Num((eMJCardType)ntype, (rand() % 9 + 1));
	//		uint8_t curIdx = nCardStartIdx + nCnt;
	//		auto findIdx = pfuncArraFind(m_vAllCards, curIdx, ntype);
	//		if ((uint8_t)-1 == findIdx)
	//		{
	//			LOGFMTE("can not find proper card , so sorry for system type = %u",ntype);
	//			continue;
	//		}

	//		if (findIdx != curIdx) // do switch 
	//		{
	//			auto temp = m_vAllCards[findIdx];
	//			m_vAllCards[findIdx] = m_vAllCards[curIdx];
	//			m_vAllCards[curIdx] = temp;
	//		}
	//		++nCnt;
	//	}

	if (!bMake)
	{
		debugPokerInfo();
	}
}

void CMJCard::makeCardForPlayer(uint8_t nPlayerIdx, bool isRobot)
{
	LOGFMTD("situation make card player idx = %u , isRobot = %u",nPlayerIdx,isRobot );
	auto pfuncArraFind = [](VEC_UINT8& vVec, uint8_t nStarIdx, uint8_t nFind)->uint8_t
	{
		for (uint8_t nIdx = nStarIdx; nIdx < vVec.size(); ++nIdx)
		{
			auto nt = card_Type(vVec[nIdx]);
			if (nt == nFind)
			{
				return nIdx;
			}
		}
		return (uint8_t)-1;
	};

	uint8_t nCardStartIdx = nPlayerIdx * 13;
	uint8_t ntype = rand() % eCT_Tiao + 1;
	uint8_t nCardCnt = 0;
	uint8_t nRate = rand() % 100;

	uint8_t nRate9 = CMakeCardConfig::getInstance()->get9CardRate(isRobot);
	uint8_t nRate8 = CMakeCardConfig::getInstance()->get8CardRate(isRobot);
	uint8_t nRate7 = CMakeCardConfig::getInstance()->get7CardRate(isRobot);
	int8_t nRateNone = 100 - nRate9 - nRate8 - nRate7;
	if (nRateNone < 0)
	{
		nRateNone = 0;
	}

	if (nRate >(nRate8 + nRate7 + nRateNone))
	{
		nCardCnt = 9;
	}
	else if (nRate > (nRate7 + nRateNone))
	{
		nCardCnt = 8;
	}
	else if (nRate > nRateNone)
	{
		nCardCnt = 7;
	}

	if (0 == nCardCnt)
	{
		return;
	}

	for (uint8_t nCnt = 0; nCnt < nCardCnt;)
	{
		//uint8_t nFindCard = make_Card_Num((eMJCardType)ntype, (rand() % 9 + 1));
		uint8_t curIdx = nCardStartIdx + nCnt;
		auto findIdx = pfuncArraFind(m_vAllCards, curIdx, ntype);
		if ((uint8_t)-1 == findIdx)
		{
			LOGFMTE("can not find proper card , so sorry for system type = %u", ntype);
			continue;
		}

		if (findIdx != curIdx) // do switch 
		{
			auto temp = m_vAllCards[findIdx];
			m_vAllCards[findIdx] = m_vAllCards[curIdx];
			m_vAllCards[curIdx] = temp;
		}
		++nCnt;
	}

	// shuffle the cards ;
	uint16_t n = 0, ncurs = 0;
	for (uint16_t i = 0; i < 11; ++i)
	{
		ncurs = nCardStartIdx + i;
		n = rand() % (13 - i - 1) + ncurs + 1;
		m_vAllCards[ncurs] = m_vAllCards[n] + m_vAllCards[ncurs];
		m_vAllCards[n] = m_vAllCards[ncurs] - m_vAllCards[n];
		m_vAllCards[ncurs] = m_vAllCards[ncurs] - m_vAllCards[n];
	}
}

void CMJCard::debugCardInfo()
{
	printf("card Info: \n");
	for ( uint8_t nCard : m_vAllCards )
	{
		printf("cardNumber : %u\n",nCard) ;
	}
	printf("card info end \n\n") ;
}

void CMJCard::initAllCard( eMJGameType eType )
{
	m_vAllCards.clear() ;
	m_nCurCardIdx = 0 ;

	assert(eType < eMJ_Max && eType >= eMJ_None && "invalid card type" );
	if ( eMJ_TwoBird == eType )
	{
		initTwoBirdCard() ;
		return ;
	}

	m_eMJGameType = eType ;

	// every card are 4 count 
	for ( uint8_t nCnt = 0 ; nCnt < 4 ; ++nCnt )
	{
		// add base 
		uint8_t vType[3] = { eCT_Wan,eCT_Tiao,eCT_Tong } ;
		for ( uint8_t nType : vType )
		{
			for ( uint8_t nValue = 1 ; nValue <= 9 ; ++nValue )
			{
				m_vAllCards.push_back(makeCardNumber((eMJCardType)nType,nValue)) ;
			}
		}

		if ( eMJ_COMMON == m_eMJGameType || eMJ_WZ == m_eMJGameType )
		{
			// add feng , add ke
			for ( uint8_t nValue = 1 ; nValue <= 4 ; ++nValue )
			{
				m_vAllCards.push_back(makeCardNumber(eCT_Feng,nValue)) ;
			}

			for ( uint8_t nValue = 1 ; nValue <= 3 ; ++nValue )
			{
				m_vAllCards.push_back(makeCardNumber(eCT_Jian,nValue)) ;
			}
		}
	}
}

void CMJCard::initTwoBirdCard()
{
	m_vAllCards.clear() ;
	m_nCurCardIdx = 0 ;
	m_eMJGameType = eMJ_TwoBird ;

	// every card are 4 count 
	for ( uint8_t nCnt = 0 ; nCnt < 4 ; ++nCnt )
	{
		// add base 
		uint8_t vType[] = { eCT_Wan } ;
		for ( uint8_t& nType : vType )
		{
			for ( uint8_t nValue = 1 ; nValue <= 9 ; ++nValue )
			{
				m_vAllCards.push_back(makeCardNumber((eMJCardType)nType,nValue)) ;
			}
		}

		// add feng , add jian
		for ( uint8_t nValue = 1 ; nValue <= 4 ; ++nValue )
		{
			m_vAllCards.push_back(makeCardNumber(eCT_Feng,nValue)) ;
		}

		for ( uint8_t nValue = 1 ; nValue <= 3 ; ++nValue )
		{
			m_vAllCards.push_back(makeCardNumber(eCT_Jian,nValue)) ;
		}
	}
}

eMJGameType CMJCard::getGameType()
{
	return m_eMJGameType ;
}

bool CMJCard::isEmpty()
{
	return getLeftCardCount() <= 0 ;
}

void CMJCard::debugPokerInfo()
{
	Json::Value js;
	for (auto& ref : m_vAllCards)
	{
		js[js.size()] = ref;
	}

	Json::StyledWriter jswriter;
	auto strJs = jswriter.write(js);
	LOGFMTD("poker is : %s",strJs.c_str());
}

eMJCardType CMJCard::parseCardType(uint8_t nCardNum)
{
	uint8_t nType = nCardNum & 0xF0 ;
	nType = nType >> 4 ;
	if ( (nType < eCT_Max && nType > eCT_None) == false )
	{
		LOGFMTE("parse card type error , cardnum = %u",nCardNum) ;
	}
	assert(nType < eCT_Max && nType > eCT_None && "invalid card type" );
	return (eMJCardType)nType ;
}

uint8_t CMJCard::parseCardValue(uint8_t nCardNum )
{
	return  (nCardNum & 0xF) ;
}

uint8_t CMJCard::makeCardNumber(eMJCardType eType,uint8_t nValue )
{
	if ( ((eType < eCT_Max && eType > eCT_None) == false) || (nValue <= 9 && nValue >= 1) == false )
	{
		LOGFMTE("makeCardNumber card type error , type  = %u, value = %u ",eType,nValue) ;
	}

	assert(eType < eCT_Max && eType > eCT_None && "invalid card type" );
	assert(nValue <= 9 && nValue >= 1 && "invalid card value" );
	uint8_t nType = eType ;
	nType = nType << 4 ;
	uint8_t nNum = nType | nValue ;
	return nNum ;
}

void CMJCard::parseCardTypeValue(uint8_t nCardNum, eMJCardType& eType,uint8_t& nValue )
{
	eType = parseCardType(nCardNum) ;
	nValue = parseCardValue(nCardNum) ;

	if ( ((eType < eCT_Max && eType > eCT_None) == false) || (nValue <= 9 && nValue >= 1) == false )
	{
		LOGFMTE("parseCardTypeValue card type error , type  = %u, value = %u number = %u",eType,nValue ,nCardNum) ;
	}

	assert(eType < eCT_Max && eType > eCT_None && "invalid card type" );
	assert(nValue <= 9 && nValue >= 1 && "invalid card value" );
}

void CMJCard::debugSinglCard(uint8_t nCard )
{
	auto cardType = parseCardType(nCard) ;
	auto cardValue = parseCardValue(nCard) ;

	switch (cardType)
	{
	case eCT_None:
		break;
	case eCT_Wan:
		printf("%u 万 \n",cardValue) ;
		break;
	case eCT_Tong:
		printf("%u 筒 \n",cardValue) ;
		break;
	case eCT_Tiao:
		printf("%u 条 \n",cardValue) ;
		break;
	case eCT_Feng:
		switch ( cardValue )
		{
		case 1 :
			printf("东 风\n") ;
			break;
		case 2:
			printf("南 风\n") ;
			break;
		case 3:
			printf("西 风\n") ;
			break;
		case 4:
			printf("北 风\n") ;
			break ;
		default:
			printf("unknown 风 card = %u \n",nCard ) ;
			break;
		}
		break;
	case eCT_Jian:
		switch ( cardValue )
		{
		case 1 :
			printf("中 \n") ;
			break;
		case 2:
			printf("发 \n") ;
			break;
		case 3:
			printf("白\n") ;
			break;
		default:
			printf("unknown 箭牌 card = %u \n",nCard ) ;
			break;
		}
		break;
	case eCT_Max:
		printf("unknown card = %u \n",nCard) ;
		break;
	default:
		break;
	}
}