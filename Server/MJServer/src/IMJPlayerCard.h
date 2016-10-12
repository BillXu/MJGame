#pragma once 
#include "NativeTypes.h"
#include <vector>
class IMJPlayerCard
{
public:
	typedef std::vector<uint8_t> VEC_CARD;
public:
	virtual ~IMJPlayerCard(){}
	virtual void reset() = 0;
	virtual void addDistributeCard( uint8_t nCardNum ) = 0 ;
	virtual void addMoCard( uint8_t nMoCard ) = 0;
	virtual void addGangCard( uint8_t nGangCard ) = 0 ;
	virtual bool chuCard( uint8_t nChuCard ) = 0;
	virtual bool eatCard( uint8_t nTarget , uint8_t nWithCardA, uint8_t nWithCardB) = 0;
	virtual bool gangCardBeRobot(uint8_t nCard ) = 0;
	virtual bool onCardBeGangPeng( uint8_t nCard ) = 0 ;

	virtual bool isHaveCard(uint8_t nCard) = 0 ;
	virtual bool canMingGangWithCard(uint8_t nCard) = 0 ;
	virtual bool canAnGangWithCard(uint8_t nCard) = 0;
	virtual bool canBuGangWithCard(uint8_t nCard) = 0;
	virtual bool canPengWithCard(uint8_t nCard) = 0;
	virtual bool canEatCard(uint8_t nCard, uint8_t& nWithA, uint8_t& withB ) = 0 ;
	virtual bool canHuWitCard( uint8_t nCard ) = 0;
	virtual bool isTingPai() = 0 ;
	virtual bool getHoldCardThatCanGang( VEC_CARD& vGangCards ) = 0 ;
	virtual bool isHoldCardCanHu() = 0 ;

	virtual bool getHoldCard(VEC_CARD& vHoldCard ) = 0 ;
	virtual bool getChuedCard(VEC_CARD& vChuedCard ) = 0 ;
	virtual bool getGangedCard(VEC_CARD& vGangCard) = 0 ;
	virtual bool getPengedCard(VEC_CARD& vPengedCard ) = 0 ;
	virtual bool getEatedCard(VEC_CARD& vEatedCard ) = 0 ;
	virtual uint32_t getNewestFetchedCard() = 0 ;
};