#include "MJPeerCardNew.h"
#include "MJCard.h"
#include <cassert>
#include <algorithm>
void CMJPeerCardNew::reset()
{
	vecHoldCard.clear();
	vecPengedCard.clear();
	vecMingGangCard.clear();
	vecAnGangCard.clear();
	vecEatedCard.clear();
	vecChuCard.clear();
}

bool CMJPeerCardNew::init()
{
	reset();
	return true ;
}

void CMJPeerCardNew::addHoldCard(uint8_t nCard )
{
	addNumberToVecWithAsc(vecHoldCard,nCard) ;
	m_isHuPaiInfoDirty = true ;
}

// check function 
bool CMJPeerCardNew::isCardCanPeng( uint8_t nCard )
{
	uint8_t nFind = 0 ;
	for ( auto& ref : vecHoldCard )
	{
		if ( ref == nCard )
		{
			++nFind ;
		}

		if ( nFind >= 2 )
		{
			return true ;
		}
	}

	return false ;
}

bool CMJPeerCardNew::isCardCanMingGang( uint8_t nCard )
{
	uint8_t nFind = 0 ;
	for ( auto& ref : vecHoldCard )
	{
		if ( ref == nCard )
		{
			++nFind ;
		}

		if ( nFind >= 3 )
		{
			return true ;
		}
	}

	return false ;
}

bool CMJPeerCardNew::isCardCanBuGang( uint8_t nCard )
{
	auto iter = std::find(vecPengedCard.begin(),vecPengedCard.end(),nCard) ;
	return iter != vecPengedCard.end();
}

bool CMJPeerCardNew::isCardCanAnGang(uint8_t nCard )
{
	uint8_t nFind = 0 ;
	for ( auto& ref : vecHoldCard )
	{
		if ( ref == nCard )
		{
			++nFind ;
		}

		if ( nFind >= 4 )
		{
			return true ;
		}
	}

	return false ;
}

bool CMJPeerCardNew::isHaveAnGangCards( std::vector<uint8_t>& vAnGangCards )
{
	uint8_t nFind = 0 ;
	uint8_t nCheckCard = 0 ;
	for ( auto& ref : vecHoldCard )
	{
		if ( nCheckCard == 0 )
		{
			++nFind;
			nCheckCard = ref ;
			continue;
		}

		if ( nCheckCard == ref )
		{
			++nFind ;
		}
		else
		{
			nFind = 1 ;
			nCheckCard = ref ;
			continue;
		}

		if ( nFind == 4 )
		{
			vAnGangCards.push_back(nCheckCard) ;
			nFind = 0 ;
			nCheckCard = 0 ;
		}
	}

	return vAnGangCards.empty() == false ;
}

bool CMJPeerCardNew::isHaveBuGang( std::vector<uint8_t>& vAnGangCards )
{
	for ( auto& ref : vecPengedCard )
	{
		auto iter = std::find(vecHoldCard.begin(),vecHoldCard.end(),ref ) ;
		if ( iter != vecHoldCard.end() )
		{
			vAnGangCards.push_back(ref);
		}
	}
	return vAnGangCards.empty() == false ;
}

bool CMJPeerCardNew::isCardCanHu( uint8_t nCard )
{
	if ( nCard )
	{
		addNumberToVecWithAsc(vecHoldCard,nCard) ;
		auto isHu = ptrHupaiInfo->parseHuPaiInfo(vecHoldCard) ;
		removeNumberFromVec(vecHoldCard,nCard) ;
		return isHu ;
	}

	if ( this->m_isHuPaiInfoDirty )
	{
		ptrHupaiInfo->parseHuPaiInfo(vecHoldCard) ;
		this->m_isHuPaiInfoDirty = false ;
	}
	
	return ptrHupaiInfo->isHu() ;
}

bool CMJPeerCardNew::isCardCanEat( uint8_t nCard, VEC_EAT_PAIR& vEatPairs )
{
	auto carType = CMJCard::parseCardType(nCard);
	if ( carType == eCT_Feng || eCT_Jian == carType )
	{
		return false ;
	}

	VEC_EAT_PAIR vecCandinate ;
	// AB X
	stEatPair p ;
	p.nCard[0] = nCard - 2 ;
	p.nCard[1] = nCard - 1 ;
	vecCandinate.push_back(p) ;

	// A X C
	p.nCard[0] = nCard - 1 ;
	p.nCard[1] = nCard + 1 ;
	vecCandinate.push_back(p) ;

	// X B C 
	p.nCard[0] = nCard + 1 ;
	p.nCard[1] = nCard + 2 ;
	vecCandinate.push_back(p) ;

	/*auto carType = CMJCard::parseCardType(nCard);
	if ( carType == eCT_Feng )
	{
		auto faceNu = CMJCard::parseCardValue(nCard) ;
		if ( faceNu == 1 )
		{
			p.nCard[0] = CMJCard::makeCardNumber(eCT_Feng,3);
			p.nCard[1] = CMJCard::makeCardNumber(eCT_Feng,4);
			vecCandinate.push_back(p) ;

			p.nCard[0] = CMJCard::makeCardNumber(eCT_Feng,2); ;
			p.nCard[1] = CMJCard::makeCardNumber(eCT_Feng,4); ;
			vecCandinate.push_back(p) ;
		}

		if ( 2 == faceNu || 3 == faceNu )
		{
			p.nCard[0] = CMJCard::makeCardNumber(eCT_Feng,1); ;
			p.nCard[1] = CMJCard::makeCardNumber(eCT_Feng,4); ;
			vecCandinate.push_back(p) ;
		}

		if ( 4 == faceNu )
		{
			p.nCard[0] = CMJCard::makeCardNumber(eCT_Feng,1); ;
			p.nCard[1] = CMJCard::makeCardNumber(eCT_Feng,3); ;
			vecCandinate.push_back(p) ;

			p.nCard[0] = CMJCard::makeCardNumber(eCT_Feng,1); ;
			p.nCard[1] = CMJCard::makeCardNumber(eCT_Feng,2); ;
			vecCandinate.push_back(p) ;
		}
	}*/

	// check invalid 
	for ( auto& ref : vecCandinate )
	{
		auto iterA = std::find(vecHoldCard.begin(),vecHoldCard.end(),ref.nCard[0]) ;
		if ( iterA == vecHoldCard.end() )
		{
			continue;
		}

		iterA = std::find(vecHoldCard.begin(),vecHoldCard.end(),ref.nCard[1]) ;
		if ( iterA == vecHoldCard.end() )
		{
			continue;
		}

		vEatPairs.push_back(ref) ;
	}
	
	return vEatPairs.empty() == false ;
}

// act function 
void CMJPeerCardNew::onMoCard( uint8_t nCard )
{
	addNumberToVecWithAsc(vecHoldCard,nCard) ;
	m_isHuPaiInfoDirty = true ;
}

bool CMJPeerCardNew::onChuCard(uint8_t nCard )
{
	m_isHuPaiInfoDirty = true ;
	vecChuCard.push_back(nCard) ; // must not sort , it deponed on time , player chu  ;
	return removeNumberFromVec(vecHoldCard,nCard) ;
}

bool CMJPeerCardNew::onPeng( uint8_t nCard )
{
	assert(isCardCanPeng(nCard) && "you can not peng this card ");

	removeNumberFromVec(vecHoldCard,nCard) ;
	removeNumberFromVec(vecHoldCard,nCard) ;

	addNumberToVecWithAsc(vecPengedCard,nCard);
	m_isHuPaiInfoDirty = true ;
	return true ;
}

bool CMJPeerCardNew::onMingGang( uint8_t nCard , uint8_t nGangNewCard )
{
	assert(isCardCanMingGang(nCard) && "you can not ming gang this card ");
	removeNumberFromVec(vecHoldCard,nCard) ;
	removeNumberFromVec(vecHoldCard,nCard) ;
	removeNumberFromVec(vecHoldCard,nCard) ;

	addNumberToVecWithAsc(vecMingGangCard,nCard);

	addNumberToVecWithAsc(vecHoldCard,nGangNewCard) ;
	m_isHuPaiInfoDirty = true ;
	return true ;
}

bool CMJPeerCardNew::onBuGang( uint8_t nCard , uint8_t nGangNewCard )
{
	assert(isCardCanBuGang(nCard) && "you can not Bu gang this card ");

	removeNumberFromVec(vecPengedCard,nCard) ;
	addNumberToVecWithAsc(vecMingGangCard,nCard);
	addNumberToVecWithAsc(vecHoldCard,nGangNewCard) ;
	m_isHuPaiInfoDirty = true ;
	return true ;
}

bool CMJPeerCardNew::onAnGang( uint8_t nCard , uint8_t nGangNewCard )
{
	assert(isCardCanAnGang(nCard) && "you can not An gang this card ");
	uint8_t n = 4 ;
	while ( n-- )
	{
		auto b = removeNumberFromVec(vecHoldCard,nCard);
		assert( b && "you do not have an gang with this card");
		if ( !b )
		{
			return false ;
		}
	}

	addNumberToVecWithAsc(vecAnGangCard,nCard);

	addNumberToVecWithAsc(vecHoldCard,nGangNewCard) ;
	m_isHuPaiInfoDirty = true ;
	return true ;
}

bool CMJPeerCardNew::onEat(uint8_t nCard , stEatPair& refWithPair )
{
	auto iter0 = std::find(vecHoldCard.begin(),vecHoldCard.end(),refWithPair.nCard[0]) ;
	auto iter1 = std::find(vecHoldCard.begin(),vecHoldCard.end(),refWithPair.nCard[1]) ;
	if ( iter1 == vecHoldCard.end() || iter0 == vecHoldCard.end() )
	{
		assert(0 && "you can not eat this card" );
		return false ;
	}
	vecHoldCard.erase(iter0) ;
	iter1 = std::find(vecHoldCard.begin(),vecHoldCard.end(),refWithPair.nCard[1]) ;
	vecHoldCard.erase(iter1) ;

	addNumberToVecWithAsc(vecEatedCard,nCard);
	addNumberToVecWithAsc(vecEatedCard,refWithPair.nCard[0]);
	addNumberToVecWithAsc(vecEatedCard,refWithPair.nCard[1]);
	m_isHuPaiInfoDirty = true ;
	return true ;
}

bool CMJPeerCardNew::onCardBeRobted( uint8_t nCard )
{
	auto iter = std::find(vecChuCard.begin(),vecChuCard.end(),nCard) ;
	if ( iter == vecChuCard.end() )
	{
		printf("you don't chu card = %u , how can be robot\n",nCard );
		return false ;
	}
	vecChuCard.erase(iter) ;
	return true ;
}

bool CMJPeerCardNew::onHu(uint8_t nCard,bool isSelf )
{
	if ( !this->m_isHuPaiInfoDirty && isSelf )
	{
		return ptrHupaiInfo->isHu();
	}

	if ( isSelf )
	{
		ptrHupaiInfo->parseHuPaiInfo(vecHoldCard) ;
	}
	else
	{
		addNumberToVecWithAsc(vecHoldCard,nCard) ;
		ptrHupaiInfo->parseHuPaiInfo(vecHoldCard) ;
		if ( ptrHupaiInfo->isHu() == false )
		{
			removeNumberFromVec(vecHoldCard,nCard) ;
		}
	}
	return ptrHupaiInfo->isHu();
}

//  get function 
void CMJPeerCardNew::getHoldCard(std::vector<uint8_t>& vCards )
{
	vCards.assign(vecHoldCard.begin(),vecHoldCard.end()) ;
}

void CMJPeerCardNew::getShowedCard(std::vector<uint8_t>& vCards )
{
	vCards.assign(vecEatedCard.begin(),vecEatedCard.end());
	// add peng 
	for ( auto& ref : vecPengedCard )
	{
		vCards.push_back(ref);
		vCards.push_back(ref);
		vCards.push_back(ref);
	}

	// add gang 
	for ( auto& ref : vecAnGangCard )
	{
		vCards.push_back(ref);
		vCards.push_back(ref);
		vCards.push_back(ref);
		vCards.push_back(ref);
	}

	// add gang 
	for ( auto& ref : vecMingGangCard )
	{
		vCards.push_back(ref);
		vCards.push_back(ref);
		vCards.push_back(ref);
		vCards.push_back(ref);
	}
}

void CMJPeerCardNew::getChuedCard(std::vector<uint8_t>& vCards )
{
	vCards.assign(vecChuCard.begin(),vecChuCard.end()) ;
}

void CMJPeerCardNew::getPengedCard(std::vector<uint8_t>& vCards )
{
	vCards.assign(vecPengedCard.begin(),vecPengedCard.end()) ;
}

void CMJPeerCardNew::getGangCard( std::vector<uint8_t>& vCards )
{
	std::vector<uint8_t> vecTemp ;
	getAnGang(vecTemp) ;
	getMingGang(vCards) ;
	vCards.insert(vCards.end(),vecTemp.begin(),vecTemp.end()) ;
	std::sort(vCards.begin(),vCards.end());
}

void CMJPeerCardNew::getMingGang( std::vector<uint8_t>& vCards )
{
	vCards.assign(vecMingGangCard.begin(),vecMingGangCard.end());
}

void CMJPeerCardNew::getAnGang(std::vector<uint8_t>& vCards)
{
	vCards.assign(vecAnGangCard.begin(),vecAnGangCard.end());
}

bool CMJPeerCardNew::addNumberToVecWithAsc(std::vector<uint8_t>& vecCards, uint8_t& nAddCard )
{
	auto iter = std::find_if(vecCards.begin(),vecCards.end(),[nAddCard]( uint8_t& ref ){ if ( ref >= nAddCard ) return true ; return false ; } );
	if ( iter != vecCards.end() )
	{
		vecCards.insert(iter,nAddCard) ;
	}
	else
	{
		vecCards.push_back(nAddCard) ;
	}

	return true ;
}

bool CMJPeerCardNew::removeNumberFromVec(std::vector<uint8_t>& vecCards , uint8_t& nCard )
{
	auto iter = std::find(vecCards.begin(),vecCards.end(),nCard) ;
	if ( iter != vecCards.end() )
	{
		vecCards.erase(iter) ;
		return true ;
	}
	return false ;
}

void CMJPeerCardNew::debugPeerCardInfo()
{
	printf("peer card info : \n");

	printf(" ÷≈∆£∫------------\n") ;
	debugVecCardInfo(vecHoldCard);

	printf("≈ˆ≈∆£∫----------------\n");
	debugVecCardInfo(vecPengedCard) ;

	printf("∏‹≈∆ ming£∫-------------\n") ;
	debugVecCardInfo(vecMingGangCard) ;

	printf("∏‹≈∆ An£∫-------------\n") ;
	debugVecCardInfo(vecAnGangCard) ;

	printf("≥‘≈∆£∫------------\n");
	debugVecCardInfo(vecEatedCard) ;

	printf("≥ˆ≈∆£∫------------------\n");
	debugVecCardInfo(vecChuCard);
	printf("peer card info end \n");
}

void CMJPeerCardNew::debugVecCardInfo(std::vector<uint8_t>&vecCards )
{
	for ( auto& ref : vecCards )
	{
		CMJCard::debugSinglCard(ref) ;
	}
}

bool CMJPeerCardNew::isHoldCardExist(uint8_t nCard )
{
	auto iter = std::find(vecHoldCard.begin(),vecHoldCard.end(),nCard) ;
	return iter != vecHoldCard.end() ;
}

