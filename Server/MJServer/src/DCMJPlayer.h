#pragma once
#include "MJPlayer.h"
#include "DCMJPlayerCard.h"
#include "json/json.h"
class DCMJPlayer
	:public MJPlayer
{
public:
	IMJPlayerCard* getPlayerCard()override;
	void init(stEnterRoomData* pData)override;
	void onWillStartGame()override;
	void roomInfoVisitor(Json::Value& jsInfo)override;
	void setBuyCode( uint8_t nCode );
	uint8_t getBuyCode();
protected:
	DCMJPlayerCard m_tPlayerCard;
	uint8_t m_nBuyCode;
};