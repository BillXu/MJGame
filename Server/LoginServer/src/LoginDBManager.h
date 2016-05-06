#pragma once
#include "ServerMessageDefine.h"
#include <list>
#include <map>
#include <assert.h>
#include "IGlobalModule.h"
struct stDBResult ;
class CLoginApp;
class CDBManager
	:public IGlobalModule
{
public:
	struct stArgData
	{
		eMsgPort eFromPort;
		unsigned int nSessionID ; // always , refer to client session with serveper , used in GameServer and LoginServer , to rresent a Player ;
		unsigned int nExtenArg1 ; // reserver argument , for later use ;
		unsigned int nExtenArg2 ; // reserver argument , for later use ;
		void* pUserData ;   // maybe need data ;
		stArgData(){ nSessionID = 0 ; nExtenArg2 = nExtenArg1 = 0 ; pUserData = NULL ;}
		void Reset(){nSessionID = 0 ; nExtenArg2 = nExtenArg1 = 0 ; assert(pUserData==NULL);}
	};
	typedef std::list<stArgData*> LIST_ARG_DATA ;
	enum eModeType
	{
		eModule_Type = 2 ,
	};
public:
	CDBManager();
	~CDBManager();
	uint16_t getModuleType()override { return eModule_Type ;}
	void init( IServerApp* svrApp )override;
	bool onMsg(Json::Value& prealMsg ,uint16_t nMsgType, eMsgPort eSenderPort , uint32_t nSessionID)override ;
	void OnDBResult(stDBResult* pResult);
	stArgData* GetReserverArgData();
protected:
	LIST_ARG_DATA m_vReserverArgData ;
};