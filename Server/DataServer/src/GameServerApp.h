#pragma once
#include "ISeverApp.h"
#include "PlayerManager.h"
#include "ConfigManager.h"
#include "ServerConfig.h"
#include "PokerCircle.h"
class CBrocaster ;
class CRobotCenter;
class CGameServerApp
	:public IServerApp
{
public:
	enum eInstallModule
	{
		eMod_None = IServerApp::eDefMod_ChildDef,
		eMod_RobotCenter = eMod_None,
		//eMod_EncryptNumber,
		//eMod_Group,
		//eMod_GameRoomCenter,
		//eMod_QinJia,
		eMod_PlayerMgr,
		eMod_Exchange,
		eMod_Max,
	};
public:
	static CGameServerApp* SharedGameServerApp();
private:
	CGameServerApp(){}
public:
	~CGameServerApp();
	bool init()override;
	CPlayerManager* GetPlayerMgr();
	CRobotCenter* getRobotCenter();
	CConfigManager* GetConfigMgr(){ return m_pConfigManager ; }
	bool onLogicMsg(stMsg* prealMsg , eMsgPort eSenderPort , uint32_t nSessionID);
	bool onLogicMsg( Json::Value& recvValue , uint16_t nmsgType, eMsgPort eSenderPort , uint32_t nSessionID )override;
	//bool onAsyncRequest(uint16_t nRequestType , const Json::Value& jsReqContent, Json::Value& jsResult )override ;
	void update(float fdeta );
	uint16_t getLocalSvrMsgPortType(){ return ID_MSG_PORT_DATA ; } ; // et : ID_MSG_PORT_DATA , ID_MSG_PORT_TAXAS
	void onConnectedToSvr()override;
	void onExit()override;
protected:
	IGlobalModule* createModule(uint16_t eModuleType);
	bool ProcessPublicMsg( stMsg* prealMsg , eMsgPort eSenderPort , uint32_t nSessionID );
	void CheckNewDay();
public:
	static CGameServerApp* s_GameServerApp ;
protected:
	CConfigManager* m_pConfigManager ;
	CPokerCircle m_tPokerCircle ;
	// check NewDay ;
	unsigned int m_nCurDay ;
};