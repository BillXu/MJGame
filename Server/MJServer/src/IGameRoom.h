#pragma once
#include "NativeTypes.h"
#include "json/json.h"
#include "MessageIdentifer.h"
class IRoomManager;
struct stBaseRoomConfig;
struct stMsg;
class IGameRoom
{
public:
	virtual ~IGameRoom(){}
	virtual bool init(IRoomManager* pRoomMgr, stBaseRoomConfig* pConfig, uint32_t nRoomID, Json::Value& vJsValue) = 0;
	virtual bool onPlayerEnter(stEnterRoomData* pEnterRoomPlayer) = 0;
	virtual bool onPlayerApplyLeave( uint32_t nPlayerUID ) = 0 ;
	virtual bool isRoomFull() = 0;

	virtual void roomItemDetailVisitor(Json::Value& vOutJsValue) = 0;
	virtual uint32_t getRoomID() = 0;
	virtual uint8_t getRoomType() = 0;
	virtual void update(float fDelta) = 0;
	virtual bool onMessage(stMsg* prealMsg, eMsgPort eSenderPort, uint32_t nPlayerSessionID) = 0;
	virtual bool onMsg(Json::Value& prealMsg, uint16_t nMsgType, eMsgPort eSenderPort, uint32_t nSessionID) = 0;
	virtual stBaseRoomConfig* getRoomConfig() = 0;
};