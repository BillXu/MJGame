#include "TwoBirdGodFanxing.h"
#include "MJPeerCardNew.h"
#include "NewMJRoom.h"
#include "LogManager.h"
#include <algorithm>
#include <set>
void CTowBirdGodCheckFanxingInfo::resetInfo( CMJPeerCardNew* peerCard, uint8_t nHuCard )
{
	m_pPeerCard = peerCard ;
	m_nHuCard = nHuCard ;
}

bool CTowBirdGodCheckFanxingInfo::isTargetCardJiang()
{
	return m_pPeerCard->getHuPaiInfoPtr()->m_nJianPai == m_nHuCard ;
}

// fanxing checker ;
CTwoBirdFanxingChecker::CTwoBirdFanxingChecker()
{
	m_pFanxingChecker = new C88FanXing();
}

CTwoBirdFanxingChecker::~CTwoBirdFanxingChecker()
{
	delete m_pFanxingChecker ;
	m_pFanxingChecker = nullptr ;
}

bool CTwoBirdFanxingChecker::check( CNewMJRoom* pRoom ,CMJPeerCardNew* peerCard, uint8_t nHuCard, uint8_t& nFanXing, uint8_t& nFanShu )
{
	m_tCheckFanxingInfo.resetInfo(peerCard,nHuCard);
	// m_pFanxingChecker->checkFanxing(&m_tCheckFanxingInfo,nFanXing,nFanShu) ;
	nFanXing = 1 ;
	nFanShu = 1 ;
	CLogMgr::SharedLogMgr()->ErrorLog("temp return base fanxing and base fanshu");
	return true ;
}

// 88番牌型
C88FanXing::C88FanXing()
{
	// da si xi 
	IMJNewCheckFanXing* pF = new CDaSiXiFanxing();
	addSubFanxing(pF) ;

	// da san yuan 
	pF = new CDaSanYuanFanxing();
	addSubFanxing(pF) ;

	// jiu bao lian deng
	pF = new CJiuBaoLianDengFanxing();
	addSubFanxing(pF) ;

	pF = new CSiGangFanxing();
	addSubFanxing(pF) ;

	pF = new CLianQiDuiFanxing();
	addSubFanxing(pF) ;

	// 64 FanXing 
	pF = new C64Fanxing();
	addSubFanxing(pF) ;
}

bool C88FanXing::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	return false ;
}

// 大四喜 
bool CDaSiXiFanxing::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto ptHuInfo = pInfo->m_pPeerCard->getHuPaiInfoPtr();
	std::vector<uint8_t> vKeCard ;
	pInfo->m_pPeerCard->getPengedCard(vKeCard);
	vKeCard.insert(vKeCard.begin(),ptHuInfo->m_vAnKeZi.begin(),ptHuInfo->m_vAnKeZi.end()) ;
	if ( vKeCard.size() != 4 )
	{
		return false ;
	}

	for ( auto& ref : vKeCard )
	{
		if ( CMJCard::parseCardType(ref) != eCT_Feng )
		{
			return false ;
		}
	}

	CLogMgr::SharedLogMgr()->SystemLog("this is da si xi ") ;
	nFanXing = 1 ;
	nFanShu = 88 ;
	return true ;
}

// 大三元 
bool CDaSanYuanFanxing::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto ptHuInfo = pInfo->m_pPeerCard->getHuPaiInfoPtr();
	std::vector<uint8_t> vKeCard ;
	pInfo->m_pPeerCard->getPengedCard(vKeCard);
	vKeCard.insert(vKeCard.begin(),ptHuInfo->m_vAnKeZi.begin(),ptHuInfo->m_vAnKeZi.end()) ;
	if ( vKeCard.size() < 3 )
	{
		return false ;
	}

	std::sort(vKeCard.begin(),vKeCard.end());
	bool bFind = false ;
	for ( uint8_t nIdx = 0 ; (uint8_t)(nIdx + 2) < vKeCard.size() ; ++nIdx )
	{
		auto ref = vKeCard[nIdx] ;
		if ( CMJCard::parseCardType(ref) != eCT_Jian || CMJCard::parseCardValue(ref) != 1 )
		{
			continue ;
		}

		bFind = ( (ref + 1) != vKeCard[nIdx + 1] && ( ref + 2) != vKeCard[nIdx + 2] ) ;
		break ;
	}

	if ( bFind )
	{
		CLogMgr::SharedLogMgr()->SystemLog("this is da san yuan ") ;
		nFanXing = 1 ;
		nFanShu = 88 ;
	}

	return bFind ;
}

// 九莲宝灯 
bool CJiuBaoLianDengFanxing::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto pPeerInfo = pInfo->m_pPeerCard;
	std::vector<uint8_t> vCards ;
	pPeerInfo->getHoldCard(vCards);

	if ( vCards.size() != 14 )
	{
		return false ;
	}

	if ( CMJCard::parseCardType(vCards[0]) != CMJCard::parseCardType(vCards[13]) )  // not the same color
	{
		return false ;
	}

	// must have 3 one , and 3 night;
	if ( CMJCard::parseCardValue(vCards[2]) != 1 || CMJCard::parseCardValue(vCards[11]) != 9 )
	{
		return false ;
	}

	std::set<uint8_t> vUniqueSet ;
	vUniqueSet.insert(vCards.begin(),vCards.end());
	if ( vUniqueSet.size() != 9 )
	{
		return false ;
	}

	CLogMgr::SharedLogMgr()->SystemLog("this is jiu bao lian deng ") ;
	nFanXing = 1 ;
	nFanShu = 88 ;
	return true ;
}

// 四杠 
bool CSiGangFanxing::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto pPeerInfo = pInfo->m_pPeerCard;
	std::vector<uint8_t> vHoldCards ,vGangCards;
	pPeerInfo->getHoldCard(vHoldCards) ;
	pPeerInfo->getGangCard(vGangCards);

	for ( uint8_t nIdx = 0 ; nIdx + 3 < vHoldCards.size(); )
	{
		if ( vHoldCards[nIdx] == vHoldCards[nIdx + 3] )
		{
			vGangCards.push_back(vHoldCards[nIdx]) ;
			nIdx += 4 ;
		}
		else
		{
			++nIdx ;
		}
	}

	if ( vGangCards.size() < 4 )
	{
		return false ;
	}
	CLogMgr::SharedLogMgr()->SystemLog("this is Si Gang ") ;
	nFanXing = 1 ;
	nFanShu = 88 ;
	return true ;
}

// 连七对 
bool CLianQiDuiFanxing::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto ptHuInfo = pInfo->m_pPeerCard->getHuPaiInfoPtr();
	if ( !ptHuInfo->is7Pairs() )
	{
		return false ;
	}

	if ( ptHuInfo->m_v7Pairs[0] + 6 != ptHuInfo->m_v7Pairs[6] )
	{
		return false ;
	}

	CLogMgr::SharedLogMgr()->SystemLog("this is lian qi dui ") ;
	nFanXing = 1 ;
	nFanShu = 88 ;
	return true ;
}

// 64番牌型
C64Fanxing::C64Fanxing()
{
	// add sub fanxing ;
	// last add 48 fanxing ;
}