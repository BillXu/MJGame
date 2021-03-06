#pragma once
#include "NativeTypes.h"
class IMJPoker
{
public:
	virtual ~IMJPoker(){}

	virtual void shuffle(bool bMake = false) = 0;
	virtual uint8_t getLeftCardCount() = 0;
	virtual uint8_t distributeOneCard() = 0;
};