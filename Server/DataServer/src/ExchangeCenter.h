#pragma once
#include "MessageDefine.h"
#include "ExchangeConfig.h"
#include "IGlobalModule.h"
class CExchangeCenter
	:public IGlobalModule
{

public:
	struct stExchangeEntry
		:public stExchangeItem
	{
		bool bDirty ;
	};
public:
	CExchangeCenter(const char* pConfigFilePath);
	~CExchangeCenter() ;
	uint16_t getModuleType(){ return eMod_Exchange ; }
	bool onMsg(stMsg* prealMsg , eMsgPort eSenderPort , uint32_t nSessionID)override ;
	bool onMsg(Json::Value& prealMsg ,uint16_t nMsgType, eMsgPort eSenderPort , uint32_t nSessionID)override ;
	void onTimeSave()override ;
	void onConnectedSvr()override ;
protected:
	CExchangeConfig m_tConfig ;
	char* pItemsBuffer ;
	uint16_t nItemsBufferLen ;

	bool bItemBufferDirty ;
	std::map<uint16_t,stExchangeEntry*> vExchangeEntrys ;
};