#include "TwoBirdGodFanxing.h"
#include "MJPeerCardNew.h"
#include "NewMJRoom.h"
#include "log4z.h"
#include <algorithm>
#include <set>
void CTowBirdGodCheckFanxingInfo::resetInfo( CMJPeerCardNew* peerCard, uint8_t nHuCard , bool isMiaoshou,bool isLoaoYue,bool isGangShangHua,bool isQaingGang, bool isZiMo,bool isJueZhang )
{
	m_pPeerCard = peerCard ;
	m_nHuCard = nHuCard ;
	this->m_isMiaoShou = isMiaoshou ;
	this->m_isLaoYue = isLoaoYue ;
	this->m_isGangshangHua = isGangShangHua ;
	this->m_isQiangGang = isQaingGang ;
	this->m_isZiMo = isZiMo ;
	this->m_isJueZhang = isJueZhang ;
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

bool CTwoBirdFanxingChecker::check( CTowBirdGodCheckFanxingInfo*pInfo, uint8_t& nFanXing, uint8_t& nFanShu )
{
	auto isRet = m_pFanxingChecker->checkFanxing(pInfo,nFanXing,nFanShu) ;
	if ( !isRet )
	{
		nFanShu = 1 ;
		nFanXing = eFanxing_PingHu;
		LOGFMTD(" common hu type ") ;
	}
	//LOGFMTE("temp return base fanxing and base fanshu");
	return true ;
}

// qing yi se fan xing 
bool CQingYiSeFanxing::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto pPeerInfo = pInfo->m_pPeerCard;
	std::vector<uint8_t> vCards , vShowedCard;
	pPeerInfo->getHoldCard(vCards);
	pPeerInfo->getShowedCard(vShowedCard);
	vCards.insert(vCards.begin(),vShowedCard.begin(),vShowedCard.end());
	std::sort(vCards.begin(),vCards.end());
	if ( CMJCard::parseCardType(vCards[0]) != CMJCard::parseCardType(vCards[vCards.size()-1]) )  // not the same color
	{
		return false ;
	}
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
	setNexFanxingGroup(pF);
	
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

	LOGFMTI("this is da si xi ") ;
	nFanXing = eFanxing_DaSiXi;
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
		LOGFMTI("this is da san yuan ") ;
		nFanXing = eFanxing_DaSanYuan;
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

	LOGFMTI("this is jiu bao lian deng ") ;
	nFanXing = eFanxing_JiuLianBaoDeng;
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
	LOGFMTI("this is Si Gang ") ;
	nFanXing = eFanxing_SiGang;
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

	LOGFMTI("this is lian qi dui ") ;
	nFanXing = eFanxing_LianQiDui;
	nFanShu = 88 ;
	return true ;
}

// 64番牌型
C64Fanxing::C64Fanxing()
{
	// add sub fanxing ;
	IMJNewCheckFanXing* pF = new CXiaoSiXiFanxing();
	addSubFanxing(pF) ;

	pF = new CXiaoSanYuanFanxing();
	addSubFanxing(pF) ;

	pF = new CZiYiSeFanxing();
	addSubFanxing(pF) ;

	pF = new CSiAnKeFanxing();
	addSubFanxing(pF) ;

	pF = new CYiSeShuangLongHuiFanxing( new CQingYiSeFanxing() );
	addSubFanxing(pF) ;

	// 48 FanXing 
	pF = new C48Fanxing();
	setNexFanxingGroup(pF);
}

//  小四喜
bool CXiaoSiXiFanxing::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto ptHuInfo = pInfo->m_pPeerCard->getHuPaiInfoPtr();

	if ( CMJCard::parseCardType(ptHuInfo->m_nJianPai) != eCT_Feng )
	{
		return false ;
	}

	std::vector<uint8_t> vKeCard ;
	pInfo->m_pPeerCard->getPengedCard(vKeCard);
	vKeCard.insert(vKeCard.begin(),ptHuInfo->m_vAnKeZi.begin(),ptHuInfo->m_vAnKeZi.end()) ;
	if ( vKeCard.size() < 3 )
	{
		return false ;
	}

	uint8_t nFengKeCnt = std::count_if(vKeCard.begin(),vKeCard.end(),[](uint8_t& ref ){ return CMJCard::parseCardType(ref) == eCT_Feng ;} ) ;
	if ( nFengKeCnt != 3 )
	{
		return false ;
	}

	LOGFMTI("this is xiao si xi ") ;
	nFanXing = eFanxing_XiaoSiXi;
	nFanShu = 64 ;
	return true ;
}

//  小三元
bool CXiaoSanYuanFanxing::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto ptHuInfo = pInfo->m_pPeerCard->getHuPaiInfoPtr();

	if ( CMJCard::parseCardType(ptHuInfo->m_nJianPai) != eCT_Jian )
	{
		return false ;
	}

	std::vector<uint8_t> vKeCard ;
	pInfo->m_pPeerCard->getPengedCard(vKeCard);
	vKeCard.insert(vKeCard.begin(),ptHuInfo->m_vAnKeZi.begin(),ptHuInfo->m_vAnKeZi.end()) ;
	if ( vKeCard.size() < 2 )
	{
		return false ;
	}

	uint8_t nJianKeCnt = std::count_if(vKeCard.begin(),vKeCard.end(),[](uint8_t& ref ){ return CMJCard::parseCardType(ref) == eCT_Jian ;} ) ; 
	if ( nJianKeCnt != 2 )
	{
		return false ;
	}

	LOGFMTI("this is xiao san yuan ") ;
	nFanXing = eFanxing_XiaoSanYuan;
	nFanShu = 64 ;
	return true ;
}

//  字一色
bool CZiYiSeFanxing::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto ptHuInfo = pInfo->m_pPeerCard->getHuPaiInfoPtr();

	if ( CMJCard::parseCardType(ptHuInfo->m_nJianPai) != eCT_Jian && CMJCard::parseCardType(ptHuInfo->m_nJianPai) != eCT_Feng )
	{
		return false ;
	}

	if ( ptHuInfo->m_vAllShunzi.empty() == false || ptHuInfo->is7Pairs() )
	{
		return false ;
	}

	std::vector<uint8_t> vAllCard , vHoldCard;
	pInfo->m_pPeerCard->getShowedCard(vAllCard);
	pInfo->m_pPeerCard->getHoldCard(vHoldCard);
	vAllCard.insert(vAllCard.begin(),vHoldCard.begin(),vHoldCard.end()) ;

	for ( auto& ref : vAllCard )
	{
		if ( CMJCard::parseCardType(ref) != eCT_Jian && CMJCard::parseCardType(ref) != eCT_Feng )
		{
			return false ;
		}
	}

	LOGFMTI("this is zi yi se ") ;
	nFanXing = eFanxing_ZiYiSe;
	nFanShu = 64 ;
	return true ;
}

//  四暗刻
bool CSiAnKeFanxing::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto ptHuInfo = pInfo->m_pPeerCard->getHuPaiInfoPtr();
	if ( ptHuInfo->m_vAnKeZi.size() != 4 )
	{
		return false ;
	}

	LOGFMTI("this is si an ke") ;
	nFanXing = eFanxing_SiAnKe;
	nFanShu = 64 ;
	return true ;
}

// 一色双龙会
bool CYiSeShuangLongHuiFanxing::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto ptHuInfo = pInfo->m_pPeerCard->getHuPaiInfoPtr();

	if ( CMJCard::parseCardValue(ptHuInfo->m_nJianPai) != 5 )
	{
		return false ;
	}

	std::vector<uint8_t> vPeng , vGang ;
	pInfo->m_pPeerCard->getPengedCard(vPeng);
	pInfo->m_pPeerCard->getGangCard(vGang);
	if ( vPeng.empty() == false || false == vGang.empty() )
	{
		return false ;
	}

	if ( !m_pqingyiSeChecker->checkFanxing(pCheckInfo,nFanXing,nFanShu ) )
	{
		return false ;
	}

	if ( ptHuInfo->m_vAnKeZi.empty() == false )
	{
		return false ;
	}

	// 2 lao shao fu 
	std::vector<uint8_t> vecCards ,vecHold ;
	pInfo->m_pPeerCard->getEatCard(vecCards);
	pInfo->m_pPeerCard->getHoldCard(vecHold) ;

	// remove jiang from hold 
	auto iter = std::find(vecHold.begin(),vecHold.end(),ptHuInfo->m_nJianPai) ;
	vecHold.erase(iter);
	iter = std::find(vecHold.begin(),vecHold.end(),ptHuInfo->m_nJianPai) ;
	vecHold.erase(iter);

	vecCards.insert(vecCards.begin(),vecHold.begin(),vecHold.end()) ;
	if ( vecCards.size() != 12 )
	{
		return false ;
	}

	std::sort(vecCards.begin(),vecCards.end());

	for ( uint8_t nPair = 0 ; nPair < 3 ; ++nPair )
	{
		// 1 2  3 group ;
		uint8_t nCardIdx = nPair * 2 ;
		if ( vecCards[nCardIdx] != vecCards[nCardIdx + 1] || vecCards[nCardIdx] != nPair + 1  )
		{
			return false ;
		}

		//  7 8 9 group 
		nCardIdx = nPair * 2 + 6;
		if ( vecCards[nCardIdx] != vecCards[nCardIdx + 1] || vecCards[nCardIdx] != nPair + 7  )
		{
			return false ;
		}
	}

	LOGFMTI("yi se shuang long hui") ;
	nFanXing = eFanxing_YISeShuangLongHui;
	nFanShu = 64 ;
	return true ;
}

// 48番牌型
C48Fanxing::C48Fanxing()
{
	// add sub fanxing ;
	IMJNewCheckFanXing* pF = new CYiSeSiTongShunFanxing(new CQingYiSeFanxing());
	addSubFanxing(pF) ;

	pF = new CYiSeSiJieGaoFanxing(new CQingYiSeFanxing());
	addSubFanxing(pF) ;

	// 32 FanXing 
	pF = new C32Fanxing();
	setNexFanxingGroup(pF);
}

// 一色四同顺 
bool CYiSeSiTongShunFanxing::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto ptHuInfo = pInfo->m_pPeerCard->getHuPaiInfoPtr();

	
	if ( !m_pqingyiSeChecker->checkFanxing(pCheckInfo,nFanXing,nFanShu ) )
	{
		return false ;
	}

	std::vector<uint8_t> vecCards ,vecHold ;
	pInfo->m_pPeerCard->getEatCard(vecCards);
	pInfo->m_pPeerCard->getHoldCard(vecHold) ;

	// remove jiang from hold 
	auto iter = std::find(vecHold.begin(),vecHold.end(),ptHuInfo->m_nJianPai) ;
	vecHold.erase(iter);
	iter = std::find(vecHold.begin(),vecHold.end(),ptHuInfo->m_nJianPai) ;
	vecHold.erase(iter);

	vecCards.insert(vecCards.begin(),vecHold.begin(),vecHold.end()) ;
	if ( vecCards.size() != 12 )
	{
		return false ;
	}
	std::sort(vecCards.begin(),vecCards.end());

	for ( uint8_t nIdx = 0 ; nIdx < 3 ; ++nIdx )
	{
		uint8_t n = nIdx * 4 ;
		if ( vecCards[n] != vecCards[n+3] )
		{
			return false ;
		}
	}

	if ( vecCards[0] + 2 != vecCards[8] )
	{
		return false ;
	}

	LOGFMTI("yi si tong shun") ;
	nFanXing = eFanxing_YiSeSiTongShun;
	nFanShu = 48 ;
	return true ;
}

// 一色四节高
bool CYiSeSiJieGaoFanxing::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
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

	std::sort(vKeCard.begin(),vKeCard.end());
	if ( CMJCard::parseCardValue(vKeCard[0]) + 3 != CMJCard::parseCardValue(vKeCard[3]) )
	{
		return false ;
	}

	if ( !m_pqingyiSeChecker->checkFanxing(pCheckInfo,nFanXing,nFanShu) )
	{
		return false ;
	}

	LOGFMTI("yi se si jie gao") ;
	nFanXing = eFanxing_YiSeSiJieGao;
	nFanShu = 48 ;
	return true ;
}

// 32番牌型（3种）
C32Fanxing::C32Fanxing()
{
	// add sub fanxing ;
	IMJNewCheckFanXing* pF = new CHunYaoJiuFanxing();
	addSubFanxing(pF) ;

	pF = new CSanGangFanxing();
	addSubFanxing(pF) ;

	pF = new CYiSeSiBuGaoFanxing( new CQingYiSeFanxing() );
	addSubFanxing(pF) ;

	// 24 FanXing 
	pF = new C24Fanxing();
	setNexFanxingGroup(pF);
}

// 一色四步高
bool CYiSeSiBuGaoFanxing::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	if ( m_pqingyiSeChecker->checkFanxing(pCheckInfo,nFanXing,nFanShu) == false )
	{
		return false ;
	}

	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto ptHuInfo = pInfo->m_pPeerCard->getHuPaiInfoPtr();
	std::vector<uint8_t> vShunFirstCard;
	pInfo->m_pPeerCard->getEatCard(vShunFirstCard);
	for (auto& ref : ptHuInfo->m_vAllShunzi )
	{
		vShunFirstCard.push_back(ref->vCards[0]);
	}

	if ( vShunFirstCard.size() != 4 )
	{
		return false ;
	}

	std::sort(vShunFirstCard.begin(),vShunFirstCard.end());
	for ( uint8_t nIdx = 0 ; (nIdx + 1 ) < vShunFirstCard.size() ; ++nIdx )
	{
		if ( vShunFirstCard[nIdx] == vShunFirstCard[nIdx+1] )
		{
			return false ;
		}
	}

	uint8_t nStep = vShunFirstCard[3] - vShunFirstCard[0] ;
	if ( nStep != 3 && nStep != 6 )
	{
		return false ;
	}
	LOGFMTI(" yi se si bu gao ") ;
	nFanXing = eFanxing_YiSeSiBuGao;
	nFanShu = 32 ;
	return true ;
}

// 三杠
bool CSanGangFanxing::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	std::vector<uint8_t> vGang ;
	pInfo->m_pPeerCard->getGangCard(vGang);
	if ( vGang.size() != 3 )
	{
		return false ;
	}

	LOGFMTI("san gang ") ;
	nFanXing = eFanxing_SanGang;
	nFanShu = 32 ;
	return true ;
}

//混幺九
bool CHunYaoJiuFanxing::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto ptHuInfo = pInfo->m_pPeerCard->getHuPaiInfoPtr();
	auto nJiangType = CMJCard::parseCardType(ptHuInfo->m_nJianPai) ;
	auto nJiangValue = CMJCard::parseCardValue(ptHuInfo->m_nJianPai);
	if ( nJiangType != eCT_Jian && nJiangType != eCT_Feng && nJiangValue != 1 && nJiangValue != 9 )
	{
		return false ;
	}

	std::vector<uint8_t> vKeCard ;
	pInfo->m_pPeerCard->getPengedCard(vKeCard);
	vKeCard.insert(vKeCard.begin(),ptHuInfo->m_vAnKeZi.begin(),ptHuInfo->m_vAnKeZi.end()) ;
	if ( vKeCard.size() != 4 )
	{
		return false ;
	}

	for ( auto& ref : vKeCard )
	{
		auto nType = CMJCard::parseCardType(ref) ;
		auto nValue = CMJCard::parseCardValue(ref);
		if ( nType != eCT_Jian && nType != eCT_Feng && nValue != 1 && nValue != 9 )
		{
			return false ;
		}
	}

	LOGFMTI(" hun yao jiu ") ;
	nFanXing = eFanxing_HunYaoJiu;
	nFanShu = 32 ;
	return true ;
}

// 24番牌型（4种）
C24Fanxing::C24Fanxing()
{
	// add sub fanxing ;
	IMJNewCheckFanXing* pF = new CQiDui();
	addSubFanxing(pF) ;

	pF = new CQingYiSe(new CQingYiSeFanxing());
	addSubFanxing(pF) ;

	pF = new CYiSeTongSanShun(new CQingYiSeFanxing());
	addSubFanxing(pF) ;

	pF = new CYiSeSanJieGao( new CQingYiSeFanxing() );
	addSubFanxing(pF) ;

	// 16 FanXing 
	pF = new C16Fanxing();
	setNexFanxingGroup(pF);
}

// 七对
bool CQiDui::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto ptHuInfo = pInfo->m_pPeerCard->getHuPaiInfoPtr();
	
	if ( !ptHuInfo->is7Pairs() )
	{
		return false ;
	}

	LOGFMTI(" qi dui  ") ;
	nFanXing = eFanxing_QiDui;
	nFanShu = 24 ;
	return true ;
}

// 清一色 
bool CQingYiSe::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	if ( !m_pqingyiSeChecker->checkFanxing(pCheckInfo,nFanXing,nFanShu) )
	{
		return false ;
	}

	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto pPeerInfo = pInfo->m_pPeerCard;
	std::vector<uint8_t> vCards , vShowedCard;
	pPeerInfo->getHoldCard(vCards);
	pPeerInfo->getShowedCard(vShowedCard);
	vCards.insert(vCards.begin(),vShowedCard.begin(),vShowedCard.end());
 
	for ( auto& ref : vCards )
	{
		auto type = CMJCard::parseCardType(ref) ;
		if ( type == eCT_Feng || type == eCT_Jian )
		{
			return false ;
		}
	}

	LOGFMTI(" qing yi se   ") ;
	nFanXing = eFanxing_QingYiSe;
	nFanShu = 24 ;
	return true ;
}

//一色三同顺
bool CYiSeTongSanShun::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	if ( m_pqingyiSeChecker->checkFanxing(pCheckInfo,nFanXing,nFanShu) == false )
	{
		return false ;
	}

	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto ptHuInfo = pInfo->m_pPeerCard->getHuPaiInfoPtr();
	std::vector<uint8_t> vShunFirstCard;
	pInfo->m_pPeerCard->getEatCard(vShunFirstCard);
	for (auto& ref : ptHuInfo->m_vAllShunzi )
	{
		vShunFirstCard.push_back(ref->vCards[0]);
	}

	if ( vShunFirstCard.size() >= 3 )
	{
		std::sort(vShunFirstCard.begin(),vShunFirstCard.end());
		if ( vShunFirstCard[0] == vShunFirstCard[2] || vShunFirstCard[vShunFirstCard.size()-1] == vShunFirstCard[vShunFirstCard.size()-3] )
		{
			nFanXing = eFanxing_YiSeSanTongShun;
			nFanShu = 24 ;
			LOGFMTD("yi se san tong shun") ;
			return true  ;
		}
	}

	// 3 an ke shun xu 
	if ( ptHuInfo->m_vAnKeZi.size() >= 3 )
	{
		if ( ( ptHuInfo->m_vAnKeZi[0] + 2) == ptHuInfo->m_vAnKeZi[2] || ptHuInfo->m_vAnKeZi[ptHuInfo->m_vAnKeZi.size()-1] == ( ptHuInfo->m_vAnKeZi[ptHuInfo->m_vAnKeZi.size()-3] + 2 ) )
		{
			nFanXing = eFanxing_YiSeSanTongShun;
			nFanShu = 24 ;
			LOGFMTD("yi se san tong shun") ;
			return true  ;
		}
	}

	return false ;
}

//一色三节高
bool CYiSeSanJieGao::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	if ( m_pqingyiSeChecker->checkFanxing(pCheckInfo,nFanXing,nFanShu) == false )
	{
		return false ;
	}

	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto ptHuInfo = pInfo->m_pPeerCard->getHuPaiInfoPtr();
	std::vector<uint8_t> vKe,vPeng;
	vKe.assign(ptHuInfo->m_vAnKeZi.begin(),ptHuInfo->m_vAnKeZi.end());
	pInfo->m_pPeerCard->getPengedCard(vPeng) ;
	vKe.insert(vKe.begin(),vPeng.begin(),vPeng.end()) ;
	std::sort(vKe.begin(),vKe.end() );
	// 3 an ke shun xu 
	if ( vKe.size() >= 3 )
	{
		if ( ( vKe[0] + 2) == vKe[2] || vKe[vKe.size()-1] == ( vKe[vKe.size()-3] + 2 ) )
		{
			nFanXing = 1 ;
			nFanShu = 24 ;
			LOGFMTD("yi se san jie gao") ;
			return true  ;
		}
	}

	return false ;
}

// 16番牌型（3种）
C16Fanxing::C16Fanxing()
{
	// add sub fanxing ;
	IMJNewCheckFanXing* pF = new CQingLong(new CQingYiSeFanxing());
	addSubFanxing(pF) ;

	pF = new CYiSeSanBuGao(new CQingYiSeFanxing());
	addSubFanxing(pF) ;

	pF = new CSanAnKe();
	addSubFanxing(pF) ;

	// 12 FanXing 
	pF = new C12Fanxing();
	setNexFanxingGroup(pF);
}

// 清龙
bool CQingLong::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	if ( !m_pqingyiSeChecker->checkFanxing(pCheckInfo,nFanXing,nFanShu) )
	{
		return false ;
	}

	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto ptHuInfo = pInfo->m_pPeerCard->getHuPaiInfoPtr();
	std::vector<uint8_t> vShunFirstCard;
	pInfo->m_pPeerCard->getEatCard(vShunFirstCard);
	for (auto& ref : ptHuInfo->m_vAllShunzi )
	{
		vShunFirstCard.push_back(ref->vCards[0]);
	}

	if ( vShunFirstCard.size() < 3 )
	{
		return false ;
	}

	for ( uint8_t nIdx = 1 ; nIdx <= 7 ; nIdx += 3 )
	{
		auto iter = std::find(vShunFirstCard.begin(),vShunFirstCard.end(),CMJCard::makeCardNumber(CMJCard::parseCardType(vShunFirstCard[0]),nIdx )) ;
		if ( iter == vShunFirstCard.end() )
		{
			return false ;
		}
	}

	LOGFMTI(" qing long ") ;
	nFanXing = eFanxing_QingLong;
	nFanShu = 16 ;
	return true ;
}

//一色三步高
bool CYiSeSanBuGao::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	if ( m_pqingyiSeChecker->checkFanxing(pCheckInfo,nFanXing,nFanShu) == false )
	{
		return false ;
	}

	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto ptHuInfo = pInfo->m_pPeerCard->getHuPaiInfoPtr();
	std::vector<uint8_t> vShunFirstCard;
	pInfo->m_pPeerCard->getEatCard(vShunFirstCard);
	for (auto& ref : ptHuInfo->m_vAllShunzi )
	{
		vShunFirstCard.push_back(ref->vCards[0]);
	}

	if ( vShunFirstCard.size() < 3 )
	{
		return false ;
	}

	std::sort(vShunFirstCard.begin(),vShunFirstCard.end());

	static auto lpFunc = [](std::vector<uint8_t>& vShunF )->bool{
		for ( uint8_t nIdx = 0 ; (nIdx + 1 ) < vShunF.size() ; ++nIdx )
		{
			if ( vShunF[nIdx] == vShunF[nIdx+1] )
			{
				return false ;
			}
		}

		uint8_t nStep = vShunF[2] - vShunF[0] ;
		if ( nStep != 3 && nStep != 6 )
		{
			return false ;
		} 

		return true ;
	} ;

	if ( vShunFirstCard.size() == 3 )
	{
		if ( lpFunc(vShunFirstCard) )
		{
			LOGFMTI(" yi se san bu gao ") ;
			nFanXing = eFanxing_YiSeSanBuGao;
			nFanShu = 16 ;
			return true ;
		}
	}
	else if ( vShunFirstCard.size() == 4 )
	{
		for ( uint8_t nIdx = 0 ; nIdx < vShunFirstCard.size() ; ++nIdx )
		{
			std::vector<uint8_t> vCp (vShunFirstCard);
			auto ptr = std::find(vCp.begin(),vCp.end(),vCp[nIdx]);
			vCp.erase(ptr) ;
			if ( lpFunc(vCp) )
			{
				LOGFMTI(" yi se san bu gao ") ;
				nFanXing = eFanxing_YiSeSanBuGao;
				nFanShu = 16 ;
				return true ;
			}
		}
	}
	else
	{
		LOGFMTE("never come to here") ;
	}
	return false ;
}

//三暗刻
bool CSanAnKe::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto ptHuInfo = pInfo->m_pPeerCard->getHuPaiInfoPtr();
	if ( ptHuInfo->m_vAnKeZi.size() == 3 )
	{
		LOGFMTD("san an ke") ;
		nFanXing = eFanxing_SanAnKe;
		nFanShu = 16 ;
		return true ;
	}
	return false ;
}

// 12番牌型（3种）
C12Fanxing::C12Fanxing()
{
	// add sub fanxing ;
	IMJNewCheckFanXing* pF = new CDaYu5();
	addSubFanxing(pF) ;

	pF = new CXiaoYu5();
	addSubFanxing(pF) ;

	pF = new CSanFengKe();
	addSubFanxing(pF) ;

	// 8 FanXing 
	pF = new C8Fanxing();
	setNexFanxingGroup(pF);
}

//大于5
bool CDaYu5::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto pPeerInfo = pInfo->m_pPeerCard;
	std::vector<uint8_t> vCards , vShowedCard, vGang;
	pPeerInfo->getGangCard(vGang);
	if ( vGang.empty() == false )
	{
		return false ;
	}
	pPeerInfo->getHoldCard(vCards);
	pPeerInfo->getShowedCard(vShowedCard);
	vCards.insert(vCards.begin(),vShowedCard.begin(),vShowedCard.end());
	for ( auto& ref : vCards )
	{
		auto ptype = CMJCard::parseCardType(ref);
		auto value = CMJCard::parseCardValue(ref);
		if ( ptype == eCT_Feng || ptype == eCT_Jian || value <= 5 )
		{
			return false ;
		}
	}

	nFanXing = eFanxing_DaYu5;
	nFanShu = 12 ;
	LOGFMTD("da yu 5") ;
	return true ;
}

//小于5
bool CXiaoYu5::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto pPeerInfo = pInfo->m_pPeerCard;
	std::vector<uint8_t> vCards , vShowedCard, vGang;
	pPeerInfo->getGangCard(vGang);
	if ( vGang.empty() == false )
	{
		return false ;
	}
	pPeerInfo->getHoldCard(vCards);
	pPeerInfo->getShowedCard(vShowedCard);
	vCards.insert(vCards.begin(),vShowedCard.begin(),vShowedCard.end());
	for ( auto& ref : vCards )
	{
		auto ptype = CMJCard::parseCardType(ref);
		auto value = CMJCard::parseCardValue(ref);
		if ( ptype == eCT_Feng || ptype == eCT_Jian || value >= 5 )
		{
			return false ;
		}
	}

	nFanXing = eFanxing_XiaoYu5;
	nFanShu = 12 ;
	LOGFMTD("da yu 5") ;
	return true ;
}

//三风刻
bool CSanFengKe::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto ptHuInfo = pInfo->m_pPeerCard->getHuPaiInfoPtr();
	std::vector<uint8_t> vKe,vPeng;
	vKe.assign(ptHuInfo->m_vAnKeZi.begin(),ptHuInfo->m_vAnKeZi.end());
	pInfo->m_pPeerCard->getPengedCard(vPeng) ;
	vKe.insert(vKe.begin(),vPeng.begin(),vPeng.end()) ;
	
	uint8_t nCnt = std::count_if(vKe.begin(),vKe.end(),[](uint8_t& ref ){ return CMJCard::parseCardType(ref) == eCT_Feng ;} ) ;  
	if ( nCnt < 3 )
	{
		return false ;
	}

	nFanXing = eFanxing_SanFengKe;
	nFanShu = 12 ;
	LOGFMTD("san feng ke") ;
	return true ;
}

// 8番牌型（4种）
C8Fanxing::C8Fanxing()
{
	// add sub fanxing ;
	IMJNewCheckFanXing* pF = new CMiaoShouHuiChun();
	addSubFanxing(pF) ;

	pF = new CHaiDiLaoYue();
	addSubFanxing(pF) ;

	pF = new CGangShangHua();
	addSubFanxing(pF) ;

	pF = new QiangGangHu();
	addSubFanxing(pF) ;

	// 6 FanXing 
	pF = new C6Fanxing();
	setNexFanxingGroup(pF);
}

// 妙手回春
bool CMiaoShouHuiChun::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto ret = pInfo->isMiaoShouHuiChun();
	if ( !ret )
	{
		return  false ;
	}
	nFanXing = eFanxing_MiaoShouHuiChun;
	nFanShu = 8  ;
	LOGFMTD("miao shou hui chun");
	return true ;
}

// 海底捞月
bool CHaiDiLaoYue::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto ret = pInfo->isHaiDiLaoYue();
	if ( !ret )
	{
		return  false ;
	}
	nFanXing = eFanxing_HaiDiLaoYue;
	nFanShu = 8  ;
	LOGFMTD("hai di lao yue");
	return true ;
}

//杠上花
bool CGangShangHua::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto ret = pInfo->isGangShangHua();
	if ( !ret )
	{
		return  false ;
	}
	nFanXing = eFanxing_GangShangHua;
	nFanShu = 8  ;
	LOGFMTD("gang shang hua");
	return true ;
}

//抢杠胡
bool QiangGangHu::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto ret = pInfo->isQiangGangHu();
	if ( !ret )
	{
		return  false ;
	}
	nFanXing = eFanxing_QiangGangHu;
	nFanShu = 8  ;
	LOGFMTD("Qiang Gang hu");
	return true ;
}

//6番牌型（5种）
C6Fanxing::C6Fanxing()
{
	// add sub fanxing ;
	IMJNewCheckFanXing* pF = new CPengPengHu();
	addSubFanxing(pF) ;

	pF = new CHunYiSe();
	addSubFanxing(pF) ;

	pF = new CQuanQiuRen();
	addSubFanxing(pF) ;

	pF = new CShuangAnGang();
	addSubFanxing(pF) ;

	pF = new CShuangJianKe();
	addSubFanxing(pF) ;

	// 4 FanXing 
	pF = new C4Fanxing();
	setNexFanxingGroup(pF);
}

// 碰碰胡
bool CPengPengHu::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto ptHuInfo = pInfo->m_pPeerCard->getHuPaiInfoPtr();
	if ( ptHuInfo->m_vAllShunzi.empty() == false )
	{
		return false ;
	}

	std::vector<uint8_t> vec ;
	pInfo->m_pPeerCard->getEatCard(vec);
	if ( vec.empty() == false )
	{
		return false ;
	}

	nFanShu = 6 ;
	nFanXing = eFanxing_DuiDuiHu;
	LOGFMTD("peng peng hu") ;
	return true ;
}

// 混一色
bool CHunYiSe::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto pPeerInfo = pInfo->m_pPeerCard;
	std::vector<uint8_t> vCards , vShowedCard;
	pPeerInfo->getHoldCard(vCards);
	pPeerInfo->getShowedCard(vShowedCard);
	vCards.insert(vCards.begin(),vShowedCard.begin(),vShowedCard.end());
	bool isHaveZi = false ;
	bool isHaveXu = false ;
	for ( auto& ref : vCards )
	{
		auto pType = CMJCard::parseCardType(ref);
		auto isZi = (pType == eCT_Feng || eCT_Jian == pType );
		if ( isZi )
		{
			isHaveZi = true ;
		}
		else
		{
			isHaveXu = true ;
		}

		if ( isHaveXu && isHaveZi )
		{
			nFanShu = 6 ;
			nFanXing = eFanxing_HunYiSe;
			LOGFMTD("Hun yi se") ;
			return true ;
		}
	}

	return false ;
}

// 全求人
bool CQuanQiuRen::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto pPeerInfo = pInfo->m_pPeerCard;
	std::vector<uint8_t> vCards ;
	pPeerInfo->getHoldCard(vCards);
	if ( vCards.size() <= 2 )
	{
		nFanShu = 6 ;
		nFanXing = eFanxing_QuanQiuRen;
		LOGFMTD("quan qiu ren") ;
		return true ;
	}
	return false ;
}

// 双暗杠
bool CShuangAnGang::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto pPeerInfo = pInfo->m_pPeerCard;
	std::vector<uint8_t> vCards ;
	pPeerInfo->getAnGang(vCards);
	if ( vCards.size() == 2 )
	{
		nFanShu = 6 ;
		nFanXing = eFanxing_ShuangAnGang;
		LOGFMTD("shuang an gang") ;
		return true ;
	}
	return false ;
}

// 双箭刻
bool CShuangJianKe::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto pPeerInfo = pInfo->m_pPeerCard;
	std::vector<uint8_t> vCards , vPeng,vGang ;
	pPeerInfo->getPengedCard(vPeng);
	pPeerInfo->getGangCard(vGang);
	auto pHuInfo = pPeerInfo->getHuPaiInfoPtr();
	vCards.assign(pHuInfo->m_vAnKeZi.begin(),pHuInfo->m_vAnKeZi.end()) ;
	vCards.insert(vCards.begin(),vPeng.begin(),vPeng.end());
	vCards.insert(vCards.begin(),vGang.begin(),vGang.end());
	
	auto nCnt = std::count_if(vCards.begin(),vCards.end(),[](uint8_t& ref ){ return CMJCard::parseCardType(ref) == eCT_Jian ;} ) ;   
	if ( 2 == nCnt )
	{
		nFanShu = 6 ;
		nFanXing = eFanxing_ShuangJianKe;
		LOGFMTD("shuang jian ke") ;
		return true ;
	}

	return false ;
}

//4番牌型（4种）
C4Fanxing::C4Fanxing()
{
	// add sub fanxing ;
	IMJNewCheckFanXing* pF = new CQuanDaiYao();
	addSubFanxing(pF) ;

	pF = new CBuQiuRen();
	addSubFanxing(pF) ;

	pF = new CShuangMingGang();
	addSubFanxing(pF) ;

	pF = new CHuJueZhang();
	addSubFanxing(pF) ;

	// 2 FanXing 
	pF = new C2Fanxing();
	setNexFanxingGroup(pF);
}

// 带幺九
bool CQuanDaiYao::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto pPeerInfo = pInfo->m_pPeerCard;
	auto pHuInfo = pPeerInfo->getHuPaiInfoPtr();

	// check jiang
	auto Type = CMJCard::parseCardType(pHuInfo->m_nJianPai);
	auto nCardValue = CMJCard::parseCardValue(pHuInfo->m_nJianPai) ;
	if ( 1 != nCardValue && 9 != nCardValue && Type != eCT_Feng && eCT_Jian != Type )
	{
		return false ;
	}

	// check shun 
	for ( auto& ref : pHuInfo->m_vAllShunzi )
	{
		auto fValue = CMJCard::parseCardValue(ref->vCards[0]);
		auto lValue = CMJCard::parseCardType(ref->vCards[2] ) ;
		bool isAyo = (1 == fValue ) || ( 9 == fValue );
		bool lIsAyo = ( 1 == lValue ) || ( 9 == lValue );
		if ( isAyo == false && false == lIsAyo )
		{
			return false ;
		}
	}

	std::vector<uint8_t> vCards ;
	pPeerInfo->getEatCard(vCards) ;
	for ( uint8_t nIdx = 0 ; (nIdx + 2 ) < vCards.size(); nIdx += 3 )
	{
		auto fValue = CMJCard::parseCardValue(vCards[nIdx]);
		auto lValue = CMJCard::parseCardType(vCards[nIdx + 2] ) ;
		bool isAyo = (1 == fValue ) || ( 9 == fValue );
		bool lIsAyo = ( 1 == lValue ) || ( 9 == lValue );
		if ( isAyo == false && false == lIsAyo )
		{
			return false ;
		}
	}

	// check ke and gang 
	vCards.clear();
	pPeerInfo->getPengedCard(vCards);
	vCards.insert(vCards.begin(),pHuInfo->m_vAnKeZi.begin(),pHuInfo->m_vAnKeZi.end()) ;

	// check gang 
	std::vector<uint8_t> vGang ;
	pPeerInfo->getGangCard(vGang);
	vCards.insert(vCards.begin(),vGang.begin(),vGang.end()) ;

	for ( uint8_t nIdx = 0 ; (nIdx) < vCards.size(); ++nIdx )
	{
		auto Type = CMJCard::parseCardType(vCards[nIdx]);
		if ( eCT_Jian == Type || eCT_Feng == Type )
		{
			continue;
		}

		auto fValue = CMJCard::parseCardValue(vCards[nIdx]);;
		bool isAyo = (1 == fValue ) || ( 9 == fValue );
		if ( isAyo == false )
		{
			return false ;
		}
	}

	nFanShu = 4 ;
	nFanXing = eFanxing_DaiYaoJiu;
	LOGFMTD("dai yao jiu") ;
	return true ;
}

// 不求人
bool CBuQiuRen::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto pPeerInfo = pInfo->m_pPeerCard;
	if ( pInfo->isZiMo() == false )
	{
		return false ;
	}

	std::vector<uint8_t> vCards ;
	pPeerInfo->getEatCard(vCards) ;
	if ( vCards.empty() == false  )
	{
		return false ;
	}

	vCards.clear();
	pPeerInfo->getPengedCard(vCards) ;
	if ( vCards.empty() == false )
	{
		return false ;
	}

	vCards.clear();
	pPeerInfo->getMingGang(vCards) ;
	if ( vCards.empty() == false )
	{
		return false ;
	}

	nFanShu = 4 ;
	nFanXing = eFanxing_BuQiuRen;
	LOGFMTD(" bu qiu ren ") ;
	return true ;
}

// 双明杠
bool CShuangMingGang::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto pPeerInfo = pInfo->m_pPeerCard;
	
	std::vector<uint8_t> vShow ;
	pPeerInfo->getMingGang(vShow) ;
	if ( vShow.size() != 2 )
	{
		return false ;
	}

	nFanShu = 4 ;
	nFanXing = eFanxing_ShuangMingGang;
	LOGFMTD(" shuang ming gang ") ;
	return true ;
}

// 胡绝张
bool CHuJueZhang::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	if ( pInfo->isHuJueZhang() == false )
	{
		return false ;
	}

	nFanShu = 4 ;
	nFanXing = eFanxing_HuJueZhang;
	LOGFMTD(" hu jue zhang ") ;
	return true ;
}

//2番牌型（2种）
C2Fanxing::C2Fanxing()
{
	// add sub fanxing ;
	IMJNewCheckFanXing* pF = new CJianKe();
	addSubFanxing(pF) ;

	pF = new CMengQianQing();
	addSubFanxing(pF) ;

	pF = new CPingHu();
	addSubFanxing(pF) ;

	pF = new CSiGuiYi();
	addSubFanxing(pF) ;

	pF = new CShuangAnKe();
	addSubFanxing(pF) ;

	pF = new CAnGang();
	addSubFanxing(pF) ;

	pF = new CDuanYao();
	addSubFanxing(pF) ;

	setNexFanxingGroup(nullptr);
}

// 箭刻
bool CJianKe::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto pPeerInfo = pInfo->m_pPeerCard;
	auto pHuPaiInfo = pPeerInfo->getHuPaiInfoPtr();
	std::vector<uint8_t> vKe ;
	pPeerInfo->getPengedCard(vKe);
	vKe.insert(vKe.begin(),pHuPaiInfo->m_vAnKeZi.begin(),pHuPaiInfo->m_vAnKeZi.end() );
	for ( auto& ref : vKe )
	{
		auto type = CMJCard::parseCardType(ref) ;
		if ( eCT_Jian == type )
		{
			nFanShu = 2 ;
			nFanXing = eFanxing_JianKe;
			LOGFMTD("this is jian ke ") ;
			return true ;
		}
	}
	return false ;
}

// 门前清
bool CMengQianQing::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto pPeerInfo = pInfo->m_pPeerCard;
	std::vector<uint8_t> vCards ;
	pPeerInfo->getEatCard(vCards) ;
	if ( vCards.empty() == false  )
	{
		return false ;
	}

	vCards.clear();
	pPeerInfo->getPengedCard(vCards) ;
	if ( vCards.empty() == false )
	{
		return false ;
	}

	vCards.clear();
	pPeerInfo->getMingGang(vCards) ;
	if ( vCards.empty() == false )
	{
		return false ;
	}

	nFanShu = 2 ;
	nFanXing = eFanxing_MengQianQing;
	LOGFMTD("this is meng qian qing ") ;
	return true ;
}

//平胡
bool CPingHu::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto pPeerInfo = pInfo->m_pPeerCard;
	auto pHuInfo = pPeerInfo->getHuPaiInfoPtr();
	std::vector<uint8_t> vCards ;
	pPeerInfo->getEatCard(vCards) ;
	if ( 4 != (vCards.size() + pHuInfo->m_vAllShunzi.size()) )
	{
		return false ;
	}

	auto type = CMJCard::parseCardType(pHuInfo->m_nJianPai) ;
	if ( eCT_Jian == type || eCT_Feng == type )
	{
		return false ;
	}

	nFanShu = 2 ;
	nFanXing = eFanxing_PingHu;
	LOGFMTD(" ping hu ") ;
	return true ;
}

// 四归一
bool CSiGuiYi::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto pPeerInfo = pInfo->m_pPeerCard;
	std::vector<uint8_t> vCards , vEatCards, vPeng;
	pPeerInfo->getEatCard(vEatCards) ;
	pPeerInfo->getHoldCard(vCards) ;
	vCards.insert(vCards.begin(),vEatCards.begin(),vEatCards.end());

	pPeerInfo->getPengedCard(vPeng) ;
	// check peng , in cards 
	for ( auto& ref : vPeng )
	{
		auto iter = std::find(vCards.begin(),vCards.end(),ref);
		if ( iter != vCards.end() )
		{
			nFanShu = 2 ;
			nFanXing = eFanxing_SiGuiYi;
			LOGFMTD(" si gui yi ") ;
			return true ;
		}
	}

	std::sort(vCards.begin(),vCards.end()) ;
	for ( uint8_t nidx = 0 ; ( nidx + 3 )< vCards.size(); ++nidx )
	{
		if ( vCards[nidx] == vCards[nidx+3] )
		{
			nFanShu = 2 ;
			nFanXing = 1 ;
			LOGFMTD(" si gui yi ") ;
			return true ;
		}
	}
	return false ;
}

// 双暗刻
bool CShuangAnKe::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto pPeerInfo = pInfo->m_pPeerCard;
	auto pHuInfo = pPeerInfo->getHuPaiInfoPtr();
	if ( pHuInfo->m_vAnKeZi.size() != 2 )
	{
		return false ;
	}

	nFanShu = 2 ;
	nFanXing = eFanxing_ShuangAnKe;
	LOGFMTD(" shuang an ke ") ;
	return true ;
}

// 暗杠
bool CAnGang::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto pPeerInfo = pInfo->m_pPeerCard;
	std::vector<uint8_t> vCards ;
	pPeerInfo->getAnGang(vCards);
	if ( vCards.empty() )
	{
		return false ; 
	}

	nFanShu = 2 ;
	nFanXing = eFanxing_AnGang;
	LOGFMTD("An Gang") ;
	return true ;
}

// 断幺
bool CDuanYao::checkSelfFanxing(ICheckFanxingInfo* pCheckInfo,uint8_t& nFanXing, uint8_t& nFanShu )
{
	CTowBirdGodCheckFanxingInfo* pInfo = (CTowBirdGodCheckFanxingInfo*)pCheckInfo ;
	auto pPeerInfo = pInfo->m_pPeerCard;
	std::vector<uint8_t> vCards, showCards ;
	pPeerInfo->getShowedCard(showCards) ;
	pPeerInfo->getHoldCard(vCards) ;
	vCards.insert(vCards.begin(),showCards.begin(),showCards.end());
	for ( auto& ref : vCards )
	{
		auto type = CMJCard::parseCardType(ref) ;
		auto value = CMJCard::parseCardValue(ref) ;
		if ( eCT_Feng == type || eCT_Jian == type || 1 == value || 9 == value  )
		{
			return false ;
		}
	}

	nFanShu = 2 ;
	nFanXing = 1 ;
	LOGFMTD(" duan yao ") ;
	return true ;
}

