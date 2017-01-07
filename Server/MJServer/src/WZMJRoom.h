#pragma once 
#include "IMJRoom.h"
#include "MJCard.h"
class WZMJRoom
	:public IMJRoom
{
public:
	bool init(IGameRoomManager* pRoomMgr, stBaseRoomConfig* pConfig, uint32_t nRoomID, Json::Value& vJsValue) override;
	void willStartGame() override;
	void startGame()override;
	void onGameDidEnd()override;
	void onGameEnd()override;
	uint8_t getAutoChuCardWhenWaitActTimeout(uint8_t nIdx)override;
	uint8_t getAutoChuCardWhenWaitChuTimeout(uint8_t nIdx)override;
	bool isGameOver()override;
	bool isCanGoOnMoPai()override;
	bool onPlayerApplyLeave(uint32_t nPlayerUID)override;
	IMJPlayer* doCreateMJPlayer()override;
	void sendRoomInfo(uint32_t nSessionID)override;
	IMJPoker* getMJPoker(){ return &m_tPoker; }
	uint8_t getRoomType()override{ return eMJ_WZ; }
	bool canPlayerChuCard(uint8_t nIdx , uint8_t nCheckCard );
	void onPlayerChu(uint8_t nIdx, uint8_t nCard)override;
	void onPlayerHu(std::vector<uint8_t>& vHuIdx, uint8_t nCard, uint8_t nInvokeIdx)override;
protected:
	uint8_t m_nCaiShenDice;
	uint8_t m_nCaiShenCard;
	uint8_t m_nGangedCnt;
	CMJCard m_tPoker;
	uint8_t m_nLastHuPlayerIdx;
	uint8_t m_nLastChuCard;
	uint8_t m_nLianZhuangCnt; 
};