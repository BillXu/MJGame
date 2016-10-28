#include "PlayerFriend.h"
#include "MessageDefine.h"
#include "log4z.h"
#include "GameServerApp.h"
#include "PlayerManager.h"
#include "Player.h"
#include "PlayerBaseData.h"
#include <time.h>
#include "ServerMessageDefine.h"
#include "PlayerEvent.h"
#include "PlayerMail.h"
#include "EventCenter.h"
#include "AutoBuffer.h"
#include "GameServerApp.h"
#include "AsyncRequestQuene.h"
CPlayerFriend::CPlayerFriend(CPlayer* pPlayer):IPlayerComponent(pPlayer)
{
	m_bDirty = false;
}

CPlayerFriend::~CPlayerFriend()
{
	
}

bool CPlayerFriend::OnMessage( Json::Value& recvValue , uint16_t nmsgType, eMsgPort eSenderPort)
{
	switch(nmsgType)
	{
	case MSG_REQ_ADD_FRIEND:
		{
			uint32_t nTargetUID = recvValue["targetUID"].asUInt();
			auto strNotice = recvValue["notice"].asString() ;
			CPlayer* pTargetPlayer = CGameServerApp::SharedGameServerApp()->GetPlayerMgr()->GetPlayerByUserUID(nTargetUID,false) ;
			Json::Value jsmsgBack ;
			jsmsgBack["ret"] = 0 ;
			jsmsgBack["targetUID"] = nTargetUID ;
			do 
			{
				if ( IsFriendListFull() )
				{
					jsmsgBack["ret"] = 3 ;
					break ;
				}

				if ( isPlayerUIDFriend(nTargetUID) )
				{
					jsmsgBack["ret"] = 4 ;
					break;
				}

				if ( pTargetPlayer == nullptr )
				{
					jsmsgBack["ret"] = 1 ;
					break;
				}

				CPlayerFriend* pTaretFriendModule = (CPlayerFriend*)pTargetPlayer->GetComponent(ePlayerComponent_Friend);
				if ( pTaretFriendModule->IsFriendListFull() )
				{
					jsmsgBack["ret"] = 2 ;
					break;
				}
				pTaretFriendModule->OnPlayerWantAddMe(this,strNotice);
			} while (0);
			SendMsg(jsmsgBack,nmsgType);
		}
		break ;
	case MSG_REPLY_NEW_FRIEND_REQ:
		{
			uint32_t nTargetUID = recvValue["targetUID"].asUInt() ;
			uint8_t isAgree = recvValue["isAgree"].asUInt() ;

			if ( isAgree )
			{
				AddFriend(nTargetUID) ;
			}

			CPlayer* pReplyToPlayer = CGameServerApp::SharedGameServerApp()->GetPlayerMgr()->GetPlayerByUserUID(nTargetUID) ;
			CPlayerFriend* pReplyToFriendComponed =  NULL;
			if ( pReplyToPlayer )
			{
				pReplyToFriendComponed = (CPlayerFriend*)pReplyToPlayer->GetComponent(ePlayerComponent_Friend) ;
				pReplyToFriendComponed->OnOtherReplayMeAboutAddItbeFriend(isAgree,this);
			}
			else
			{
				LOGFMTD("send mail to tell the result ") ;
				if ( isAgree )
				{
					Json::Value jsarg ;
					jsarg["friendUID"] = GetPlayer()->GetUserUID() ;
					CPlayerMailComponent::PostOfflineEvent(CPlayerMailComponent::Event_AddFriend,jsarg,nTargetUID);
				}
				
			}
		}
		break ;
	case MSG_DELETE_FRIEND:
		{
			uint32_t nTargetUID = recvValue["targetUID"].asUInt() ;
			RemoveFriendByUID(nTargetUID);

			Json::Value jsgMsgBack ;
			jsgMsgBack["ret"] = 0 ;
			jsgMsgBack["targetUID"] = nTargetUID ;
			SendMsg(jsgMsgBack,nmsgType) ;

			CPlayer* pReplyToPlayer = CGameServerApp::SharedGameServerApp()->GetPlayerMgr()->GetPlayerByUserUID(nTargetUID) ;
			if ( pReplyToPlayer )
			{
				auto pReplyToFriendComponed = (CPlayerFriend*)pReplyToPlayer->GetComponent(ePlayerComponent_Friend) ;
				pReplyToFriendComponed->RemoveFriendByUID(GetPlayer()->GetUserUID()) ;
			}
			else
			{
				LOGFMTD("send msg tell delete friend event") ;
				Json::Value jsarg ;
				jsarg["friendUID"] = GetPlayer()->GetUserUID() ;
				CPlayerMailComponent::PostOfflineEvent(CPlayerMailComponent::Event_DelteFriend,jsarg,nTargetUID);
			}
		}
		break;
	case MSG_REQ_FRIEND_LIST:
		{
			uint32_t nStartIdx = recvValue["startIdx"].asUInt() ;
			uint32_t nReqCnt = recvValue["cnt"].asUInt() ;
			SendListToClient(nStartIdx,nReqCnt);
		}
		break ;
	default:
		return false ;
	}
	return true ;
}

bool CPlayerFriend::OnMessage(stMsg* pMsg, eMsgPort eSenderPort )
{
	switch ( pMsg->usMsgType )
	{
	case MSG_READ_FRIEND_LIST:
		{
			stMsgReadFriendListRet* pRet = (stMsgReadFriendListRet*)pMsg ;
			char* pBuffer = (char*)&pRet;
			pBuffer += sizeof(stMsgReadFriendListRet);

			if ( pRet->nFriendCountLen )
			{
				Json::Reader reader ;
				Json::Value arrayValue ;
				reader.parse(pBuffer,pBuffer + pRet->nFriendCountLen,arrayValue,false);
				for ( uint16_t nIdx = 0 ; nIdx < arrayValue.size(); ++nIdx )
				{
					m_vAllFriends.insert(arrayValue[nIdx].asUInt());
				}
			}
			LOGFMTD("read friend list ok uid = %d",GetPlayer()->GetUserUID());
		}
		break;
	case MSG_REQUEST_FRIEND_LIST:
		{
			//SendListToClient();
		}
		break;
	case MSG_PLAYER_ADD_FRIEND:
		{
			//stMsgPlayerAddFriend* pMsgRet = (stMsgPlayerAddFriend*)pMsg ;
			//CPlayer* pTargetPlayer = CGameServerApp::SharedGameServerApp()->GetPlayerMgr()->GetPlayerByUserUID(pMsgRet->nTargetUID,false) ;
			//stMsgPlayerAddFriendRet msg ;
			//msg.nRet = 0 ;
			//msg.nTaregtUID = pMsgRet->nTargetUID ;
			//memset(msg.pReplayerName,0,sizeof(msg.pReplayerName));
			//if ( pTargetPlayer )
			//{
			//	sprintf_s(msg.pReplayerName,sizeof(msg.pReplayerName),"%s",pTargetPlayer->GetBaseData()->GetPlayerName()) ;
			//}

			//if ( IsFriendListFull() )
			//{
			//	msg.nRet = 2 ;
			//	LOGFMTD("friend list is full, can not add");
			//}
			//else if ( isPlayerUIDFriend(pMsgRet->nTargetUID) )
			//{
			//	msg.nRet = 5 ;
			//	LOGFMTD("already your friend , can not add");
			//}
			//else if ( pTargetPlayer == nullptr )
			//{
			//	msg.nRet = 4 ;
			//}
			//else
			//{
			//	CPlayerFriend* pTaretFriendModule = (CPlayerFriend*)pTargetPlayer->GetComponent(ePlayerComponent_Friend);
			//	if ( pTaretFriendModule->IsFriendListFull() )
			//	{
			//		msg.nRet = 3 ;
			//	}
			//	else
			//	{
			//		pTaretFriendModule->OnPlayerWantAddMe(this);
			//	}
			//}
			//if ( msg.nRet )
			//{
			//	SendMsg(&msg,sizeof(msg)) ;
			//}
		}
		break;
	case MSG_PLAYER_BE_ADDED_FRIEND_REPLY:
		{
			stMsgPlayerBeAddedFriendReply* pMsgRet = (stMsgPlayerBeAddedFriendReply*)pMsg ;
			CPlayer* pReplyToPlayer = CGameServerApp::SharedGameServerApp()->GetPlayerMgr()->GetPlayerByUserUID(pMsgRet->nReplayToPlayerUserUID) ;
			CPlayerFriend* pReplyToFriendComponed =  NULL;
			stMsgPlayerBeAddedFriendReplyRet msgBack ;
			msgBack.nNewFriendUserUID = pMsgRet->nReplayToPlayerUserUID ;
			msgBack.nRet = 0 ;
			if ( pReplyToPlayer )
			{
				pReplyToFriendComponed = (CPlayerFriend*)pReplyToPlayer->GetComponent(ePlayerComponent_Friend) ;
			}

			if ( pReplyToFriendComponed )
			{
				if ( IsFriendListFull() )
				{
					msgBack.nRet = 2 ;
				}

				if ( msgBack.nRet == 0 && pReplyToFriendComponed->IsFriendListFull() )
				{
					msgBack.nRet = 1 ;
				}

				if ( pMsgRet->bAgree && msgBack.nRet == 0  )
				{
					AddFriend(pMsgRet->nReplayToPlayerUserUID) ;
				}

				pReplyToFriendComponed->OnOtherReplayMeAboutAddItbeFriend(pMsgRet->bAgree,this);
			}
			else
			{
				LOGFMTE("%d uid offline can not reply add friend ",pMsgRet->nReplayToPlayerUserUID);
				msgBack.nRet = 3 ;
			}

			if ( pMsgRet->bAgree )
			{
				SendMsg(&msgBack,sizeof(msgBack)) ;
				LOGFMTD("uid = %u msg type = stMsgPlayerBeAddedFriendReplyRet, typeid = %s",GetPlayer()->GetUserUID(),typeid(stMsgPlayerBeAddedFriendReplyRet)) ;
			}
		}
		break;
	/*case MSG_PLAYER_DELETE_FRIEND:
		{
			stMsgPlayerDelteFriendRet msgBack ;
			msgBack.nRet = 0 ;
			stMsgPlayerDelteFriend* pMsgRet = (stMsgPlayerDelteFriend*)pMsg ;
			msgBack.nDeleteUID = pMsgRet->nDelteFriendUserUID ;
			if ( isPlayerUIDFriend(pMsgRet->nDelteFriendUserUID) == false )
			{
				msgBack.nRet = 1 ;
			}
			else
			{
				RemoveFriendByUID(pMsgRet->nDelteFriendUserUID) ;
			}
			SendMsg(&msgBack,sizeof(msgBack));

			CPlayer* pReplyToPlayer = CGameServerApp::SharedGameServerApp()->GetPlayerMgr()->GetPlayerByUserUID(msgBack.nDeleteUID) ;
			CPlayerFriend* pReplyToFriendComponed =  NULL;
			if ( pReplyToPlayer )
			{
				pReplyToFriendComponed = (CPlayerFriend*)pReplyToPlayer->GetComponent(ePlayerComponent_Friend) ;
				pReplyToFriendComponed->RemoveFriendByUID(GetPlayer()->GetUserUID()) ;
			}
			else
			{
				LOGFMTE("no online tell mail to delete friend");
			}
		}
		break;*/
	default:
		return false ;
	}
	return true ;
}

void CPlayerFriend::OnPlayerDisconnect()
{
	TimerSave();
	CEventCenter::SharedEventCenter()->RemoveEventListenner(eEvent_PlayerOnline,this,CPlayerFriend::EventFunc ) ;
	CEventCenter::SharedEventCenter()->RemoveEventListenner(eEvent_PlayerOffline,this,CPlayerFriend::EventFunc ) ;
}

void CPlayerFriend::Reset()
{
	m_bDirty = false;
	m_vAllFriends.clear();
	// send request ;
	//stMsgReadFriendList msgRead ;
	//msgRead.nUserUID = GetPlayer()->GetUserUID() ;
	//SendMsg(&msgRead,sizeof(msgRead)) ;
	readFriendList();
}

void CPlayerFriend::Init()
{
	IPlayerComponent::Init() ;
	m_vAllFriends.clear();
	m_bDirty = false;

	// send request ;
	//stMsgReadFriendList msgRead ;
	//msgRead.nUserUID = GetPlayer()->GetUserUID() ;
	//SendMsg(&msgRead,sizeof(msgRead)) ;
	readFriendList();
}

void CPlayerFriend::SendListToClient(uint32_t nStartIdx , uint32_t nreqCnt)
{
	Json::Value jsFirendUIDs ;
	auto iter = m_vAllFriends.begin() ;
	for ( uint32_t nIdx = 0 ; iter != m_vAllFriends.end() ; ++iter , ++nIdx )
	{
		if ( nIdx < nStartIdx )
		{
			continue;
		}

		jsFirendUIDs[jsFirendUIDs.size()] = *iter ;
		if ( jsFirendUIDs.size() == nreqCnt )
		{
			break;
		}
	}

	Json::Value jsmsg ;
	jsmsg["totalCnt"] = m_vAllFriends.size();
	jsmsg["friendUIDs"] = jsFirendUIDs ;
	SendMsg(jsmsg,MSG_REQ_FRIEND_LIST) ;
	LOGFMTD("send friend list to client cnt = %u , send cnt = %u",m_vAllFriends.size() , jsFirendUIDs.size()) ;
}

void CPlayerFriend::OnPlayerWantAddMe(CPlayerFriend* pPlayerWantAddMe, std::string& strNotice )
{
	//stMsgPlayerBeAddedFriend msg ;
	//msg.nPlayerUserUID = pPlayerWantAddMe->GetPlayer()->GetUserUID() ;
	//memcpy(msg.pPlayerName,pPlayerWantAddMe->GetPlayer()->GetBaseData()->GetPlayerName(),sizeof(msg.pPlayerName));
	//SendMsg(&msg,sizeof(msg)) ;

	Json::Value jsmsg ;
	jsmsg["reqUID"] = pPlayerWantAddMe->GetPlayer()->GetUserUID() ;
	jsmsg["reqName"] = pPlayerWantAddMe->GetPlayer()->GetBaseData()->GetPlayerName();
	jsmsg["notice"] = strNotice ;
	SendMsg(jsmsg,MSG_RECIEVED_NEW_FRIEND_REQ);
}

void CPlayerFriend::OnOtherReplayMeAboutAddItbeFriend(bool bAgree,CPlayerFriend* pWhoReplyMe)
{
	if ( bAgree )
	{
		AddFriend(pWhoReplyMe->GetPlayer()->GetUserUID()) ;
	}

	Json::Value jsmsg ;
	jsmsg["isAgree"] = bAgree ? 1 : 0 ;
	jsmsg["name"] = pWhoReplyMe->GetPlayer()->GetBaseData()->GetPlayerName();
	jsmsg["targetUID"] = pWhoReplyMe->GetPlayer()->GetUserUID() ;
	SendMsg(jsmsg,MSG_REQ_ADD_FRIEND_RESULT) ;

	//stMsgPlayerAddFriendRet msg ;
	//msg.nRet = 0 ;
	//msg.nTaregtUID = pWhoReplyMe->GetPlayer()->GetUserUID() ;
	//memset(msg.pReplayerName,0,sizeof(msg.pReplayerName));
	//sprintf_s(msg.pReplayerName,sizeof(msg.pReplayerName),"%s",pWhoReplyMe->GetPlayer()->GetBaseData()->GetPlayerName()) ;
	//if ( bAgree == false )
	//{
	//	msg.nRet = 1 ;
	//}
	//else
	//{
	//	if ( IsFriendListFull() )
	//	{
	//		msg.nRet = 2 ;
	//	}
	//	else
	//	{
	//		AddFriend(msg.nTaregtUID) ;
	//	}
	//}

	//SendMsg(&msg,sizeof(msg)) ;
	LOGFMTD("uid = %d send msg stMsgPlayerAddFriendRet",GetPlayer()->GetUserUID());
}

void CPlayerFriend::TimerSave()
{
	//if ( !m_bDirty )
	//{
	//	return ;
	//}
	//m_bDirty = false ;

	//stMsgSaveFirendList saveList ;
	//saveList.nFriendCountLen = m_vAllFriends.size() ;
	//saveList.nUserUID = GetPlayer()->GetUserUID() ;
	//if ( saveList.nFriendCountLen == 0 )
	//{
	//	SendMsg(&saveList,sizeof(saveList)) ;
	//	return ;
	//}

	//std::string strFriends = "";
	//if ( !m_vAllFriends.empty() )
	//{
	//	Json::Value followArray ;
	//	Json::StyledWriter writeFollow ;

	//	FRIENDS_UID::iterator iter = m_vAllFriends.begin() ;
	//	uint16_t nIdx = 0 ;
	//	for ( ; iter != m_vAllFriends.end(); ++iter ,++nIdx)
	//	{
	//		followArray[nIdx] = *iter ;
	//	}
	//	strFriends = writeFollow.write(followArray) ;
	//}

	//saveList.nFriendCountLen = strFriends.size() ;
	//CAutoBuffer auBf ( sizeof(saveList) + saveList.nFriendCountLen ) ;
	//auBf.addContent((char*)&saveList,sizeof(saveList)) ;
	//
	//for ( size_t nValue : m_vAllFriends )
	//{
	//	auBf.addContent((char*)&nValue,sizeof(nValue)) ;
	//}
	//CGameServerApp::SharedGameServerApp()->sendMsg(GetPlayer()->GetUserUID(),auBf.getBufferPtr(),auBf.getContentSize()) ;
	//LOGFMTD("save friend list uid = %d friend cnt = %d",GetPlayer()->GetUserUID(),m_vAllFriends.size());
}

void CPlayerFriend::RemoveFriendByUID(unsigned int nPlayerUID )
{
	FRIENDS_UID::iterator iter = m_vAllFriends.begin() ;
	for ( ; iter != m_vAllFriends.end(); ++iter )
	{
		if ( *iter == nPlayerUID )
		{
			m_vAllFriends.erase(iter) ;
			m_bDirty = true;
			break;
		}
	}

	auto asynQueue = CGameServerApp::SharedGameServerApp()->getAsynReqQueue();
	Json::Value jsReq ;
	std::string strSql = "delete from playerfriend where userUID = '" + std::to_string(GetPlayer()->GetUserUID()) + "' and friendUID = ' "  + std::to_string(nPlayerUID) + " ' ;";
	jsReq["sql"] = strSql;
	asynQueue->pushAsyncRequest(ID_MSG_PORT_DB,eAsync_Db_Delete,jsReq);
}

void CPlayerFriend::AddFriend(unsigned int nFriendUserUID)
{
#ifdef DEBUG
	if ( isPlayerUIDFriend(nFriendUserUID) )
	{
		LOGFMTE("error already friend");
		return ;
	}
#endif
	m_vAllFriends.insert(nFriendUserUID);
	m_bDirty = true;
	auto asynQueue = CGameServerApp::SharedGameServerApp()->getAsynReqQueue();
	Json::Value jsReq ;
	std::string strSql = "insert into playerfriend ( userUID,friendUID ) values ( '" + std::to_string(GetPlayer()->GetUserUID()) + "' , ' "  + std::to_string(nFriendUserUID) + " ') ;";
	jsReq["sql"] = strSql ;
	asynQueue->pushAsyncRequest(ID_MSG_PORT_DB,eAsync_DB_Add,jsReq);
}

void CPlayerFriend::readFriendList()
{
	auto asynQueue = CGameServerApp::SharedGameServerApp()->getAsynReqQueue();
	Json::Value jsReq ;
	std::string strSql = "select friendUID from playerfriend where userUID = '" + std::to_string(GetPlayer()->GetUserUID()) + "' limit 20 offset " + std::to_string(m_vAllFriends.size()) + " ;";
	jsReq["sql"] = strSql ;
	asynQueue->pushAsyncRequest(ID_MSG_PORT_DB,eAsync_DB_Select,jsReq,[this](uint16_t nReqType ,const Json::Value& retContent,Json::Value& jsUserData ){
		Json::Value jsData = retContent["data"] ;
		for ( uint16_t nIdx = 0 ; nIdx < jsData.size() ; ++nIdx )
		{
			auto jsRow = jsData[nIdx] ;
			uint32_t nFriendUID = jsRow["friendUID"].asUInt();
			if ( nFriendUID )
			{
				m_vAllFriends.insert(nFriendUID);
			}
		}

		if ( jsData.size() >= 20 )
		{
			readFriendList() ;
		}
	});
}

bool CPlayerFriend::EventFunc(void* pUserData,stEventArg* pArg)
{
	CPlayerFriend* pF = (CPlayerFriend*)pUserData ;
	pF->OnProcessEvent(pArg) ;
	return false ;
}

void CPlayerFriend::OnProcessEvent(stEventArg* pArg)
{
	if ( pArg == NULL || (pArg->cEvent != eEvent_PlayerOffline && eEvent_PlayerOnline != pArg->cEvent) )
	{
		return ;
	}

	CPlayer* p = (CPlayer*)pArg->pData ;
	
	if ( 0 )
	{
		if ( pArg->cEvent == eEvent_PlayerOnline )
		{
			//pinfo->OnFriendOnLine(p) ;
		}
		else if ( eEvent_PlayerOffline == pArg->cEvent ) 
		{
			//pinfo->OnFriendOffline(p);
		}
	}
}

bool CPlayerFriend::isPlayerUIDFriend(uint32_t nPlayerUID)
{
	FRIENDS_UID::iterator iter = m_vAllFriends.begin() ;
	for ( ; iter != m_vAllFriends.end() ; ++iter )
	{
		if ( *iter == nPlayerUID )
		{
			return true ;
		}
	}
	return false ;
}