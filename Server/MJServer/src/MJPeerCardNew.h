#pragma once
#include "MJDefine.h"
#include "NativeTypes.h"
#include <list>
#include <map>
#include "json/json.h"
#include <vector>
class CMJPeerCardNew
{
public:
	struct stEatPair
	{
		uint8_t nCard[2] ;
	};
	typedef std::vector<stEatPair> VEC_EAT_PAIR ;
public:
	CMJPeerCardNew(){ reset(); }
	void reset();
	bool init();
	void addHoldCard(uint8_t nCard );
	
	// check function 
	bool isCardCanPeng( uint8_t nCard );
	bool isCardCanMingGang( uint8_t nCard );
	bool isCardCanBuGang( uint8_t nCard );
	bool isCardCanAnGang(uint8_t nCard );
	bool isHaveAnGangCards( std::vector<uint8_t>& vAnGangCards );
	bool isCardCanHu( uint8_t nCard );
	bool isCardCanEat( uint8_t nCard, VEC_EAT_PAIR& vEatPairs );

	// act function 
	void onMoCard( uint8_t nCard );
	bool onChuCard(uint8_t nCard );
	bool onPeng( uint8_t nCard );
	bool onMingGang( uint8_t nCard , uint8_t nGangNewCard );
	bool onBuGang( uint8_t nCard , uint8_t nGangNewCard );
	bool onAnGang( uint8_t nCard , uint8_t nGangNewCard );
	bool onEat(uint8_t nCard , stEatPair& refWithPair );
	bool onCardBeRobted( uint8_t nCard );
	bool onHu(uint8_t nCard,uint8_t& eFanType , uint16_t& nFanShu );  // total card = 14  means self hu , else not self hu ;

	//  get function 
	void debugPeerCardInfo();
	uint8_t getFirstHoldCard(){ return vecHoldCard.front() ; }
	bool isHoldCardExist(uint8_t nCard );
protected:
	void debugVecCardInfo(std::vector<uint8_t>&vecCards );
	bool addNumberToVecWithAsc(std::vector<uint8_t>& vecCards, uint8_t& nAddCard );
	bool removeNumberFromVec(std::vector<uint8_t>& vecCards , uint8_t& nCard );
protected:
	std::vector<uint8_t> vecHoldCard ;
	std::vector<uint8_t> vecPengedCard ;
	std::vector<uint8_t> vecGangCard ;
	std::vector<uint8_t> vecEatedCard ;
	std::vector<uint8_t> vecChuCard ;
};