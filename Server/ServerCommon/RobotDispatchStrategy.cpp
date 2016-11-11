#include "RobotDispatchStrategy.h"
#include <ctime>
#include "log4z.h"
#include "ServerMessageDefine.h"
#include "IRoomState.h"
#include "json/json.h"
#include "../MJServer/src/IMJPlayer.h"
#include "../MJServer/src/IMJRoomState.h"
#ifdef _DEBUG
	#define TIME_ROBOT_STAY 3*60
	#define  TIME_UPDATE_DISPATCH_TICK 1*60
#else
#define TIME_ROBOT_STAY 30*60
#define  TIME_UPDATE_DISPATCH_TICK 5*60
#endif // DEBUG


CRobotDispatchStrategy::CRobotDispatchStrategy()
{
	m_pRoom = nullptr ;
	m_pMJRoom = nullptr;
	m_nReqRobotLevel = 0 ;
	m_nRoomID = 0 ;
	m_nSubRoomIdx = 0 ;
}

CRobotDispatchStrategy::~CRobotDispatchStrategy()
{
	for ( auto ref : m_vPlayingRobot )
	{
		delete ref ;
	}
	m_vPlayingRobot.clear() ;
	m_tReqRobotTimer.canncel();
}

bool CRobotDispatchStrategy::init(ISitableRoom* pRoom , uint8_t nReqRobotLevel, uint32_t nRoomID , uint8_t nsubRoomIdx )
{
	m_pRoom = pRoom ;
	m_nReqRobotLevel = nReqRobotLevel ;
	m_nRoomID = nRoomID ;
	m_nSubRoomIdx = nsubRoomIdx ;
	m_nRoomType = m_pRoom->getRoomType();
	interalInit();
	return true ;
}

bool CRobotDispatchStrategy::init(IMJRoom* pRoom, uint8_t nReqRobotLevel, uint32_t nRoomID)
{
	m_pMJRoom = pRoom;
	m_nReqRobotLevel = nReqRobotLevel;
	m_nRoomID = nRoomID;
	m_nSubRoomIdx = 0;
	m_nRoomType = m_pMJRoom->getRoomType();
	interalInit();
	return true;
}

bool CRobotDispatchStrategy::interalInit()
{
	m_tReqRobotTimer.setCallBack([this](CTimer* p, float f){
		if ( isTargetRoomWaitPlayerJoin() && isTargetRoomHaveRealPlayer() && isTargetRoomHaveEmptySeat())
		{
			stMsgRequestRobotToEnterRoom msgreq;
			msgreq.nReqRobotLevel = m_nReqRobotLevel;
			msgreq.nRoomID = m_nRoomID;
			msgreq.nRoomType = m_nRoomType;
			msgreq.nSubRoomIdx = m_nSubRoomIdx;
			sendMsgToPlayer(&msgreq, sizeof(msgreq), 0);
			//printf("room id = %u timer req robot join \n ",m_pRoom->getRoomID());
		}

		if (isTargetRoomWaitPlayerJoin() &&  (isTargetRoomHaveRealPlayer() == false ))
		{
			// all robot leave ;
			Json::Value jsmsg;
			for (auto proto : m_vPlayingRobot)
			{
				sendMsgToPlayer(jsmsg, MSG_SVR_INFORM_ROBOT_LEAVE, proto->nSessionID);
				delete proto;
				proto = nullptr;
			}
			m_vPlayingRobot.clear();
		}

	});

	m_tReqRobotTimer.setInterval(2);
	m_tReqRobotTimer.setIsAutoRepeat(true);
	m_tReqRobotTimer.start();
	return true;
}

void CRobotDispatchStrategy::onPlayerJoin(uint32_t nSessionID, bool isRobot )
{
	if ( isRobot )
	{
		stJoinRobot* pRet = new stJoinRobot ;
		pRet->nSessionID = nSessionID ;
		pRet->tLeaveTime = time(nullptr) + TIME_ROBOT_STAY ;
		m_vPlayingRobot.push_back(pRet) ;

		LOGFMTD("a robot session id = %u join room id = %u",nSessionID,m_nRoomID );
		return ;
	}

	if (  isTargetRoomHaveEmptySeat() )
	{
		stMsgRequestRobotToEnterRoom msgreq ;
		msgreq.nReqRobotLevel = m_nReqRobotLevel ;
		msgreq.nRoomID = m_nRoomID ;
		msgreq.nRoomType = m_nRoomType ;
		msgreq.nSubRoomIdx = m_nSubRoomIdx ;
		sendMsgToPlayer(&msgreq,sizeof(msgreq),0) ;

		LOGFMTD("real player session id = %u , join room id = %u, still have empty seat , just req robot ",nSessionID , m_nRoomID );
	}
}

void CRobotDispatchStrategy::onPlayerLeave(uint32_t nSessioID,bool isRobot )
{
	if ( isRobot )
	{
		auto iter = m_vPlayingRobot.begin() ;
		for ( ; iter != m_vPlayingRobot.end() ; ++iter )
		{
			if ( (*iter)->nSessionID == nSessioID )
			{
				delete (*iter) ;
				(*iter) = nullptr ;
				m_vPlayingRobot.erase(iter) ;
				return ;
			}
		}

		LOGFMTD( "a robot leave room session id = %u , room id = %u" ,nSessioID , m_nRoomID );
	}
	else
	{
		if ( isTargetRoomHaveRealPlayer() )
		{
	
		}
		else
		{
			LOGFMTD("a real player session = %u , leave room id = %u, and no real player in , so order all robot leave ",nSessioID,m_nRoomID );
			// all robot leave ;
			Json::Value jsmsg ;
			for ( auto proto : m_vPlayingRobot )
			{
				sendMsgToPlayer(jsmsg,MSG_SVR_INFORM_ROBOT_LEAVE,proto->nSessionID);
				delete proto ;
				proto = nullptr ;
			}
			m_vPlayingRobot.clear() ;

			stMsgRequestRobotCanncel msgreq ;
			msgreq.nRoomID = m_nRoomID ;
			msgreq.nRoomType = m_nRoomType ;
			msgreq.nSubRoomIdx = m_nSubRoomIdx ;
			sendMsgToPlayer(&msgreq,sizeof(msgreq),0) ;
			LOGFMTD("room id = %u , req canncel all robot req ",m_nRoomID) ;
		}
	}

}

bool CRobotDispatchStrategy::isTargetRoomHaveRealPlayer()
{
	if (m_pRoom)
	{
		return m_pRoom->isHaveRealPlayer();
	}

	if (!m_pMJRoom)
	{
		LOGFMTE("why both room is null ,robot dispatch !");
		return false;
	}
	// mj room 
	for (uint8_t nIdx = 0; nIdx < m_pMJRoom->getSeatCnt(); ++nIdx)
	{
		auto pp = m_pMJRoom->getMJPlayerByIdx(nIdx);
		if (pp && pp->isRobot() == false)
		{
			return true;
		}
	}
	return false;
}

bool CRobotDispatchStrategy::isTargetRoomHaveEmptySeat()
{
	if (m_pRoom)
	{
		return m_pRoom->getEmptySeatCount() > 0 ;
	}

	if (!m_pMJRoom)
	{
		LOGFMTE("why both room is null ,robot dispatch !");
		return false;
	}
	// mj room 
	return m_pMJRoom->isRoomFull() == false ;
}

bool CRobotDispatchStrategy::isTargetRoomWaitPlayerJoin()
{
	uint8_t nState = 0;
	if (m_pRoom)
	{
		return eRoomSate_WaitReady == m_pRoom->getCurRoomState()->getStateID();
	}
	
	if (!m_pMJRoom)
	{
		LOGFMTE("why both room is null ,robot dispatch !");
		return false;
	}

	return eRoomSate_WaitReady == m_pMJRoom->getCurRoomState()->getStateID();
}

void CRobotDispatchStrategy::sendMsgToPlayer(Json::Value& jsContent, unsigned short nMsgType, uint32_t nSessionID)
{
	if (m_pRoom)
	{
		m_pRoom->sendMsgToPlayer(jsContent, nMsgType, nSessionID);
	}

	if (!m_pMJRoom)
	{
		LOGFMTE("why both room is null ,robot dispatch ! can not send msg");
		return;
	}
	m_pMJRoom->sendMsgToPlayer(jsContent, nMsgType, nSessionID);
}

void CRobotDispatchStrategy::sendMsgToPlayer(stMsg* pmsg, uint16_t nLen, uint32_t nSessionID)
{
	if (m_pRoom)
	{
		m_pRoom->sendMsgToPlayer(pmsg, nLen, nSessionID);
	}

	if (!m_pMJRoom)
	{
		LOGFMTE("why both room is null ,robot dispatch ! can not send msg");
		return;
	}
	m_pMJRoom->sendMsgToPlayer(pmsg, nLen, nSessionID);
}
