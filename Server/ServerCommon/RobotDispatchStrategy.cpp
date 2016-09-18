#include "RobotDispatchStrategy.h"
#include <ctime>
#include "LogManager.h"
#include "ServerMessageDefine.h"
#include "IRoomState.h"
#include "json/json.h"
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
}

bool CRobotDispatchStrategy::init(ISitableRoom* pRoom , uint8_t nReqRobotLevel, uint32_t nRoomID , uint8_t nsubRoomIdx )
{
	m_pRoom = pRoom ;
	m_nReqRobotLevel = nReqRobotLevel ;
	m_nRoomID = nRoomID ;
	m_nSubRoomIdx = nsubRoomIdx ;

	m_tReqRobotTimer.setCallBack([this](CTimer* p , float f ){ 

	if ( m_pRoom->isHaveRealPlayer() && m_pRoom->getEmptySeatCount() > 0 )
	{
		stMsgRequestRobotToEnterRoom msgreq ;
		msgreq.nReqRobotLevel = m_nReqRobotLevel ;
		msgreq.nRoomID = m_nRoomID ;
		msgreq.nRoomType = m_pRoom->getRoomType() ;
		msgreq.nSubRoomIdx = m_nSubRoomIdx ;
		m_pRoom->sendMsgToPlayer(&msgreq,sizeof(msgreq),0) ;
		//printf("room id = %u timer req robot join \n ",m_pRoom->getRoomID());
	}

	if ( m_pRoom->isHaveRealPlayer() == false )
	{
		// all robot leave ;
			Json::Value jsmsg ;
			for ( auto proto : m_vPlayingRobot )
			{
				m_pRoom->sendMsgToPlayer(jsmsg,MSG_SVR_INFORM_ROBOT_LEAVE,proto->nSessionID);
				delete proto ;
				proto = nullptr ;
			}
			m_vPlayingRobot.clear() ;
	}

	} ) ;

	m_tReqRobotTimer.setInterval(2) ;
	m_tReqRobotTimer.setIsAutoRepeat(true) ;
	m_tReqRobotTimer.start() ;
	return true ;
}

void CRobotDispatchStrategy::onPlayerJoin(uint32_t nSessionID, bool isRobot )
{
	if ( isRobot )
	{
		stJoinRobot* pRet = new stJoinRobot ;
		pRet->nSessionID = nSessionID ;
		pRet->tLeaveTime = time(nullptr) + TIME_ROBOT_STAY ;
		m_vPlayingRobot.push_back(pRet) ;

		CLogMgr::SharedLogMgr()->PrintLog("a robot session id = %u join room id = %u",nSessionID,m_pRoom->getRoomID() );
		return ;
	}

	if (  m_pRoom->getEmptySeatCount() > 0 )
	{
		stMsgRequestRobotToEnterRoom msgreq ;
		msgreq.nReqRobotLevel = m_nReqRobotLevel ;
		msgreq.nRoomID = m_nRoomID ;
		msgreq.nRoomType = m_pRoom->getRoomType() ;
		msgreq.nSubRoomIdx = m_nSubRoomIdx ;
		m_pRoom->sendMsgToPlayer(&msgreq,sizeof(msgreq),0) ;

		CLogMgr::SharedLogMgr()->PrintLog("real player session id = %u , join room id = %u, still have empty seat , just req robot ",nSessionID , m_pRoom->getRoomID() );
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

		CLogMgr::SharedLogMgr()->PrintLog( "a robot leave room session id = %u , room id = %u" ,nSessioID , m_pRoom->getRoomID() );
	}
	else
	{
		if ( m_pRoom->isHaveRealPlayer() )
		{
	
		}
		else
		{
			CLogMgr::SharedLogMgr()->PrintLog("a real player session = %u , leave room id = %u, and no real player in , so order all robot leave ",nSessioID,m_pRoom->getRoomID() );
			// all robot leave ;
			Json::Value jsmsg ;
			for ( auto proto : m_vPlayingRobot )
			{
				m_pRoom->sendMsgToPlayer(jsmsg,MSG_SVR_INFORM_ROBOT_LEAVE,proto->nSessionID);
				delete proto ;
				proto = nullptr ;
			}
			m_vPlayingRobot.clear() ;

			stMsgRequestRobotCanncel msgreq ;
			msgreq.nRoomID = m_nRoomID ;
			msgreq.nRoomType = m_pRoom->getRoomType() ;
			msgreq.nSubRoomIdx = m_nSubRoomIdx ;
			m_pRoom->sendMsgToPlayer(&msgreq,sizeof(msgreq),0) ;
			CLogMgr::SharedLogMgr()->PrintLog("room id = %u , req canncel all robot req ",m_pRoom->getRoomID()) ;
		}
	}

}