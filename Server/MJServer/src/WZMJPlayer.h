#pragma once
#include "MJPlayer.h"
#include "WZMJPlayerCard.h"
class WZMJPlayer
	:public MJPlayer
{
public:
	void onWillStartGame()override;
	IMJPlayerCard* getPlayerCard()override;
	bool isDingDi();
	void doDingDi();
protected:
	bool m_isDingDi;
	WZMJPlayerCard m_tPlayerCard;
};