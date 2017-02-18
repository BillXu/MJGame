#include "DCMJPlayer.h"
IMJPlayerCard* DCMJPlayer::getPlayerCard()
{
	return &m_tPlayerCard;
}

void DCMJPlayer::init(stEnterRoomData* pData)
{
	MJPlayer::init(pData);
	m_nBuyCode = 0;
}

void DCMJPlayer::onWillStartGame()
{
	MJPlayer::onWillStartGame();
	m_nBuyCode = 0;
}

void DCMJPlayer::roomInfoVisitor(Json::Value& jsInfo)
{
	MJPlayer::roomInfoVisitor(jsInfo);
	jsInfo["buyCode"] = getBuyCode();
}

void DCMJPlayer::setBuyCode(uint8_t nCode)
{
	m_nBuyCode = nCode;
}

uint8_t DCMJPlayer::getBuyCode()
{
	return m_nBuyCode;
}