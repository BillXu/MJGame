#pragma once 
#include "ISeverApp.h"
#include "Timer.h"
#include "ServerConfig.h"
class CDBManager ;
class CDataBaseThread ;
class CLoginApp
	:public IServerApp
{
public:
	enum eModeType
	{
		eModule_Type = 200,
	};
public:
	CLoginApp();
	~CLoginApp();
	bool init();
	void update(float fdeta );	
	void onExit();
	uint16_t getLocalSvrMsgPortType(){ return ID_MSG_PORT_LOGIN ;}
	IGlobalModule* createModule(uint16_t eModuleType)override;
protected:
	CDataBaseThread* m_pDBThread;
	CSeverConfigMgr m_stSvrConfigMgr ;
};