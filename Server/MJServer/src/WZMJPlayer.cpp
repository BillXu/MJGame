#include "WZMJPlayer.h"
void WZMJPlayer::onWillStartGame()
{
	MJPlayer::onWillStartGame();
	m_isDingDi = false;
}

IMJPlayerCard* WZMJPlayer::getPlayerCard()
{
	return &m_tPlayerCard;
}

bool WZMJPlayer::isDingDi()
{
	return m_isDingDi;
}

void WZMJPlayer::doDingDi()
{
	m_isDingDi = true;
}