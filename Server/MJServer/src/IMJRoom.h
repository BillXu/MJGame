#pragma once
#include "IGameRoom.h"
class IMJPlayer;
struct stEnterRoomData;
class IMJRoomState;
class IMJRoom
	:public IGameRoom
{
public:
	typedef std::map<uint16_t, IMJRoomState*>	MAP_ID_ROOM_STATE;
public:
	~IMJRoom();
	bool init(IRoomManager* pRoomMgr, stBaseRoomConfig* pConfig, uint32_t nRoomID, Json::Value& vJsValue) override;
	bool onPlayerEnter(stEnterRoomData* pEnterRoomPlayer)override;
	bool onPlayerApplyLeave(uint32_t nPlayerUID)override;
	bool isRoomFull()override;

	void roomItemDetailVisitor(Json::Value& vOutJsValue) override;
	uint32_t getRoomID()override;
	uint8_t getRoomType() override;
	void update(float fDelta) override;
	bool onMessage(stMsg* prealMsg, eMsgPort eSenderPort, uint32_t nPlayerSessionID)override;
	bool onMsg(Json::Value& prealMsg, uint16_t nMsgType, eMsgPort eSenderPort, uint32_t nSessionID) override;

	void sendRoomMsg(Json::Value& prealMsg, uint16_t nMsgType );
	void sendMsgToPlayer(Json::Value& prealMsg, uint16_t nMsgType, uint32_t nSessionID );
	bool sitdown(IMJPlayer* pPlayer , uint8_t nIdx );
	bool standup( uint32_t nUID );
	IMJPlayer* getMJPlayerBySessionID(uint32_t nSessionid );
	IMJPlayer* getMJPlayerByUID( uint32_t nUID );

	virtual void startGame() { }
	virtual void willStartGame() { }
	virtual void gameEnd(){}
	virtual void onGameDidEnd(){}
	virtual bool canStartGame();
	virtual float getStateDuringForState( uint32_t nState );

	void goToState(IMJRoomState* pTargetState, Json::Value* jsValue = nullptr);
	void goToState(uint16_t nStateID, Json::Value* jsValue = nullptr);
	uint8_t getBankerIdx();
	// mj function ;
	void onWaitPlayerAct( uint8_t nIdx , bool& isCanPass );
	uint8_t getAutoChuCardWhenWaitActTimeout(uint8_t nIdx);
protected:
	bool addRoomState(IMJRoomState* pState);
protected:
	IMJPlayer* m_vMJPlayers[4];
	MAP_ID_ROOM_STATE m_vRoomStates;
	IMJRoomState* m_pCurState;
};