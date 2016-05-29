#pragma warning(disable:4800)
#include "LoginDBManager.h"
#include "LogManager.h"
#include "DBRequest.h"
#include "ServerMessageDefine.h"
#include "LoginApp.h"
#include "DataBaseThread.h"
#define PLAYER_BRIF_DATA "playerName,userUID,sex,vipLevel,defaultPhotoID,isUploadPhoto,exp,coin,diamond"
#define PLAYER_DETAIL_DATA "playerName,userUID,sex,vipLevel,defaultPhotoID,isUploadPhoto,exp,coin,diamond,signature,singleWinMost,winTimes,loseTimes,yesterdayPlayTimes,todayPlayTimes,longitude,latitude,offlineTime"
CDBManager::CDBManager()
{
	m_vReserverArgData.clear();
}

CDBManager::~CDBManager()
{
	LIST_ARG_DATA::iterator iter = m_vReserverArgData.begin() ;
	for ( ; iter != m_vReserverArgData.end() ; ++iter )
	{
		if ( *iter )
		{
			delete *iter ;
			*iter = NULL ;
		}
	}
	m_vReserverArgData.clear() ;
}

void CDBManager::init(IServerApp* pApp)
{
	IGlobalModule::init(pApp) ;
	if ( MAX_LEN_ACCOUNT < 20 )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("MAX_LEN_ACCOUNT must big than 18 , or guset login will crash ") ;
	}
}

bool CDBManager::onMsg( Json::Value& pV ,uint16_t nMsgType, eMsgPort eSenderPort , uint32_t nSessionID )
{
	if ( IGlobalModule::onMsg(pV,nMsgType,eSenderPort,nSessionID) )
	{
		return true;
	}
	// construct sql
	stArgData* pdata = GetReserverArgData() ;
	if ( pdata == NULL )
	{
		pdata = new stArgData ;
	}

	pdata->eFromPort = eSenderPort ;
	switch( nMsgType )
	{
	case MSG_PLAYER_REGISTER:
		{
			pdata->nSessionID = nSessionID ;

			uint8_t nRegType = pV["regType"].asInt() ;
			if ( nRegType == 0  )
			{
				char cAccount[100] = { 0 } ;
				time_t tCur = time(NULL);
				tm t ;
				t = *localtime(&tCur);
				uint16_t nRandN = rand()%10000 ;
				uint16_t nRandN2 = rand() % 100 ;
				sprintf_s(cAccount,"%d%d%d%d%d%d%d",nRandN2,t.tm_mon,t.tm_mday,t.tm_hour,t.tm_min,t.tm_sec,nRandN );
				pV["acc"] = cAccount ;
				pV["pwd"] = "hello" ;
			}
			
			pdata->nExtenArg1 = nRegType;
			if ( strlen(pV["acc"].asCString()) >= MAX_LEN_ACCOUNT || strlen(pV["pwd"].asCString()) >= MAX_LEN_PASSWORD )
			{
				CLogMgr::SharedLogMgr()->ErrorLog("pLoginRegister password or account len is too long ");
				m_vReserverArgData.push_back(pdata) ;
				break; 
			}

			stDBRequest* pRequest = CDBRequestQueue::SharedDBRequestQueue()->GetReserveRequest();
			pRequest->cOrder = eReq_Order_Super ;
			pRequest->eType = eRequestType_Select ;
			pRequest->nRequestUID = nMsgType ;
			pRequest->pUserData = pdata ;
			// format sql String ;
			pRequest->nSqlBufferLen = sprintf_s(pRequest->pSqlBuffer,"call RegisterAccount('%s','%s',%d,%d);",pV["acc"].asCString(),pV["pwd"].asCString(),nRegType,pV["regChannel"].asInt() ) ;
			CDBRequestQueue::SharedDBRequestQueue()->PushRequest(pRequest) ;
		}
		break;
	case MSG_PLAYER_LOGIN:
		{
			
			pdata->nSessionID = nSessionID ;
			// must end with \0
			if ( strlen(pV["acc"].asCString()) >= MAX_LEN_ACCOUNT || strlen(pV["pwd"].asCString()) >= MAX_LEN_PASSWORD )
			{
				CLogMgr::SharedLogMgr()->ErrorLog("password or account len is too long ");
				m_vReserverArgData.push_back(pdata) ;
				break; 
			}

			stDBRequest* pRequest = CDBRequestQueue::SharedDBRequestQueue()->GetReserveRequest();
			pRequest->cOrder = eReq_Order_High ;
			pRequest->eType = eRequestType_Select ;
			pRequest->nRequestUID = nMsgType ;
			pRequest->pUserData = pdata ;
			// format sql String ;
			pRequest->nSqlBufferLen = sprintf_s(pRequest->pSqlBuffer,"call CheckAccount('%s','%s')",pV["acc"].asCString(),pV["pwd"].asCString() ) ;
			CDBRequestQueue::SharedDBRequestQueue()->PushRequest(pRequest) ;
		}
		break;
	case MSG_PLAYER_BIND_ACCOUNT:
		{
			uint32_t nUserUID = pV["UID"].asUInt() ;
			const char* pAccount = pV["acc"].asCString() ;
			const char* pwd = pV["pwd"].asCString() ;

			pdata->nSessionID = nSessionID ;
			pdata->nExtenArg1 = nUserUID ;
			if ( strlen(pAccount) >= MAX_LEN_ACCOUNT || strlen(pwd) >= MAX_LEN_PASSWORD )
			{
				CLogMgr::SharedLogMgr()->ErrorLog("MSG_PLAYER_BIND_ACCOUNT password or account len is too long ");
				m_vReserverArgData.push_back(pdata) ;
				break; 
			}

			stDBRequest* pRequest = CDBRequestQueue::SharedDBRequestQueue()->GetReserveRequest();
			pRequest->cOrder = eReq_Order_Super ;
			pRequest->eType = eRequestType_Select ;
			pRequest->nRequestUID = nMsgType ;
			pRequest->pUserData = pdata;
			pRequest->nSqlBufferLen = sprintf_s(pRequest->pSqlBuffer,"call RebindAccount(%d,'%s','%s')",nUserUID,pAccount,pwd ) ;
			CDBRequestQueue::SharedDBRequestQueue()->PushRequest(pRequest) ;
		}
		break;
	case MSG_MODIFY_PASSWORD:
		{
			uint32_t nUserUID = pV["UID"].asUInt() ;
			const char* pOldPwd = pV["oldPwd"].asCString() ;
			const char* pwd = pV["pwd"].asCString() ;

			pdata->nSessionID = nSessionID ;
			pdata->nExtenArg1 = nUserUID ;
			if ( strlen(pOldPwd) >= MAX_LEN_PASSWORD || strlen(pwd) >= MAX_LEN_PASSWORD )
			{
				CLogMgr::SharedLogMgr()->ErrorLog("MSG_MODIFY_PASSWORD password or account len is too long ");
				m_vReserverArgData.push_back(pdata) ;
				break; 
			}

			stDBRequest* pRequest = CDBRequestQueue::SharedDBRequestQueue()->GetReserveRequest();
			pRequest->cOrder = eReq_Order_Super ;
			pRequest->eType = eRequestType_Select ;
			pRequest->nRequestUID = nMsgType ;
			pRequest->pUserData = pdata;
			pRequest->nSqlBufferLen = sprintf_s(pRequest->pSqlBuffer,"call ModifyPassword(%d,'%s','%s')",nUserUID,pOldPwd,pwd ) ;
			CDBRequestQueue::SharedDBRequestQueue()->PushRequest(pRequest) ;
		}
		break;
	//case MSG_RESET_PASSWORD:
	//	{
	//		stMsgResetPassword* pMsgRet = (stMsgResetPassword*)pmsg ;
	//		pdata->nSessionID = nSessionID ;
	//		if ( strlen(pMsgRet->cAccount) >= MAX_LEN_ACCOUNT || strlen(pMsgRet->cNewPassword) >= MAX_LEN_PASSWORD )
	//		{
	//			CLogMgr::SharedLogMgr()->ErrorLog("MSG_MODIFY_PASSWORD password or account len is too long ");
	//			m_vReserverArgData.push_back(pdata) ;
	//			break; 
	//		}

	//		stDBRequest* pRequest = CDBRequestQueue::SharedDBRequestQueue()->GetReserveRequest();
	//		pRequest->cOrder = eReq_Order_Super ;
	//		pRequest->eType = eRequestType_Select ;
	//		pRequest->nRequestUID = pmsg->usMsgType ;
	//		pRequest->pUserData = pdata;
	//		pRequest->nSqlBufferLen = sprintf_s(pRequest->pSqlBuffer,"call ResetPassword('%s','%s')",pMsgRet->cAccount,pMsgRet->cNewPassword ) ;
	//		CDBRequestQueue::SharedDBRequestQueue()->PushRequest(pRequest) ;
	//	}
	//	break;
	default:
		{
			m_vReserverArgData.push_back(pdata) ;
			CLogMgr::SharedLogMgr()->ErrorLog("unknown msg type = %d",nMsgType ) ;
			return false ;
		}
	}
	return true ;
}

void CDBManager::OnDBResult(stDBResult* pResult)
{
	stArgData*pdata = (stArgData*)pResult->pUserData ;
	switch ( pResult->nRequestUID )
	{
	case  MSG_PLAYER_REGISTER:
		{
			Json::Value jValue ;
			jValue["regType"] = (uint8_t)pdata->nExtenArg1; 
			jValue["UID"] = 0 ;
			jValue["ret"] = 0 ;
			if ( pResult->nAffectRow <= 0 )
			{
				jValue["ret"] = 1 ;
				getSvrApp()->sendMsg(pdata->nSessionID,jValue,pResult->nRequestUID);
				CLogMgr::SharedLogMgr()->ErrorLog("why register affect row = 0 ") ;
				return ;
			}

			 CMysqlRow& pRow = *pResult->vResultRows.front() ;
			 jValue["ret"] = pRow["nOutRet"]->IntValue();
			 if ( pRow["nOutRet"]->IntValue() != 0 )
			 {
				 getSvrApp()->sendMsg(pdata->nSessionID,jValue,pResult->nRequestUID);
				 CLogMgr::SharedLogMgr()->PrintLog("register failed duplicate account = %s",pRow["strAccount"]->CStringValue() );
				 return ;
			 }

			 jValue["acc"] = pRow["strAccount"]->CStringValue() ;
			 jValue["pwd"] = pRow["strPassword"]->CStringValue() ;
			 jValue["UID"] = pRow["nOutUserUID"]->IntValue();

			// request db to create new player data 
			stMsgRequestDBCreatePlayerData msgCreateData ;
			msgCreateData.nUserUID = pRow["nOutUserUID"]->IntValue() ;
			msgCreateData.isRegister = pdata->nExtenArg1 != 0 ;
			getSvrApp()->sendMsg(pdata->nSessionID,(char*)&msgCreateData,sizeof(msgCreateData)) ;

			// tell client the success register result ;
			getSvrApp()->sendMsg(pdata->nSessionID,jValue,pResult->nRequestUID);
			CLogMgr::SharedLogMgr()->PrintLog("register success account = %s",pRow["strAccount"]->CStringValue() );

			stMsgLoginSvrInformGateSaveLog msglog ;
			msglog.nlogType = eLog_Register ;
			msglog.nUserUID = msgCreateData.nUserUID ;
			getSvrApp()->sendMsg(pdata->nSessionID,(char*)&msglog,sizeof(msglog)) ;
		}
		break;
	case MSG_PLAYER_LOGIN:
		{
			uint8_t nRegType = 0 ;
			uint32_t nUserUID = 0 ;
			uint8_t nRet = 0 ; 
			if ( pResult->nAffectRow > 0 )
			{
				CMysqlRow& pRow = *pResult->vResultRows.front() ;
				nRet = pRow["nOutRet"]->IntValue() ;
				nRegType = pRow["nOutRegisterType"]->IntValue() ;
				nUserUID = pRow["nOutUID"]->IntValue() ;
				CLogMgr::SharedLogMgr()->PrintLog("check accout = %s  ret = %d",pRow["strAccount"]->CStringValue(),nRet  ) ;
			}
			else
			{
				nRet = 1 ;  // account error ;   
				CLogMgr::SharedLogMgr()->ErrorLog("check account  why affect row = 0 ? ") ;
			}

			Json::Value jValue ;
			jValue["regType"] = nRegType ;
			jValue["ret"] = nRet ;
			getSvrApp()->sendMsg(pdata->nSessionID,jValue,pResult->nRequestUID);
			// tell data svr login success 
			if ( nRet == 0 )
			{
				stMsgOnPlayerLogin msgData ;
				msgData.nUserUID =  nUserUID;
				getSvrApp()->sendMsg(pdata->nSessionID,(char*)&msgData,sizeof(msgData) ) ;

				stMsgLoginSvrInformGateSaveLog msglog ;
				msglog.nlogType = eLog_Login ;
				msglog.nUserUID = nUserUID ;
				getSvrApp()->sendMsg(pdata->nSessionID,(char*)&msglog,sizeof(msglog)) ;
			}
		}
		break;
	case MSG_PLAYER_BIND_ACCOUNT:
		{
			uint8_t nRet = 0 ;
			if ( pResult->nAffectRow > 0 )
			{
				CMysqlRow& pRow = *pResult->vResultRows.front() ;
				nRet = pRow["nOutRet"]->IntValue() ;
			}
			else
			{
				nRet = 3 ;
				CLogMgr::SharedLogMgr()->ErrorLog("uid = %d ,bind account error db ",pdata->nExtenArg1) ;
			}

			Json::Value jValue ;
			jValue["ret"] = nRet ;
			getSvrApp()->sendMsg(pdata->nSessionID,jValue,pResult->nRequestUID);
			CLogMgr::SharedLogMgr()->PrintLog("rebind account ret = %d , userUID = %d",nRet,pdata->nExtenArg1 ) ;

			if ( nRet == 0 )
			{
				stMsgOnPlayerBindAccount msgInfom ;
				getSvrApp()->sendMsg(pdata->nSessionID,(char*)&msgInfom,sizeof(msgInfom)); 

				stMsgLoginSvrInformGateSaveLog msglog ;
				msglog.nlogType = eLog_BindAccount ;
				msglog.nUserUID = pdata->nExtenArg1 ;
				getSvrApp()->sendMsg(pdata->nSessionID,(char*)&msglog,sizeof(msglog)) ;
			}
		}
		break;
	case MSG_MODIFY_PASSWORD:
		{
			uint8_t nRet = 0 ;
			if ( pResult->nAffectRow > 0 )
			{
				CMysqlRow& pRow = *pResult->vResultRows.front() ;
				nRet = pRow["nOutRet"]->IntValue() ;
			}
			else
			{
				nRet = 3 ;
				CLogMgr::SharedLogMgr()->ErrorLog("uid = %d , modify password error db ",pdata->nExtenArg1) ;
			}
			CLogMgr::SharedLogMgr()->PrintLog("uid = %d modify password ret = %d",pdata->nExtenArg1,nRet ) ;
			Json::Value jValue ;
			jValue["ret"] = nRet ;
			getSvrApp()->sendMsg(pdata->nSessionID,jValue,pResult->nRequestUID);

			if ( nRet == 0 )
			{
				stMsgLoginSvrInformGateSaveLog msglog ;
				msglog.nlogType = eLog_ModifyPwd ;
				msglog.nUserUID = pdata->nExtenArg1 ;
				getSvrApp()->sendMsg(pdata->nSessionID,(char*)&msglog,sizeof(msglog)) ;
			}
		}
		break;
	//case MSG_RESET_PASSWORD:
	//	{
	//		stMsgResetPasswordRet msgBack ;
	//		msgBack.nRet = 0 ;
	//		uint32_t nUID = 0 ;
	//		if ( pResult->nAffectRow > 0 )
	//		{
	//			CMysqlRow& pRow = *pResult->vResultRows.front() ;
	//			msgBack.nRet = pRow["nOutRet"]->IntValue() ;
	//			nUID = pRow["nUID"]->IntValue() ;
	//		}
	//		else
	//		{
	//			msgBack.nRet = 1 ;
	//		}
	//		CLogMgr::SharedLogMgr()->PrintLog("uid = %d modify password ret = %d",nUID,msgBack.nRet ) ;
	//		m_pTheApp->sendMsg(pdata->nSessionID,(char*)&msgBack,sizeof(msgBack)); 

	//		if ( msgBack.nRet == 0 )
	//		{
	//			stMsgLoginSvrInformGateSaveLog msglog ;
	//			msglog.nlogType = eLog_ResetPassword ;
	//			msglog.nUserUID = nUID ;
	//			m_pTheApp->sendMsg(pdata->nSessionID,(char*)&msglog,sizeof(msglog)) ;
	//		}
	//	}
	//	break;
	default:
		{
			CLogMgr::SharedLogMgr()->ErrorLog("unprocessed login db result msg id = %d ", pResult->nRequestUID );
		}
	}
	m_vReserverArgData.push_back(pdata) ;
}

CDBManager::stArgData* CDBManager::GetReserverArgData()
{
	LIST_ARG_DATA::iterator iter = m_vReserverArgData.begin() ;
	if ( iter != m_vReserverArgData.end() )
	{
		stArgData* p = *iter ;
		m_vReserverArgData.erase(iter) ;
		p->Reset();
		return p ;
	}
	return NULL ;
}

