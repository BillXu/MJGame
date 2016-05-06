#include <windows.h>
#include "ISeverApp.h"
#include "LogManager.h"
#include "MessageDefine.h"
#include "ServerMessageDefine.h"
#include <time.h>
#include <assert.h>
#include <synchapi.h>
#include "IGlobalModule.h"
#include "AutoBuffer.h"

#define TIME_WAIT_FOR_RECONNECT 5
#pragma comment(lib,"JsonDll.lib")
bool IServerApp::init()
{
	m_bRunning = true;
	m_nTargetSvrNetworkID = INVALID_CONNECT_ID;
	
	m_pNetWork = new CNetWorkMgr ;
	m_pNetWork->SetupNetwork(1);
	m_pNetWork->AddMessageDelegate(this);

	m_pTimerMgr = new CTimerManager ;

	m_fReconnectTick = 0 ;

	return true ;
}

IServerApp::IServerApp()
{
	m_pTimerMgr = nullptr ;
	m_pNetWork = nullptr ;
	m_eConnectState = CNetWorkMgr::eConnectType_None ;
	memset(&m_stConnectConfig,0,sizeof(m_stConnectConfig));
	m_fReconnectTick = 0 ;
	m_vAllModule.clear() ;
}

IServerApp::~IServerApp()
{
	for ( auto pp : m_vAllModule )
	{
		delete pp.second ;
		pp.second = nullptr ;
	}

	if ( m_pTimerMgr )
	{
		delete m_pTimerMgr ;
		m_pTimerMgr = nullptr ;
	}

	if ( m_pNetWork )
	{
		m_pNetWork->ShutDown() ;
		delete m_pNetWork ;
		m_pNetWork = nullptr ;
	}
}

bool IServerApp::OnMessage( Packet* pMsg )
{
	CHECK_MSG_SIZE(stMsg,pMsg->_len) ;
	stMsg* pmsg = (stMsg*)pMsg->_orgdata ;
	if ( pmsg->cSysIdentifer == ID_MSG_VERIFY )
	{
		CLogMgr::SharedLogMgr()->SystemLog("no need recieve verify msg") ;
		return true ;
	}

	stMsg* pRet = pmsg;
	if ( pRet->usMsgType != MSG_TRANSER_DATA )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("why msg type is not transfer data , type = %d",pRet->usMsgType ) ;
		return true;
	}

	stMsgTransferData* pData = (stMsgTransferData*)pRet ;
	stMsg* preal = (stMsg*)( pMsg->_orgdata + sizeof(stMsgTransferData));

	if ( preal->usMsgType == MSG_JSON_CONTENT  )
	{
		stMsgJsonContent* pRet = (stMsgJsonContent*)preal ;
		char* pBuffer = (char*)preal ;
		pBuffer += sizeof(stMsgJsonContent);
//#ifdef __DEBUG
		char pLog[1024] = { 0 };
		memcpy(pLog,pBuffer,pRet->nJsLen);
		printf("rec : %s\n",pLog);
//#endif // __DEBUG

		Json::Reader reader ;
		Json::Value rootValue ;
		reader.parse(pBuffer,pBuffer + pRet->nJsLen,rootValue,false) ;
		uint16_t nMsgType = rootValue[JS_KEY_MSG_TYPE].asUInt() ;
		if ( onLogicMsg(rootValue,nMsgType,(eMsgPort)pData->nSenderPort,pData->nSessionID) )
		{
			return true ;
		}
		return false ;
	}

	if ( onLogicMsg(preal,(eMsgPort)pData->nSenderPort,pData->nSessionID) )
	{
		return true ;
	}


	CLogMgr::SharedLogMgr()->ErrorLog("unprocessed msg = %d , from port = %d , session id = %d",preal->usMsgType,pData->nSenderPort,pData->nSessionID) ;
	return false ;
}

bool IServerApp::OnLostSever(Packet* pMsg)
{
	m_nTargetSvrNetworkID = INVALID_CONNECT_ID ;
	CLogMgr::SharedLogMgr()->ErrorLog("Target server disconnected !") ;
	m_eConnectState = CNetWorkMgr::eConnectType_Disconnectd ;

	m_fReconnectTick = TIME_WAIT_FOR_RECONNECT ;// right now start reconnect ;
	return false ;
}

bool IServerApp::OnConnectStateChanged( eConnectState eSate, Packet* pMsg)
{
	m_eConnectState = eConnect_Accepted == eSate ? CNetWorkMgr::eConnectType_Connected : CNetWorkMgr::eConnectType_Disconnectd ;
	if ( eConnect_Accepted == eSate )
	{
		m_nTargetSvrNetworkID = pMsg->_connectID ;
		stMsg cMsg ;
		cMsg.cSysIdentifer = (uint8_t)getTargetSvrPortType() ;
		cMsg.usMsgType = getVerifyType() ;
		m_pNetWork->SendMsg((char*)&cMsg,sizeof(stMsg),pMsg->_connectID) ;
		CLogMgr::SharedLogMgr()->SystemLog("Connected to Target Svr") ;
		onConnectedToSvr();
		return false ;
	}

	m_nTargetSvrNetworkID = INVALID_CONNECT_ID ;
	CLogMgr::SharedLogMgr()->ErrorLog("connect target svr failed, %d seconds later reconnect",TIME_WAIT_FOR_RECONNECT) ;
	return false ;
}

bool IServerApp::run()
{
	clock_t t = clock();
	while ( m_bRunning )
	{
		if ( m_pNetWork )
		{
			m_pNetWork->ReciveMessage();
		}

		if ( m_pTimerMgr )
		{
			m_pTimerMgr->Update() ;
		}

		clock_t tNow = clock();
		float fDelta = float(tNow - t ) / CLOCKS_PER_SEC ;
		t = tNow ;
		update(fDelta);
		Sleep(10);
	}

	onExit();
	CLogMgr::SharedLogMgr()->SystemLog("sleep 4k mili seconds");
	Sleep(4000);
	shutDown();
	return true ;
}

void IServerApp::shutDown()
{
	if ( m_pNetWork )
	{
		m_pNetWork->ShutDown() ;
		m_eConnectState = CNetWorkMgr::eConnectType_None ;
	}
}

bool IServerApp::sendMsg( const char* pBuffer , int nLen )
{
	assert(m_pNetWork && "please invoke IServerApp init" );
	if ( isConnected() )
	{
		m_pNetWork->SendMsg(pBuffer,nLen,m_nTargetSvrNetworkID) ;
	}
	else
	{
		CLogMgr::SharedLogMgr()->ErrorLog("target is disconnect , can not send msg");
	}
	return isConnected() ;
}

bool IServerApp::sendMsg(  uint32_t nSessionID , const char* pBuffer , uint16_t nLen, bool bBroadcast )
{
	if ( isConnected() == false )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("target svr is not connect , send msg failed") ;
		return false ;
	}
	stMsgTransferData msgTransData ;
	msgTransData.nSenderPort = getLocalSvrMsgPortType() ;
	msgTransData.bBroadCast = bBroadcast ;
	msgTransData.nSessionID = nSessionID ;
	int nLne = sizeof(msgTransData) ;
	if ( nLne + nLen >= MAX_MSG_BUFFER_LEN )
	{
		stMsg* pmsg = (stMsg*)pBuffer ;
		CLogMgr::SharedLogMgr()->ErrorLog("msg send to session id = %d , is too big , cannot send , msg id = %d ",nSessionID,pmsg->usMsgType) ;
		return false;
	}
	memcpy(m_pSendBuffer,&msgTransData,nLne);
	memcpy(m_pSendBuffer + nLne , pBuffer,nLen );
	nLne += nLen ;
	sendMsg(m_pSendBuffer,nLne);
	return true ;
}

bool IServerApp::sendMsg( uint32_t nSessionID , Json::Value& recvValue, uint16_t nMsgID,uint8_t nTargetPort, bool bBroadcast )
{
	if ( recvValue.isNull() )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("why send a null js value") ;
		return false ;
	}

	if ( nMsgID )
	{
		if ( !recvValue[JS_KEY_MSG_TYPE] )
		{

		}
		else
		{
			CLogMgr::SharedLogMgr()->ErrorLog("msg id = %u ,already have this tag ",nMsgID ) ;
		}
		recvValue[JS_KEY_MSG_TYPE] = nMsgID ;
	}

	Json::StyledWriter writerJs ;
	std::string strContent = writerJs.write(recvValue);
	CLogMgr::SharedLogMgr()->PrintLog("send : %s",strContent.c_str());
	stMsgJsonContent msg ;
	msg.cSysIdentifer = nTargetPort ;
	msg.nJsLen = strContent.size() ;
	CAutoBuffer bufferTemp(sizeof(msg) + msg.nJsLen);
	bufferTemp.addContent(&msg,sizeof(msg)) ;
	bufferTemp.addContent(strContent.c_str(),msg.nJsLen) ;
	return sendMsg(nSessionID,bufferTemp.getBufferPtr(),bufferTemp.getContentSize(),bBroadcast) ; 
}

void IServerApp::stop()
{
	m_bRunning = false ;
}

bool IServerApp::onLogicMsg(stMsg* prealMsg , eMsgPort eSenderPort , uint32_t nSessionID)
{
	for ( auto pp : m_vAllModule )
	{
		if ( pp.second->onMsg(prealMsg,eSenderPort,nSessionID) )
		{
			return true ;
		}
	}
	return false ;
}

bool IServerApp::onLogicMsg( Json::Value& recvValue , uint16_t nmsgType, eMsgPort eSenderPort , uint32_t nSessionID )
{
	for ( auto pp : m_vAllModule )
	{
		if ( pp.second->onMsg(recvValue,nmsgType,eSenderPort,nSessionID) )
		{
			return true ;
		}
	}
	return false ;
}

void IServerApp::update(float fDeta )
{
	if ( m_eConnectState == CNetWorkMgr::eConnectType_Disconnectd )
	{
		m_fReconnectTick += fDeta ;
		if ( m_fReconnectTick >= TIME_WAIT_FOR_RECONNECT )
		{
			CLogMgr::SharedLogMgr()->SystemLog("Reconnecting....");
			doConnectToTargetSvr() ;
			m_fReconnectTick = 0 ;
		}
	}

	// moudle update ;
	for ( auto pp : m_vAllModule )
	{
		pp.second->update(fDeta);
	}
}

uint16_t IServerApp::getTargetSvrPortType()
{
	return ID_MSG_PORT_CENTER ;
}

bool IServerApp::isConnected()
{
	return m_eConnectState == CNetWorkMgr::eConnectType_Connected;
}

void IServerApp::setConnectServerConfig(stServerConfig* pConfig )
{
	if ( pConfig == nullptr )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("connect config is null") ;
		return ;
	}

	m_stConnectConfig = *pConfig ;

	if ( m_eConnectState != CNetWorkMgr::eConnectType_Connected && CNetWorkMgr::eConnectType_Connecting != m_eConnectState )
	{
		doConnectToTargetSvr();
	}
}

void IServerApp::doConnectToTargetSvr()
{
	if ( m_eConnectState == CNetWorkMgr::eConnectType_Connecting || isConnected() )
	{
		return ;
	}

	assert(m_pNetWork && "IServer init not invoke" ) ;
	assert(m_stConnectConfig.nPort && "please set connect config" ) ;
	m_pNetWork->ConnectToServer(m_stConnectConfig.strIPAddress,m_stConnectConfig.nPort,m_stConnectConfig.strPassword) ;
	m_eConnectState = CNetWorkMgr::eConnectType_Connecting ;
	CLogMgr::SharedLogMgr()->SystemLog("connecting to target svr ip = %s", m_stConnectConfig.strIPAddress );
}
uint16_t IServerApp::getVerifyType()
{
	switch ( getLocalSvrMsgPortType() )
	{
	case ID_MSG_PORT_CLIENT:
		return MSG_VERIFY_CLIENT ;
	case ID_MSG_PORT_GATE:
		return MSG_VERIFY_GATE ;
	case ID_MSG_PORT_LOGIN:
		return MSG_VERIFY_LOGIN ;
	case ID_MSG_PORT_VERIFY:
		return MSG_VERIFY_VERIYF ;
	case ID_MSG_PORT_APNS:
		return MSG_VERIFY_APNS ;
	case ID_MSG_PORT_LOG:
		return MSG_VERIFY_LOG; 
	case ID_MSG_PORT_DATA:
		return MSG_VERIFY_DATA ;
	case ID_MSG_PORT_TAXAS:
		return MSG_VERIFY_TAXAS;
	case ID_MSG_PORT_DB:
		return MSG_VERIFY_DB ;
	case ID_MSG_PORT_NIU_NIU:
		return MSG_VERIFY_NIU_NIU ;
	case ID_MSG_PORT_GOLDEN:
		return MSG_VERIFY_GOLDEN;
	default:
		assert(0 && "what verify type for the svr ? " ) ;
		break;
	}
	return MSG_VERIFY_END ;
}

void IServerApp::onExit()
{
	for ( auto pp : m_vAllModule )
	{
		pp.second->onExit();
	}
}

void IServerApp::onConnectedToSvr()
{
	for ( auto pp : m_vAllModule )
	{
		pp.second->onConnectedSvr();
	}
}

void IServerApp::registerModule(IGlobalModule* pModule)
{
	auto pp = getModuleByType(pModule->getModuleType()) ;
	assert(pp == nullptr && "already have this module" );
	m_vAllModule[pModule->getModuleType()] = pModule ;
	pModule->init(this);
}

IGlobalModule* IServerApp::getModuleByType(uint16_t nType )
{
	auto pp = m_vAllModule.find(nType) ;
	if ( pp != m_vAllModule.end() )
	{
		return pp->second ;
	}
	return nullptr;
}