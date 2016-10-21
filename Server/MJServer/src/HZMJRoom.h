#pragma once 
#include "IMJRoom.h"
#include "MJCard.h"
class HZMJRoom
	:public IMJRoom
{
public:
	bool init(IRoomManager* pRoomMgr, stBaseRoomConfig* pConfig, uint32_t nRoomID, Json::Value& vJsValue) override;
	void startGame()override;
	void onGameEnd()override;
	void willStartGame()override;
	void onGameDidEnd()override;
	bool isAnyPlayerPengOrHuThisCard(uint8_t nInvokeIdx, uint8_t nCard)override;;
	bool isAnyPlayerRobotGang(uint8_t nInvokeIdx, uint8_t nCard)override;;
	bool isGameOver()override;;
	bool isCanGoOnMoPai()override;
	IMJPlayer* doCreateMJPlayer()override;
	uint8_t distributeOneCard()override;
	void onPlayerChu(uint8_t nIdx, uint8_t nCard)override;
	void onPlayerHu(std::vector<uint8_t>& vHuIdx, uint8_t nCard, uint8_t nInvokeIdx)override;
protected:
	uint8_t m_nContinueBankes;
	bool m_vCaiPiaoFlag[MAX_SEAT_CNT];
	CMJCard m_tPoker;
};