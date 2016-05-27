#include "MJBloodFanxing.h"
#include <cassert>
#include "MJCard.h"
CBloodFanxingPingHu::CBloodFanxingPingHu()
{
	auto pp = new CBloodQingYiSe ;
	addChildFanXing(pp);

	auto ppp = new CBloodFanxingDaiYaoJiu;
	addChildFanXing(ppp) ;

	auto ppD = new CBloodFanxingDuiDuiHu ;
	addChildFanXing(ppD) ;
}

bool CBloodFanxingPingHu::checkFanXingWantedCards(CMJPeerCard& peerCard, LIST_WANTED_CARD& vWanted)
{
	uint8_t nWantCount = vWanted.size() ;
	if ( peerCard.isContainMustQue() )
	{
		return false ;
	}

	std::vector<uint8_t> vAnCards;
	for ( auto ref : peerCard.m_vSubCollectionCards )
	{
		for ( auto singlCard : ref.second.m_vAnCards )
		{
			vAnCards.push_back(singlCard.nCardNumber) ;
		}
	}

	std::vector<uint8_t> vOut ;
	std::vector<uint8_t> vAll ;
	if ( checkHaveJiangJustQueShun(vAnCards,vOut) )
	{
		vAll.assign(vOut.begin(),vOut.end()) ;
	}

	if ( checkJustQueJiang( vAnCards,vOut ) )
	{
		vAll.insert(vAll.begin(),vOut.begin(),vOut.end());
	}
	
	// wanted card mo ban ;
	stWantedCard stWant;
	stWant.eCanInvokeAct = eMJAct_Hu ;
	stWant.eFanxing = getType() ;
	stWant.eWanteddCardFrom = ePos_Any ;
	stWant.nFanRate = getFanRate();
	for ( auto ref : vAll )
	{
		stWant.nNumber = ref ;
		vWanted.push_back(stWant) ;
	}
	return nWantCount < vWanted.size() ;
}

bool CBloodFanxingPingHu::checkType(CMJPeerCard& peerCard)
{
	if ( peerCard.isContainMustQue() )
	{
		return false ;
	}

	std::vector<uint8_t> vAnCards;
	for ( auto ref : peerCard.m_vSubCollectionCards )
	{
		for ( auto singlCard : ref.second.m_vAnCards )
		{
			vAnCards.push_back(singlCard.nCardNumber) ;
		}
	}

	if ( vAnCards.size() % 3 != 2 )
	{
		return false ;
	}

	if ( vAnCards.size() == 2 && vAnCards[0] == vAnCards[1] )
	{
		return true ;
	}

	std::vector<uint8_t> vmayBeJiang ;
	for ( uint8_t nIdx = 0 ; nIdx + 1 < vAnCards.size(); )
	{
		if ( vAnCards[nIdx] == vAnCards[nIdx + 1] )
		{
			if ( vmayBeJiang.empty() || vAnCards[nIdx] != vmayBeJiang.back() )
			{
				vmayBeJiang.push_back(vAnCards[nIdx]);
			}
			nIdx += 2 ;
		}
		else
		{
			++nIdx ;
		}
	}

	if ( vmayBeJiang.empty() )
	{
		return false ;
	}

	for ( auto nJiang : vmayBeJiang )
	{
		std::vector<uint8_t> vecCheck ;
		uint8_t nSkipJiang = 0 ;
		for ( uint8_t nIdx = 0 ; nIdx < vAnCards.size() ; ++nIdx )
		{
			if ( vAnCards[nIdx] == nJiang && nSkipJiang < 2 )
			{
				++nSkipJiang ;
				continue; 
			}

			vecCheck.push_back(vAnCards[nIdx]) ;
		}

		std::vector<std::vector<uint8_t>> tTemp ;
 		if ( findAllShun(vecCheck,tTemp) )
		{
			return true ;
		}
	}

	return false ;
}

// help function 
bool CBloodFanxingPingHu::findQueShun( std::vector<uint8_t>& vec ,uint8_t& nQueIdx )
{
	uint8_t nNextQue = nQueIdx + 1 ;
	if ( nNextQue >= vec.size() ) 
	{
		return false ;
	}

	uint8_t nVal = vec[nQueIdx];
	uint8_t nVal2 = vec[nNextQue] ;
	if ( nVal2 - nVal <= 2 )
	{
		// make a vec for check 
		std::vector<uint8_t> vecCheck ;
		for ( uint8_t nIdx = 0 ; nIdx < vec.size() ; ++nIdx )
		{
			if ( nIdx == nNextQue || nQueIdx == nIdx )
			{
				continue;
			}
			vecCheck.push_back(vec[nIdx]) ;
		}

		std::vector<std::vector<uint8_t>> vecOutShunzi ;
		if ( findAllShun(vecCheck,vecOutShunzi) )
		{
			return true ;
		}
	}
	

	if ( findQueShun(vec,nNextQue) )
	{
		nQueIdx = nNextQue ;
		return true ;
	}
	return false ;
}

bool CBloodFanxingPingHu::checkHaveJiangJustQueShun( std::vector<uint8_t>& vec, std::vector<uint8_t>& vecOutQue )
{
	uint8_t nCnt = vecOutQue.size() ;
	if ( vec.size() < 4 )
	{
		return false ;
	}

	if ( ( vec.size() - 2 ) % 3 != 2 )
	{
		return false ;
	}

	std::vector<uint8_t> vmayBeJiang ;
	for ( uint8_t nIdx = 0 ; nIdx + 1 < vec.size(); )
	{
		 if ( vec[nIdx] == vec[nIdx + 1] )
		 {
			 if ( vmayBeJiang.empty() || vec[nIdx] != vmayBeJiang.back() )
			 {
				 vmayBeJiang.push_back(vec[nIdx]);
			 }
			 nIdx += 2 ;
		 }
		 else
		 {
			 ++nIdx ;
		 }
	}

	if ( vmayBeJiang.empty() )
	{
		return false ;
	}

	for ( auto nJiang : vmayBeJiang )
	{
		std::vector<uint8_t> vecCheck ;
		uint8_t nSkipJiang = 0 ;
		for ( uint8_t nIdx = 0 ; nIdx < vec.size() ; ++nIdx )
		{
			if ( vec[nIdx] == nJiang && nSkipJiang < 2 )
			{
				++nSkipJiang ;
				continue; 
			}

			vecCheck.push_back(vec[nIdx]) ;
		}

		std::vector<uint8_t> vecQueShun ;
		if ( vecCheck.size() == 2 )
		{
			if ( vecCheck[1] - vecCheck[0] <= 2 )
			{
				vecQueShun.assign(vecCheck.begin(),vecCheck.end()) ;
			}
		}
		else
		{
			for ( uint8_t nQueIdx = 0 ; nQueIdx + 1 < vecCheck.size() ; ++nQueIdx)
			{
				if ( findQueShun(vecCheck,nQueIdx) )
				{
					vecQueShun.push_back(vecCheck[nQueIdx]);
					vecQueShun.push_back(vecCheck[nQueIdx+1]);
				}
				else
				{
					break;
				}
			}
		}

		for ( uint8_t nCIdx = 0 ; nCIdx + 1 < vecQueShun.size() ; nCIdx += 2 )
		{
			uint8_t nNumberLeft = vecQueShun[nCIdx] ;
			uint8_t nNumberRight = vecQueShun[nCIdx + 1 ] ;
			uint8_t nV = CMJCard::parseCardValue(nNumberLeft);
			uint8_t nV1 = CMJCard::parseCardValue(nNumberRight);
			if ( nV == nV1 )
			{
				vecOutQue.push_back(nNumberLeft) ;
			}
			else if ( nV + 1 == nV1 )
			{
				if ( nV > 1 )
				{
					vecOutQue.push_back(nNumberLeft - 1 ) ;
				}

				if ( nV1 < 9 )
				{
					vecOutQue.push_back(nNumberRight + 1 ) ;
				}
			}
			else if ( nV + 2 == nV1 )
			{
				vecOutQue.push_back(nNumberLeft + 1 ) ;
			}
			else
			{
				assert( 0 && "error can not come to here " );
			}
		}
	}

	return nCnt < vecOutQue.size() ;
}

bool CBloodFanxingPingHu::checkJustQueJiang( std::vector<uint8_t>& vec, std::vector<uint8_t>& vecOutQue )
{
	if ( vec.size() % 3 != 1 )
	{
		return false ;
	}

	if ( vec.size() == 1 )
	{
		vecOutQue.push_back(vec[0]) ;
		return true ;
	}

	bool bFind = false ;
	for ( auto nQueJiang : vec )
	{
		std::vector<uint8_t> vecCheckShun ;
		for ( uint8_t nIdx = 0 ; nIdx < vec.size(); ++nIdx )
		{
			if ( vec[nIdx] != nQueJiang )
			{
				vecCheckShun.push_back(vec[nIdx]) ;
			}
		}

		std::vector<std::vector<uint8_t>> vecShun ;
		if ( findAllShun(vecCheckShun,vecShun) )
		{
			vecOutQue.push_back(nQueJiang) ;
			bFind = true ;
		}
	}

	return bFind ;
}

bool CBloodFanxingPingHu::findAllShun( std::vector<uint8_t>& vec , std::vector<std::vector<uint8_t>>& vecOutShunzi )
{
	if ( vec.size() % 3 != 0 )
	{
		return false ;
	}

	if ( vec.empty() )
	{
		printf("why check count is 0 \n") ;
		return true ;
	}
	
	// find 3 same shun 
	std::vector<uint8_t> vecCheck(vec);
	std::vector<uint8_t> vecShun ;
	for ( uint8_t nIdx = 0 ; nIdx + 2 < vecCheck.size() ; )
	{
		if ( vecCheck[nIdx] == vecCheck[nIdx + 1] && vecCheck[nIdx] == vecCheck[nIdx + 2] ) // find one 
		{
			vecShun.clear() ;
			vecShun.push_back(vecCheck[nIdx]);
			vecShun.push_back(vecCheck[nIdx+1]);
			vecShun.push_back(vecCheck[nIdx+2]);
			vecOutShunzi.push_back(vecShun) ;

			// mark found item as 0 ;
			vecCheck[nIdx] = 0 ;
			vecCheck[nIdx + 1] = 0 ;
			vecCheck[nIdx + 2 ] = 0;

			// go on seek ;
			nIdx += 3 ;
		}
		else 
		{
			++nIdx ;
		}
	}

	// seak not 3 same shun ;
	vecShun.clear() ;
	for ( auto ref : vecCheck )
	{
		if ( ref == 0 )
		{
			continue;
		}

		if ( vecShun.empty() )
		{
			vecShun.push_back(ref) ;
			continue;
		}

		if ( ref == vecShun.back() + 1 )
		{
			vecShun.push_back(ref) ;
			if ( vecShun.size() == 3 )  // find a shun 
			{
				vecOutShunzi.push_back(vecShun) ;
				vecShun.clear() ;
			}

			continue;
		}

		return false ;
	}

	return true ;
}

// qing yi se 
bool CBloodQingYiSe::checkType(CMJPeerCard& peerCard)
{
	auto fi = peerCard.m_vSubCollectionCards.begin() ;
	return peerCard.m_vSubCollectionCards.size() == 1 && fi->first != peerCard.getMustQueType() ;
}

// qi dui 
CBloodFanxingQiDui::CBloodFanxingQiDui()
{
	auto pp = new CBloodFanxingQingQiDui ;
	addChildFanXing(pp);

	auto ppp = new CBloodFanxingLongQiDui ;
	addChildFanXing(ppp);
}

bool CBloodFanxingQiDui::checkFanXingWantedCards(CMJPeerCard& peerCard, LIST_WANTED_CARD& vWanted)
{
	uint8_t nWantCount = vWanted.size() ;
	if ( peerCard.isContainMustQue() )
	{
		return false ;
	}

	std::vector<uint8_t> vAnCards;
	for ( auto ref : peerCard.m_vSubCollectionCards )
	{
		for ( auto singlCard : ref.second.m_vAnCards )
		{
			vAnCards.push_back(singlCard.nCardNumber) ;
		}
	}

	if ( vAnCards.size() != 13 )
	{
		printf("qi dui , must have 13 an pai \n");
		return false ;
	}
	
	// find may be que jing ;
	uint8_t nFindQue = 0 ;
	for ( uint8_t nIdx = 0 ; nIdx + 1 < vAnCards.size() ;  )
	{
		if ( vAnCards[nIdx] != vAnCards[nIdx + 1] )
		{
			if ( nFindQue != 0 )
			{
				return false ;
			}

			nFindQue = vAnCards[nIdx] ;
			++nIdx;
			continue;
		}
		nIdx += 2 ;
	}
	stWantedCard stWant;
	stWant.eCanInvokeAct = eMJAct_Hu ;
	stWant.eFanxing = getType() ;
	stWant.eWanteddCardFrom = ePos_Any ;
	stWant.nFanRate = getFanRate();
	stWant.nNumber = nFindQue ;
	vWanted.push_back(stWant) ;
	return true ;
}

bool CBloodFanxingQiDui::checkType(CMJPeerCard& peerCard)
{
	if ( peerCard.isContainMustQue() )
	{
		return false ;
	}

	std::vector<uint8_t> vAnCards;
	for ( auto ref : peerCard.m_vSubCollectionCards )
	{
		for ( auto singlCard : ref.second.m_vAnCards )
		{
			vAnCards.push_back(singlCard.nCardNumber) ;
		}
	}

	if ( vAnCards.size() != 14 )
	{
		printf("qi dui , must have 14 an pai \n");
		return false ;
	}

	for ( uint8_t nIdx = 0 ; nIdx + 1 < vAnCards.size() ; nIdx += 2 )
	{
		if ( vAnCards[nIdx] != vAnCards[nIdx + 1] )
		{
			return false ;
		}
	}
	return true ;
}

// logn qi dui 
CBloodFanxingLongQiDui::CBloodFanxingLongQiDui()
{
	auto pp = new CBloodFanxingQingLongQiDui ;
	addChildFanXing(pp);
}

bool CBloodFanxingLongQiDui::checkType(CMJPeerCard& peerCard)
{
	std::vector<uint8_t> vAnCards;
	for ( auto ref : peerCard.m_vSubCollectionCards )
	{
		for ( auto singlCard : ref.second.m_vAnCards )
		{
			vAnCards.push_back(singlCard.nCardNumber) ;
		}
	}

	for ( uint8_t nIdx = 0 ; nIdx + 3 < vAnCards.size() ; nIdx += 2 )
	{
		if ( vAnCards[nIdx] == vAnCards[nIdx + 1] && vAnCards[nIdx] == vAnCards[nIdx + 2]  && vAnCards[nIdx] == vAnCards[nIdx + 3])
		{
			return true ;
		}
	}

	return false ;
}

// dui dui hu 
CBloodFanxingDuiDuiHu::CBloodFanxingDuiDuiHu()
{
	auto p = new CBloodFanxingQingDuiDuiHu ;
	addChildFanXing(p) ;

	auto pp = new CBloodFanxingJinGouDiao ;
	addChildFanXing(pp) ;

	auto pppp = new CBloodFanxingShiBaLuoHan ;
	addChildFanXing(pppp) ;
}

bool CBloodFanxingDuiDuiHu::checkType(CMJPeerCard& peerCard)
{
	std::vector<uint8_t> vAnCards;
	for ( auto ref : peerCard.m_vSubCollectionCards )
	{
		for ( auto singlCard : ref.second.m_vAnCards )
		{
			vAnCards.push_back(singlCard.nCardNumber) ;
		}
	}

	// dui dui hu , can must can not find 3 card , different each other  
	uint8_t nFindQue = 0 ;
	for ( uint8_t nIdx = 0 ; nIdx + 2 < vAnCards.size() ; ++nIdx )
	{
		if ( vAnCards[nIdx] != vAnCards[nIdx + 1] && vAnCards[nIdx] != vAnCards[nIdx + 2]  && vAnCards[nIdx+1] != vAnCards[nIdx + 2])
		{
			return false ;
		}
	}
	return true ;
}

// jin gou gou 
CBloodFanxingJinGouDiao::CBloodFanxingJinGouDiao()
{
	auto pp = new CBloodFanxingQingJinGouDiao ;
	addChildFanXing(pp) ;

	auto ppp = new CBloodFanxingJiangJinGouDiao ;
	addChildFanXing(ppp) ;
}

bool CBloodFanxingJinGouDiao::checkType(CMJPeerCard& peerCard)
{
	std::vector<uint8_t> vAnCards;
	for ( auto ref : peerCard.m_vSubCollectionCards )
	{
		for ( auto singlCard : ref.second.m_vAnCards )
		{
			vAnCards.push_back(singlCard.nCardNumber) ;
		}
	}

	return vAnCards.size() == 2 ;
}

// shi ba luo han 
bool CBloodFanxingShiBaLuoHan::checkType(CMJPeerCard& peerCard)
{
	std::vector<uint8_t> vAnCards;
	for ( auto ref : peerCard.m_vSubCollectionCards )
	{
		for ( auto singlCard : ref.second.m_vAnCards )
		{
			vAnCards.push_back(singlCard.nCardNumber) ;
		}

		for ( auto singlCard : ref.second.m_vMingCards )
		{
			vAnCards.push_back(singlCard.nCardNumber) ;
		}
	}

	return vAnCards.size() == 18 ; 
}

// jiang jin gou diao 
bool CBloodFanxingJiangJinGouDiao::checkType(CMJPeerCard& peerCard)
{
	std::vector<uint8_t> vAnCards;
	for ( auto ref : peerCard.m_vSubCollectionCards )
	{
		for ( auto singlCard : ref.second.m_vAnCards )
		{
			uint8_t nValue = CMJCard::parseCardValue(singlCard.nCardNumber) ;
			if ( nValue != 5 && 8 != nValue && 2 != nValue )
			{
				return false ;
			}
		}

		for ( auto singlCard : ref.second.m_vMingCards )
		{
			uint8_t nValue = CMJCard::parseCardValue(singlCard.nCardNumber) ;
			if ( nValue != 5 && 8 != nValue && 2 != nValue )
			{
				return false ;
			}
		}
	}

	return true ;
}

// dai yao jiu 
CBloodFanxingDaiYaoJiu::CBloodFanxingDaiYaoJiu()
{
	auto pp = new CBloodFanxingQingDaiYaoJiu ;
	addChildFanXing(pp) ;
}

bool CBloodFanxingDaiYaoJiu::checkType(CMJPeerCard& peerCard)
{
	std::vector<uint8_t> vAnCards;
	for ( auto ref : peerCard.m_vSubCollectionCards )
	{
		for ( auto singlCard : ref.second.m_vAnCards )
		{
			vAnCards.push_back(singlCard.nCardNumber) ;
		}
	}

	if ( vAnCards.size() != 14 )
	{
		return false ;
	}
	
	for ( uint8_t nIdx = 0 ; nIdx < vAnCards.size() ; ++nIdx )
	{
		uint8_t nValue = CMJCard::parseCardValue(vAnCards[nIdx]) ;
		if ( nValue == 1|| nValue == 9 )
		{
			continue;
		}

		if ( nValue > 3 && nValue < 7 )
		{
			return false ;
		}

		if ( nIdx + 1 < vAnCards.size() )  // only 1  or 9 can be pair  
		{
			if ( vAnCards[nIdx] == vAnCards[nIdx + 1 ] )
			{
				return false ;
			}
		}
	}

	return true ;
}

// blood fan xin 

bool CBloodFanxing::checkHuPai(CMJPeerCard& peerCard , eFanxingType & eHuType , uint8_t& nFanshu )
{
	if ( m_tPingHu.checkFinalType(peerCard,eHuType,nFanshu) )
	{
		return true ;
	}

	if ( m_tQiDui.checkFinalType(peerCard,eHuType,nFanshu) )
	{
		return true ;
	}

	return false ;
}

bool CBloodFanxing::checkFanXingWantedCards(CMJPeerCard& peerCard, LIST_WANTED_CARD& vWaited) 
{
	if ( m_tPingHu.checkFanXingWantedCards(peerCard,vWaited) )
	{
		return true ;
	}

	if ( m_tQiDui.checkFanXingWantedCards(peerCard,vWaited) )
	{
		return true ;
	}

	return false ;
}
