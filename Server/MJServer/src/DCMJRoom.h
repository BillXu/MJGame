#pragma once 
#include "IMJRoom.h"
#include "MJCard.h"
class DCMJRoom
	:public IMJRoom
{
public:
	bool init(IGameRoomManager* pRoomMgr, stBaseRoomConfig* pConfig, uint32_t nRoomID, Json::Value& vJsValue) override;
	void willStartGame() override;
	void startGame()override;
	void onGameDidEnd()override;
	void onGameEnd()override;
	bool isGameOver()override;
	bool onPlayerApplyLeave(uint32_t nPlayerUID)override;
	IMJPlayer* doCreateMJPlayer()override;
	IMJPoker* getMJPoker(){ return &m_tPoker; }
	uint8_t getRoomType()override{ return eMJ_DC; }
	void onPlayerHu(std::vector<uint8_t>& vHuIdx, uint8_t nCard, uint8_t nInvokeIdx)override;
	bool isEnableBuyCode();
protected:
	CMJCard m_tPoker;
	uint8_t m_nHuPlayerIdx;
	bool m_isEnableYaoHu;
	bool m_isEnableBuyCode;
};