#include "LoginApp.h"
#include "ServerMessageDefine.h"
#include "LoginDBManager.h"
#include "DataBaseThread.h"
#include "DBRequest.h"
#include "log4z.h"
CLoginApp::CLoginApp()
{
	m_pDBThread = NULL ;
}

CLoginApp::~CLoginApp()
{
	if ( m_pDBThread )
	{
		m_pDBThread->StopWork();
		delete m_pDBThread ;
		m_pDBThread = NULL ;
	}
}

bool CLoginApp::init()
{
	IServerApp::init();
	//CLogMgr::SharedLogMgr()->SetOutputFile("LoginSvr");
	m_stSvrConfigMgr.LoadFile("../configFile/serverConfig.txt");
	
	time_t tSeed = time(NULL);
	srand(tSeed);


	// start Db thread 
	stServerConfig* pSvrConfigItem = m_stSvrConfigMgr.GetServerConfig(eSvrType_DataBase );
	if ( pSvrConfigItem == NULL )
	{
		LOGFMTE("Data base config is null , can not start login svr ") ;
		return false;
	}
	m_pDBThread = new CDataBaseThread ;
	bool bRet = m_pDBThread->InitDataBase(pSvrConfigItem->strIPAddress,pSvrConfigItem->nPort,pSvrConfigItem->strAccount,pSvrConfigItem->strPassword,"game");
	if ( bRet )
	{
		m_pDBThread->Start() ;
		LOGFMTI("start db thread ok") ;
	}
	else
	{	
		delete m_pDBThread ;
		m_pDBThread = NULL ;
		LOGFMTE("start db thread errror ") ;
		return false;
	}

	installModule(eModule_Type);

	// connected to center ;
	pSvrConfigItem = m_stSvrConfigMgr.GetServerConfig(eSvrType_Center );
	if ( pSvrConfigItem == NULL )
	{
		LOGFMTE("center svr config is null so can not start svr ") ;
		return false;
	}
	setConnectServerConfig(pSvrConfigItem);
	return true ;
}

void CLoginApp::update(float fdeta )
{
	IServerApp::update(fdeta);
				// process DB Result ;
	CDBRequestQueue::VEC_DBRESULT vResultOut ;
	CDBRequestQueue::SharedDBRequestQueue()->GetAllResult(vResultOut) ;
	CDBRequestQueue::VEC_DBRESULT::iterator iter = vResultOut.begin() ;
	auto pM = (CDBManager*)getModuleByType(eModule_Type);
	for ( ; iter != vResultOut.end(); ++iter )
	{
		stDBResult* pRet = *iter ;
		pM->OnDBResult(pRet) ;
		delete pRet ;
	}
	vResultOut.clear();
}

void CLoginApp::onExit()
{
	if ( m_pDBThread )
	{
		m_pDBThread->StopWork();
	}
}

IGlobalModule* CLoginApp::createModule(uint16_t eModuleType)
{
	auto p = IServerApp::createModule(eModuleType);
	if (p)
	{
		return p;
	}

	if (eModuleType == eModule_Type)
	{
		p = new CDBManager();
	}
	return p;
}
