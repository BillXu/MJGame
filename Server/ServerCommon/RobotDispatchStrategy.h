#pragma once
#include "ISitableRoom.h"
#include <list>
#include "Timer.h"
#include "../MJServer/src/IMJRoom.h"
class CRobotDispatchStrategy
{
public:
	struct stJoinRobot
	{
		uint32_t nSessionID ;
		time_t tLeaveTime ;
	};
	typedef std::list<stJoinRobot*> LIST_JOIN_ROBOT ;
public:
	CRobotDispatchStrategy();
	~CRobotDispatchStrategy();
	bool init(ISitableRoom* pRoom , uint8_t nReqRobotLevel, uint32_t nRoomID , uint8_t nsubRoomIdx );
	bool init(IMJRoom* pRoom, uint8_t nReqRobotLevel, uint32_t nRoomID);
	void onPlayerJoin(uint32_t nSessionID, bool isRobot );
	void onPlayerLeave(uint32_t nSessioID,bool isRobot );
protected:
	bool interalInit();
	bool isTargetRoomHaveRealPlayer();
	bool isTargetRoomHaveEmptySeat();
	bool isTargetRoomWaitPlayerJoin();
	void sendMsgToPlayer(Json::Value& jsContent, unsigned short nMsgType, uint32_t nSessionID); 
	void sendMsgToPlayer(stMsg* pmsg, uint16_t nLen, uint32_t nSessionID);
protected:
	ISitableRoom* m_pRoom ;
	IMJRoom* m_pMJRoom;
	uint32_t m_nRoomID ;
	uint8_t m_nSubRoomIdx ;
	uint8_t m_nReqRobotLevel ; 
	uint8_t m_nRoomType;
	LIST_JOIN_ROBOT m_vPlayingRobot ;
	CTimer m_tReqRobotTimer ;
};