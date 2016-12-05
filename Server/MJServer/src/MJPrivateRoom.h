#pragma once 
#include "Timer.h"
#include "RoomConfig.h"
#include "IMJRoom.h"
class MJPrivateRoom
	:public IGameRoom
	,public IMJRoomDelegate
{
public:
	struct stVipPlayer
	{
		uint32_t nSessionID;
		uint32_t nUID;
		uint32_t nRealCoin;
		uint32_t nRoomCoin;
	};
	typedef std::map<uint32_t, stVipPlayer> MAP_UID_VIP_PLAYERS;
public:
	~MJPrivateRoom();
	bool init(IGameRoomManager* pRoomMgr, stBaseRoomConfig* pConfig, uint32_t nRoomID, Json::Value& vJsValue)override;
	bool onPlayerEnter(stEnterRoomData* pEnterRoomPlayer)override;
	uint8_t checkPlayerCanEnter(stEnterRoomData* pEnterRoomPlayer)override;
	bool onPlayerApplyLeave(uint32_t nPlayerUID)override;
	bool isRoomFull()override;

	//virtual void roomItemDetailVisitor(Json::Value& vOutJsValue) = 0;
	uint32_t getRoomID()override;
	uint8_t getRoomType()override;
	void update(float fDelta)override;
	bool onMessage(stMsg* prealMsg, eMsgPort eSenderPort, uint32_t nPlayerSessionID)override;
	bool onMsg(Json::Value& prealMsg, uint16_t nMsgType, eMsgPort eSenderPort, uint32_t nSessionID) override;
	stBaseRoomConfig* getRoomConfig()override;
	void sendRoomInfo(uint32_t nSessionID)override;
	uint32_t getCoinNeedToSitDown()override;
	IGameRoom* doCreateMJRoom( eRoomType eMJType );

	void sendRoomMsg(Json::Value& prealMsg, uint16_t nMsgType)final;
	void sendMsgToPlayer(Json::Value& prealMsg, uint16_t nMsgType, uint32_t nSessionID)final;

	void onCheckDismissReply(bool bTimerOut);

	void onDidGameOver(IMJRoom* pRoom);
	void onRoomGameOver(bool isDismissed);
protected:
	IGameRoomManager* m_pRoomMgr;
	uint32_t m_nOwnerUID;

	uint8_t m_nLeftCircle;
	uint8_t m_nInitCircle;
	uint32_t m_nInitCoin;
	bool m_bComsumedRoomCards;
	MAP_UID_VIP_PLAYERS m_vAllPlayers;

	std::map<uint32_t, uint8_t> m_mapRecievedReply; // key : uid , value: replayer , 0  agree , 1 disagree ;
	bool m_bWaitDismissReply;
	CTimer m_tWaitRepklyTimer;
	time_t m_tInvokerTime;
	uint32_t m_nInvokerDismissUID;
	bool m_bDoDismissRoom;

	eRoomState m_eState;
	IGameRoom* m_pRoom;

	stMJRoomConfig m_stConfig;

	bool m_bRoomInfoDiry;
};