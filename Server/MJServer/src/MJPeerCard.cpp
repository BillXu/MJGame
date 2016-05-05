#include "MJPeerCard.h"
#include <cassert>
#include "MJCard.h"
#include "MJBloodFanxing.h"
bool CPeerCardSubCollect::removeCardNumber( uint8_t nNumber )
{
	auto iter = m_vAnCards.begin() ;
	for ( ; iter != m_vAnCards.end(); ++iter )
	{
		if ( (*iter).nCardNumber == nNumber )
		{
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
	std::vector<uint8_t> vAllCard ;
	for ( auto ref : m_vAnCards )
	{
		vAllCard.push_back(ref.nCardNumber) ;
	}

	for ( auto ref : m_vMingCards )
	{
		auto iter = vAllCard.begin();
		for ( ; iter != vAllCard.end() ; ++iter )
		{
			if ( (*iter) > ref.nCardNumber )
			{
				vAllCard.insert(iter,ref.nCardNumber) ;
				break;
			}
			vAllCard.push_back(ref.nCardNumber) ;
		}
	}

	uint8_t nCnt = 0 ;
	for ( uint8_t nIdx = 0; uint8_t(nIdx + 3) < vAllCard.size() ; )
	{
		if ( vAllCard[nIdx] == vAllCard[nIdx + 3] )
		{
			++nCnt ;
			nIdx += 4 ;
		}
		else
		{
			++nIdx ;
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
			for ( ; iter != m_vAnCards.end() && nEraseCount > 0; ++iter )
			{
				if ( (*iter).nCardNumber == nNumber )
				{
					m_vAnCards.erase(iter) ;
					--nEraseCount ;
					iter = m_vAnCards.begin() ;
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
			for ( ; iter != m_vAnCards.end() && nEraseCount > 0; ++iter )
			{
				if ( (*iter).nCardNumber == nNumber )
				{
					m_vAnCards.erase(iter) ;
					--nEraseCount ;
					iter = m_vAnCards.begin() ;
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
			for ( ; iter != m_vAnCards.end() && nEraseCount > 0; ++iter )
			{
				if ( (*iter).nCardNumber == nNumber )
				{
					m_vAnCards.erase(iter) ;
					--nEraseCount ;
					iter = m_vAnCards.begin() ;
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
	LIST_PEER_CARDS vListTemp ;
	uint8_t nCurListCardNumber = 0 ;
	for ( ; iter != m_vAnCards.end() ; ++iter )
	{
		if ( vListTemp.empty() )
		{
			vListTemp.push_back(*iter) ;
			nCurListCardNumber = (*iter).nCardNumber ;
			continue;
		}

		if ( nCurListCardNumber == (*iter).nCardNumber )
		{
			vListTemp.push_back(*iter) ;
			continue;
		}
		// another number card ;

		// check preType 
		if ( vListTemp.size() == 4 )
		{
			stWantedCard wtc ;
			wtc.eCanInvokeAct = eMJAct_AnGang ;
			wtc.eWanteddCardFrom = ePos_Self ;
			wtc.nNumber = nCurListCardNumber ;
			vOutList.push_back(wtc) ;
		}
		else if ( vListTemp.size() == 3 )
		{
			stWantedCard wtc ;
			wtc.eCanInvokeAct = eMJAct_MingGang ;
			wtc.eWanteddCardFrom = ePos_Other ;
			wtc.nNumber = nCurListCardNumber ;
			vOutList.push_back(wtc) ;

			// peng
			wtc.eCanInvokeAct = eMJAct_Peng ;
			wtc.eWanteddCardFrom = ePos_Other ;
			wtc.nNumber = nCurListCardNumber ;
			vOutList.push_back(wtc) ;
		}
		else if ( vListTemp.size() == 2 )
		{
			stWantedCard wtc ;
			wtc.eCanInvokeAct = eMJAct_Peng ;
			wtc.eWanteddCardFrom = ePos_Other ;
			wtc.nNumber = nCurListCardNumber ;
			vOutList.push_back(wtc) ;
		}
		else
		{
			// clear pre and add new ;
			nCurListCardNumber = 0 ;
			vListTemp.clear() ;
			vListTemp.push_back(*iter) ;
		}
	}

	// check bu gang 
	auto iterBu = m_vMingCards.begin() ;
	uint8_t lastCheck = 0 ;
	for ( ; iterBu != m_vMingCards.begin(); ++iterBu )
	{
		if ( (*iterBu).eState == eSinglePeerCard_Peng && lastCheck != (*iterBu).nCardNumber )
		{
			lastCheck = (*iterBu).nCardNumber ;
			stWantedCard wtc ;
			wtc.eCanInvokeAct = eMJAct_BuGang;
			wtc.eWanteddCardFrom = ePos_Self;
			wtc.nNumber = lastCheck ;
			vOutList.push_back(wtc) ;
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
