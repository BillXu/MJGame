#pragma once 
#include "NativeTypes.h"
class IMJPlayerCard;
struct stEnterRoomData;
class IMJPlayer
{
public:
	virtual ~IMJPlayer(){}
	virtual void init(stEnterRoomData* pData);
	virtual uint8_t getIdx() = 0;
	virtual void setIdx(uint8_t nIdx) = 0;
	virtual int32_t getCoin() = 0;
	virtual void addOffsetCoin( int32_t nOffset ) = 0 ;
	virtual IMJPlayerCard* getPlayerCard() = 0;
	virtual void setState( uint32_t eState );
	virtual bool haveState( uint32_t eState );
	virtual uint32_t getState() = 0;
	virtual uint32_t getSessionID() = 0;
	virtual uint32_t getUID() = 0;
};