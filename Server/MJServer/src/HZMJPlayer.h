#pragma once 
#include "MJPlayer.h"
class HZMJPlayer
	:public MJPlayer
{
public:
	void onWillStartGame()override;
	void onStartGame()override;
	void onGameDidEnd()override;
	void onGameEnd()override;
	void increasePiaoTimes(bool isGangPiao );
	void clearPiaoTimes();
	uint8_t getPiaoTimes();
	bool haveGangPiao();
protected:
	uint8_t m_nPiaoTimes;
	bool m_bHaveGangPiao;
};