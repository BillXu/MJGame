#include "GameServerApp.h"
#include "PlayerManager.h"
#include <ctime>
#include "PlayerMail.h"
#include "EventCenter.h"
#include "log4z.h"
#include "RewardConfig.h"
#include "ServerStringTable.h"
//#include "ExchangeCenter.h"
#include "RobotCenter.h"
#ifndef USHORT_MAX
#define USHORT_MAX 65535 
#endif
#include <cassert>
#include "log4z.h"
CGameServerApp* CGameServerApp::s_GameServerApp = NULL ;
CGameServerApp* CGameServerApp::SharedGameServerApp()
{
	if ( s_GameServerApp == nullptr )
	{
		s_GameServerApp = new CGameServerApp ;
	}
	return s_GameServerApp ;
}

CGameServerApp::~CGameServerApp()
{
	delete m_pConfigManager ;
}

bool CGameServerApp::init()
{
	IServerApp::init();
	//if ( s_GameServerApp == NULL )
	//{
	//	s_GameServerApp = this ;
	//}
	//else
	//{
	//	LOGFMTE("Game Server App can not be init more than once !") ;
	//	return false;
	//}
	srand((unsigned int)time(0));

	CSeverConfigMgr SvrConfigMgr ;
	SvrConfigMgr.LoadFile("../configFile/serverConfig.txt");
	stServerConfig* pConfig = SvrConfigMgr.GetServerConfig(eSvrType_Center) ;
	if ( pConfig == NULL )
	{
		LOGFMTE("center svr config is null , so can not connected to !") ;
		return false;
	}
	setConnectServerConfig(pConfig);

	CServerStringTable::getInstance()->LoadFile("../configFile/stringTable.txt");
	CRewardConfig::getInstance()->LoadFile("../configFile/rewardConfig.txt");

	m_pConfigManager = new CConfigManager() ;
	m_pConfigManager->LoadAllConfigFile("../configFile/") ;
	// init component ;

	//auto pExc = new CExchangeCenter("../configFile/exchange.txt");
	//registerModule(pExc);

	//auto robotC = new CRobotCenter ;
	//registerModule(robotC) ;

	// install module
	for (uint16_t nModule = eMod_None; nModule < eMod_Max; ++nModule)
	{
		auto b = installModule(nModule);
		assert(b && "install this module failed ");
		if (!b)
		{
			LOGFMTE("install module = %u failed", nModule);
		}
	}
	
	time_t tNow = time(NULL) ;
	m_nCurDay = localtime(&tNow)->tm_mday ;
	return true ;
}

CPlayerManager* CGameServerApp::GetPlayerMgr()
{
	return (CPlayerManager*)getModuleByType(eMod_PlayerMgr);
}

CRobotCenter* CGameServerApp::getRobotCenter()
{
	return (CRobotCenter*)getModuleByType(eMod_RobotCenter);
}

bool CGameServerApp::onLogicMsg( stMsg* prealMsg , eMsgPort eSenderPort , uint32_t nSessionID )
{
	if ( IServerApp::onLogicMsg(prealMsg,eSenderPort,nSessionID) )
	{
		return true ;
	}

	if ( m_tPokerCircle.onMessage(prealMsg,eSenderPort,nSessionID) )
	{
		return true ;
	}

	if ( ProcessPublicMsg(prealMsg,eSenderPort,nSessionID ) )
	{
		return true ;
	}

	LOGFMTE("unprocess msg = %d , from port = %d , nsssionid = %d",prealMsg->usMsgType,eSenderPort,nSessionID ) ;
	return false ;
}

bool CGameServerApp::onLogicMsg( Json::Value& recvValue , uint16_t nmsgType, eMsgPort eSenderPort , uint32_t nSessionID )
{
	if ( IServerApp::onLogicMsg(recvValue,nmsgType,eSenderPort,nSessionID) )
	{
		return true ;
	}

	LOGFMTE("unprocess msg = %d , from port = %d , nsssionid = %d",nmsgType,eSenderPort,nSessionID ) ;
	return false ;
}

//bool CGameServerApp::onAsyncRequest(uint16_t nRequestType , const Json::Value& jsReqContent, Json::Value& jsResult )
//{
//	
//	return false ;
//}

bool CGameServerApp::ProcessPublicMsg( stMsg* prealMsg , eMsgPort eSenderPort , uint32_t nSessionID )
{
	return false ;
}


void CGameServerApp::update(float fdeta )
{
	IServerApp::update(fdeta);
	CheckNewDay();
}

void CGameServerApp::CheckNewDay()
{
	// check new day 
	time_t tNow = time(NULL) ;
	struct tm tmNow = *localtime(&tNow) ;
	if ( tmNow.tm_mday != m_nCurDay )
	{
		m_nCurDay = tmNow.tm_mday ;
		// new day 
		CEventCenter::SharedEventCenter()->PostEvent(eEvent_NewDay,&tmNow) ;
	}
}

void CGameServerApp::onConnectedToSvr()
{
	IServerApp::onConnectedToSvr() ;
	m_tPokerCircle.readTopics();
}

void CGameServerApp::onExit()
{
	IServerApp::onExit() ;
}

IGlobalModule* CGameServerApp::createModule(uint16_t eModuleType)
{
	IGlobalModule* pMod = IServerApp::createModule(eModuleType);
	if (pMod)
	{
		return pMod;
	}

	switch (eModuleType)
	{
	case eMod_PlayerMgr:
	{
		pMod = new CPlayerManager();
	}
	break;
	case eMod_RobotCenter:
	{
		pMod = new CRobotCenter();
	}
	break;
	/*case eMod_EncryptNumber:
	{
		pMod = new CEncryptNumber();
	}
	break;
	case eMod_Group:
	{
		pMod = new CGroup();
	}
	break;
	case eMod_GameRoomCenter:
	{
		pMod = new CGameRoomCenter();
	}
	break;
	case eMod_QinJia:
	{
		pMod = new CQinJiaModule();
	}
	break;*/
	default:
		break;
	}

	return pMod;
}