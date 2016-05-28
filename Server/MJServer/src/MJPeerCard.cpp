#include "MJPeerCard.h"
#include <cassert>
#include "MJCard.h"
#include "MJBloodFanxing.h"
#include "LogManager.h"
bool CPeerCardSubCollect::removeCardNumber( uint8_t nNumber )
{
	auto iter = m_vAnCards.begin() ;
	for ( ; iter != m_vAnCards.end(); ++iter )
	{
		if ( (*iter).nCardNumber == nNumber )
		{
			printf("remove a card numer = %u ",nNumber) ;
			m_vAnCards.erase(iter) ;
			return true ;
		}
	}

	assert(0&&"can not find remove number");
	return false ;
}

uint8_t CPeerCardSubCollect::getCardCount()
{
	return m_vAnCards.size() + m_vMingCards.size() ;
}

uint8_t CPeerCardSubCollect::getGenCount()
{
	LIST_PEER_CARDS vAllCard ;
	vAllCard.assign(m_vAnCards.begin(),m_vAnCards.end()) ;
	vAllCard.insert(vAllCard.begin(),m_vMingCards.begin(),m_vMingCards.end()) ;
	
	std::map<uint8_t,uint8_t> vCardCnt ;
	for ( auto ref : vAllCard )
	{
		auto iter = vCardCnt.find(ref.nCardNumber) ;
		if ( iter == vCardCnt.end() )
		{
			vCardCnt[ref.nCardNumber] = 1 ;
		}
		else
		{
			vCardCnt[ref.nCardNumber] = 1 + vCardCnt[ref.nCardNumber] ;
		}
	}

	uint8_t nCnt = 0 ;
	for ( auto refIter : vCardCnt )
	{
		if ( refIter.second == 4 )
		{
			++nCnt ;
		}
	}
	return nCnt ;
}

void CPeerCardSubCollect::doAction(eMJActType eType, uint8_t nNumber )
{
	switch ( eType )
	{
	case eMJAct_Mo:
		{
			stSinglePeerCard peer ;
			peer.nCardNumber = nNumber ;

			auto iter = m_vAnCards.begin() ;
			for ( ; iter != m_vAnCards.end(); ++iter )
			{
				if ( (*iter).nCardNumber >= nNumber )
				{
					m_vAnCards.insert(iter,peer) ;
					printf("add a card numer = %u ",nNumber) ;
					return;
				}
			}
			m_vAnCards.push_back(peer) ;
		}
		break;
	case eMJAct_Peng:
		{
			stSinglePeerCard peer ;
			peer.nCardNumber = nNumber ;
			peer.eState = eSinglePeerCard_Peng;

			uint8_t nEraseCount= 2 ;
			auto iter = m_vAnCards.begin() ;
			for ( ; iter != m_vAnCards.end() && nEraseCount > 0;  )
			{
				if ( (*iter).nCardNumber == nNumber )
				{
					m_vAnCards.erase(iter) ;
					--nEraseCount ;
					iter = m_vAnCards.begin() ;
				}
				else
				{
					++iter;
				}
			}
			assert(nEraseCount == 0 && "why can not peng" );

			// add to ming pai
			auto iterMing = m_vMingCards.begin() ;
			for ( ; iterMing != m_vMingCards.end(); ++iterMing )
			{
				if ( (*iterMing).nCardNumber >= nNumber )
				{
					iterMing = m_vMingCards.insert(iterMing,peer) ;
					iterMing = m_vMingCards.insert(iterMing,peer) ;
					iterMing = m_vMingCards.insert(iterMing,peer) ;
					return;
				}
			}
			m_vMingCards.push_back(peer) ;
			m_vMingCards.push_back(peer) ;
			m_vMingCards.push_back(peer) ;
		}
		break;
	case eMJAct_AnGang:
		{
			stSinglePeerCard peer ;
			peer.nCardNumber = nNumber ;
			peer.eState = eSinglePeerCard_AnGang;

			uint8_t nEraseCount= 4 ;
			auto iter = m_vAnCards.begin() ;
			for ( ; iter != m_vAnCards.end() && nEraseCount > 0; )
			{
				if ( (*iter).nCardNumber == nNumber )
				{
					m_vAnCards.erase(iter) ;
					--nEraseCount ;
					iter = m_vAnCards.begin() ;
				}
				else
				{
					++iter ;
				}
			}
			assert(nEraseCount == 0 && "why can not peng" );

			// add to ming pai
			auto iterMing = m_vMingCards.begin() ;
			for ( ; iterMing != m_vMingCards.end(); ++iterMing )
			{
				if ( (*iterMing).nCardNumber >= nNumber )
				{
					iterMing = m_vMingCards.insert(iterMing,peer) ;
					iterMing = m_vMingCards.insert(iterMing,peer) ;
					iterMing = m_vMingCards.insert(iterMing,peer) ;
					iterMing = m_vMingCards.insert(iterMing,peer) ;
					return;
				}
			}
			m_vMingCards.push_back(peer) ;
			m_vMingCards.push_back(peer) ;
			m_vMingCards.push_back(peer) ;
			m_vMingCards.push_back(peer) ;
		}
		break;
	case eMJAct_MingGang:
		{
			stSinglePeerCard peer ;
			peer.nCardNumber = nNumber ;
			peer.eState = eSinglePeerCard_MingGang;

			uint8_t nEraseCount= 3 ;
			auto iter = m_vAnCards.begin() ;
			for ( ; iter != m_vAnCards.end() && nEraseCount > 0; )
			{
				if ( (*iter).nCardNumber == nNumber )
				{
					m_vAnCards.erase(iter) ;
					--nEraseCount ;
					iter = m_vAnCards.begin() ;
				}
				else
				{
					++iter ;
				}
			}
			assert(nEraseCount == 0 && "why can not peng" );

			// add to ming pai
			auto iterMing = m_vMingCards.begin() ;
			for ( ; iterMing != m_vMingCards.end(); ++iterMing )
			{
				if ( (*iterMing).nCardNumber >= nNumber )
				{
					iterMing = m_vMingCards.insert(iterMing,peer) ;
					iterMing = m_vMingCards.insert(iterMing,peer) ;
					iterMing = m_vMingCards.insert(iterMing,peer) ;
					iterMing = m_vMingCards.insert(iterMing,peer) ;
					return;
				}
			}
			m_vMingCards.push_back(peer) ;
			m_vMingCards.push_back(peer) ;
			m_vMingCards.push_back(peer) ;
			m_vMingCards.push_back(peer) ;
		}
		break;
	case eMJAct_BuGang:
		{
			stSinglePeerCard peer ;
			peer.nCardNumber = nNumber ;
			peer.eState = eSinglePeerCard_BuGang;

			auto iter = m_vAnCards.begin() ;
			for ( ; iter != m_vAnCards.end() ; ++iter )
			{
				if ( (*iter).nCardNumber == nNumber )
				{
					m_vAnCards.erase(iter) ;
					break;
				}
			}

			// add to ming pai
			auto iterMing = m_vMingCards.begin() ;
			for ( ; iterMing != m_vMingCards.end(); ++iterMing )
			{
				if ( (*iterMing).nCardNumber >= nNumber )
				{
					iterMing = m_vMingCards.insert(iterMing,peer) ;
					return;
				}
			}
			assert( 0 && "bu gang shou not come to here");
		}
		break;
	case eMJAct_Chi:
		{
			assert(0 && "how do this" );
		}
		break;
	default:
		break;
	}
}

void CPeerCardSubCollect::getWantedCardList(LIST_WANTED_CARD& vOutList,bool bOmitChi )
{
	assert(bOmitChi == true && "not id don't want the chi" );
	// check an pai  ;
	auto iter = m_vAnCards.begin() ;
	std::map<uint8_t,uint8_t> mapCardCount ;
	for ( ; iter != m_vAnCards.end() ; ++iter )
	{
		auto cardNumber = (*iter).nCardNumber ;
		auto mapIter = mapCardCount.find(cardNumber);
		if ( mapIter == mapCardCount.end() )
		{
			mapCardCount[cardNumber] = 1 ;
		}
		else
		{
			mapIter->second = mapIter->second + 1 ;
		}
	}

	for ( auto checkCard : mapCardCount )
	{
		auto cnt = checkCard.second ;
		auto nCard = checkCard.first ;
		if ( cnt <= 1 )
		{
			continue;
		}

		if ( cnt == 3 )
		{
			stWantedCard wtc ;
			wtc.eCanInvokeAct = eMJAct_MingGang ;
			wtc.eWanteddCardFrom = ePos_Other ;
			wtc.nNumber = nCard ;
			vOutList.push_back(wtc) ;
		}

		if ( cnt >= 2 )
		{
			stWantedCard wtc ;
			wtc.eCanInvokeAct = eMJAct_Peng ;
			wtc.eWanteddCardFrom = ePos_Other ;
			wtc.nNumber = nCard ;
			vOutList.push_back(wtc) ;
		}
	}
}

void CPeerCardSubCollect::getSelfOperateCardList(LIST_WANTED_CARD& vOutList)
{
	// check an pai  ;
	auto iter = m_vAnCards.begin() ;
	std::map<uint8_t,uint8_t> mapCardCount ;
	for ( ; iter != m_vAnCards.end() ; ++iter )
	{
		auto cardNumber = (*iter).nCardNumber ;
		auto mapIter = mapCardCount.find(cardNumber);
		if ( mapIter == mapCardCount.end() )
		{
			mapCardCount[cardNumber] = 1 ;
		}
		else
		{
			mapIter->second = mapIter->second + 1 ;
		}
	}

	std::vector<uint8_t> vMayBeBuGang ;
	for ( auto checkCard : mapCardCount )
	{
		auto cnt = checkCard.second ;
		auto nCard = checkCard.first ;
		if ( cnt <= 1 )
		{
			vMayBeBuGang.push_back(nCard) ;
			continue;
		}

		if ( cnt == 4 )
		{
			stWantedCard wtc ;
			wtc.eCanInvokeAct = eMJAct_AnGang ;
			wtc.eWanteddCardFrom = ePos_Already ;
			wtc.nNumber = nCard ;
			vOutList.push_back(wtc) ;
			continue; 
		}
	}

	// check bu gang 
	auto iterBu = m_vMingCards.begin() ;
	uint8_t lastCheck = 0 ;
	for ( ; iterBu != m_vMingCards.end(); ++iterBu )
	{
		if ( (*iterBu).eState == eSinglePeerCard_Peng )
		{
			if ( lastCheck == (*iterBu).nCardNumber )
			{
				continue;
			}

			lastCheck = (*iterBu).nCardNumber ;

			for ( auto bu : vMayBeBuGang )
			{
				if ( bu == (*iterBu).nCardNumber )
				{
					stWantedCard wtc ;
					wtc.eCanInvokeAct = eMJAct_BuGang;
					wtc.eWanteddCardFrom = ePos_Already;
					wtc.nNumber = (*iterBu).nCardNumber ;
					vOutList.push_back(wtc) ;
					break;
				}
			}

		}
	}
}

void CPeerCardSubCollect::clear()
{
	m_vMingCards.clear() ;
	m_vAnCards.clear() ;
}

// peer card impl
bool CMJPeerCard::removeCardNumber( uint8_t nNumber )
{
	auto eType = CMJCard::parseCardType(nNumber) ;
	auto iter = m_vSubCollectionCards.find(eType) ;
	assert(iter != m_vSubCollectionCards.end() && "no this card how to remove" );
	auto b = iter->second.removeCardNumber(nNumber) ;
	if ( b )
	{
		if ( iter->second.getCardCount() == 0 )
		{
			m_vSubCollectionCards.erase(iter) ;
		}
	}
	return b ;
}

void CMJPeerCard::doAction(eMJActType eType, uint8_t nNumber )
{
	auto eCardThisType = CMJCard::parseCardType(nNumber) ;
	auto iter = m_vSubCollectionCards.find(eCardThisType) ;
	assert(iter != m_vSubCollectionCards.end() || eType == eMJAct_Mo && "no this card how to remove" );
	if ( iter == m_vSubCollectionCards.end() && eType == eMJAct_Mo )
	{
		CPeerCardSubCollect v ;
		v.doAction(eType,nNumber) ;
		m_vSubCollectionCards[eCardThisType] = v ;
		return ;
	}
	iter->second.doAction(eType,nNumber);
}

void CMJPeerCard::updateWantedCard(LIST_WANTED_CARD& vWantList)
{
	for ( auto ref : m_vSubCollectionCards )
	{
		if ( ref.first == getMustQueType() )
		{
			continue;
		}
		ref.second.getWantedCardList(vWantList,true);
	}

	LIST_WANTED_CARD vList ;
	if ( CBloodFanxing::getInstance()->checkFanXingWantedCards(*this,vList) )
	{
		vWantList.insert(vWantList.begin(),vList.begin(),vList.end()) ;
	}
}

void CMJPeerCard::updateSelfOperateCard( LIST_WANTED_CARD& vOperateList, uint8_t nNewCard )
{
	for ( auto ref : m_vSubCollectionCards )
	{
		if ( ref.first == getMustQueType() )
		{
			continue;
		}
		ref.second.getSelfOperateCardList(vOperateList);
	}

	eFanxingType eFtype ;
	uint8_t nFanShu = 0 ;
	if ( CBloodFanxing::getInstance()->checkHuPai(*this,eFtype,nFanShu) )
	{
		stWantedCard stWt;
		stWt.eCanInvokeAct = eMJAct_Hu ;
		stWt.eFanxing = eFtype ;
		stWt.eWanteddCardFrom = ePos_Already ;
		stWt.nFanRate = nFanShu ;
		stWt.nNumber = nNewCard ;
		vOperateList.push_back(stWt) ;
	}
}

bool CMJPeerCard::isContainMustQue()
{
	auto iter = m_vSubCollectionCards.find(getMustQueType()) ;
	return iter != m_vSubCollectionCards.end() ;
}

void CMJPeerCard::reset()
{
	m_vSubCollectionCards.clear() ;
	m_eMustQueType = eCT_Max ;
}

void CMJPeerCard::addCard( uint8_t nCardNuber )
{
	doAction(eMJAct_Mo,nCardNuber);
}

uint8_t CMJPeerCard::doHuPaiFanshu( uint8_t nCardNumber , uint8_t& nGenShu ) // nCardNumber = 0 , means self mo ; return value not include gen ;
{
	if ( nCardNumber != 0 )
	{
		addCard(nCardNumber) ;
	}

	eFanxingType eFtype ;
	uint8_t nFanShu = 0 ;
	if ( CBloodFanxing::getInstance()->checkHuPai(*this,eFtype,nFanShu) )
	{
		nGenShu = getGenShu() ;
		if ( eFtype == eFanxing_LongQiDui )
		{
			nGenShu -= 1 ;
		}

		if ( nCardNumber != 0 )
		{
			removeCardNumber(nCardNumber) ;
			// must left 13 count card , because player can hu more than once 
		} 
		CLogMgr::SharedLogMgr()->PrintLog("player hu pai , type = %u , fanShu = %u",eFtype,nFanShu) ;
		return nFanShu ;
	}
	else
	{
		assert(0 && "why ? can not  hu ? " );
		if ( nCardNumber != 0 )
		{
			removeCardNumber(nCardNumber) ;
		}
	}
	nGenShu = 0 ;
	return 0 ;
}

uint8_t CMJPeerCard::getMaxHuPaiFanShu( uint8_t& nGenShu )
{
	LIST_WANTED_CARD vList ;
	if ( ! CBloodFanxing::getInstance()->checkFanXingWantedCards(*this,vList) )
	{
		 return 0 ;
	}

	assert(vList.empty() == false && "can hu , but is null why ? already hu ?" );

	uint8_t nMaxFan = 0 ;
	eFanxingType eMaxFtype ;
	
	for ( auto ref : vList )
	{
		addCard(ref.nNumber) ;

		eFanxingType eFtype ;
		uint8_t nFanShu = 0 ;
		if ( CBloodFanxing::getInstance()->checkHuPai(*this,eFtype,nFanShu) )
		{
			if ( nFanShu > nMaxFan )
			{
				nMaxFan = nFanShu ;
				eMaxFtype = eFtype ;
				nGenShu = getGenShu();
			}
		}
		else
		{
			assert(0 && "why ? can not  hu ? , you say can hu" );
		}

		removeCardNumber(ref.nNumber) ;
	}
	
	if ( eMaxFtype == eFanxing_LongQiDui )
	{
		nGenShu -= 1 ;
	}
	return nMaxFan ;
}

uint8_t CMJPeerCard::getGenShu()
{
	uint8_t nGen = 0 ;
	for ( auto ref : m_vSubCollectionCards )
	{
		nGen += ref.second.getGenCount() ;
	}

	return nGen ;
}

uint8_t CMJPeerCard::getCardByIdx(uint8_t nCardIdx, bool isForExchange )
{
	uint8_t nLestCnt = 1 ;
	if ( isForExchange )
	{
		nLestCnt = 3 ;
	}

	eMJCardType type = eMJCardType::eCT_Max ;
	uint8_t nCnt = 0 ;
	for ( auto ref : m_vSubCollectionCards )
	{
		if ( ref.second.getAnPaiCount() >= nLestCnt )
		{
			if ( nCnt == 0 || ref.second.getAnPaiCount() < nCnt)
			{
				type = ref.first ;
				nCnt = ref.second.getAnPaiCount();
			}
		}
	}

	auto iter = m_vSubCollectionCards.find(type) ;
	assert(iter != m_vSubCollectionCards.end() && "why have no card?" );
	
	for ( auto ref : iter->second.m_vAnCards )
	{
		if ( nCardIdx == 0 )
		{
			return ref.nCardNumber ;
		}

		--nCardIdx ;
	}
	assert(0&& "why can not find proper card ?" );
	return 0 ;
}

bool CMJPeerCard::isHaveAnCard(uint8_t nCardNumber)
{
	for ( auto ref : m_vSubCollectionCards )
	{
		for ( auto refValue : ref.second.m_vAnCards )
		{
			if ( refValue.nCardNumber == nCardNumber )
			{
				return true ;
			}
		}
	}

	return false ;
}

void CMJPeerCard::getAnPai(Json::Value& vAnPia )
{
	for ( auto ref : m_vSubCollectionCards )
	{
		for ( auto refValue : ref.second.m_vAnCards )
		{
			vAnPia[vAnPia.size()] = refValue.nCardNumber ;
		}
	}
}

void CMJPeerCard::getMingPai( Json::Value& vMingPia )
{
	for ( auto ref : m_vSubCollectionCards )
	{
		for ( auto refValue : ref.second.m_vMingCards )
		{
			vMingPia[vMingPia.size()] = refValue.nCardNumber ;
		}
	}
}

void  CMJPeerCard::debugAnpaiCount()
{
	uint8_t nTotal = 0 ;
	for ( auto ref : m_vSubCollectionCards )
	{
		nTotal += ref.second.getAnPaiCount() ;
		//CLogMgr::SharedLogMgr()->PrintLog("An pai type = %u, cnt = %u\n",ref.first,ref.second.getAnPaiCount()) ;
		for ( auto refValue : ref.second.m_vAnCards )
		{
			//CLogMgr::SharedLogMgr()->PrintLog("An pai Number = %u\n",refValue.nCardNumber) ;
		}
	}

	CLogMgr::SharedLogMgr()->PrintLog("total cnt = %u",nTotal) ;
}