#pragma once

#include "NetWorkManager.h"

#include "Timer.h"
#include "ServerConfig.h"
#include "ServerMessageIdentifer.h"
#include "json/json.h"
struct stMsg ;
class IGlobalModule ;
class IServerApp
	:CNetMessageDelegate
{
public:
	IServerApp();
	virtual ~IServerApp();
	virtual bool init();
	virtual bool OnMessage( Packet* pMsg ) ;
	virtual bool OnLostSever(Packet* pMsg);
	virtual bool OnConnectStateChanged( eConnectState eSate, Packet* pMsg);
	bool run();
	void shutDown();
	bool sendMsg( const char* pBuffer , int nLen );
	bool sendMsg( uint32_t nSessionID , const char* pBuffer , uint16_t nLen, bool bBroadcast = false );
	bool sendMsg( uint32_t nSessionID , Json::Value& recvValue, uint16_t nMsgID = 0 ,bool bBroadcast = false );
	void stop();
	virtual bool onLogicMsg( stMsg* prealMsg , eMsgPort eSenderPort , uint32_t nSessionID );
	virtual bool onLogicMsg( Json::Value& recvValue , uint16_t nmsgType, eMsgPort eSenderPort , uint32_t nSessionID );
	virtual void update(float fDeta );
	virtual uint16_t getLocalSvrMsgPortType() = 0 ; // et : ID_MSG_PORT_DATA , ID_MSG_PORT_TAXAS
	virtual uint16_t getTargetSvrPortType();
	bool isConnected();
	void setConnectServerConfig(stServerConfig* pConfig );
	CTimerManager* getTimerMgr(){ return m_pTimerMgr ; }
	virtual void onExit();
	virtual void onConnectedToSvr();
	void registerModule(IGlobalModule* pModule);
	IGlobalModule* getModuleByType(uint16_t nType );
protected:
	void doConnectToTargetSvr();
	uint16_t getVerifyType(); // et:MSG_VERIFY_DATA ,MSG_VERIFY_TAXAS,MSG_VERIFY_LOGIN
	CNetWorkMgr* getNetwork(){ return m_pNetWork ;}
private:
	bool m_bRunning;
	CONNECT_ID m_nTargetSvrNetworkID ;
	CNetWorkMgr::eConnectType m_eConnectState ;
	CTimerManager* m_pTimerMgr ;
	CNetWorkMgr* m_pNetWork ;

	stServerConfig m_stConnectConfig ;
	float m_fReconnectTick ;

	char m_pSendBuffer[MAX_MSG_BUFFER_LEN] ;

	std::map<uint16_t,IGlobalModule*> m_vAllModule ;
};