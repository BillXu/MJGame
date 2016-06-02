#pragma once
#include "ISitableRoom.h"
#include <list>
#include "Timer.h"
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
	void onPlayerJoin(uint32_t nSessionID, bool isRobot );
	void onPlayerLeave(uint32_t nSessioID,bool isRobot );
protected:

protected:
	ISitableRoom* m_pRoom ;
	uint32_t m_nRoomID ;
	uint8_t m_nSubRoomIdx ;
	uint8_t m_nReqRobotLevel ; 
	LIST_JOIN_ROBOT m_vPlayingRobot ;
	CTimer m_tReqRobotTimer ;
};