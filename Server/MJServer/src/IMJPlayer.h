#pragma once 
#include "NativeTypes.h"
class IMJPlayerCard;
class IMJPlayer
{
public:
	virtual ~IMJPlayer(){}
	virtual uint8_t getIdx() = 0;
	virtual void setIdx(uint8_t nIdx) = 0;
	virtual int32_t getCoin() = 0;
	virtual void addOffsetCoin( int32_t nOffset ) = 0 ;
	virtual IMJPlayerCard* getPlayerCard() = 0;
	virtual void setState( uint32_t eState );
	virtual void haveState( uint32_t eState );
	virtual uint32_t getSessionID() = 0;
	virtual uint32_t getUID() = 0;
	virtual void setUID( uint32_t nUID ) = 0 ;
	virtual void setSessionID( uint32_t nSessionID ) = 0 ;
};