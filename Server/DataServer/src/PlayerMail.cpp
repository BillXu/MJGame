#include "PlayerMail.h"
#include "ServerMessageDefine.h"
#include "log4z.h"
#include "Player.h"
#include <time.h>
#include "PlayerFriend.h"
#include "ShopConfg.h"
#include "PlayerBaseData.h"
#include "PlayerItem.h"
#include "GameServerApp.h"
#include "ConfigManager.h"
#include "PlayerEvent.h"
#include "AutoBuffer.h"
#include <cassert>
#define  MAX_KEEP_MAIL_CNT 20

bool arrageMailByTime( CPlayerMailComponent::stRecievedMail left , CPlayerMailComponent::stRecievedMail right )
{
	if ( left.nRecvTime < right.nRecvTime ) 
	{
		return true ;
	}

	if ( left.nRecvTime > right.nRecvTime )
	{
		return false ;
	}
	return false ;
}

CPlayerMailComponent::LIST_MAIL CPlayerMailComponent::s_vPublicMails ;
bool CPlayerMailComponent::s_isReadedPublic = false ;
bool CPlayerMailComponent::OnMessage( stMsg* pMsg , eMsgPort eSenderPort )
{
	if ( IPlayerComponent::OnMessage(pMsg,eSenderPort) )
	{
		return true ;
	}

	switch ( pMsg->usMsgType )
	{
	case MSG_PLAYER_READ_MAIL_LIST:
		{
			stMsgReadMailListRet* pMsgRet = (stMsgReadMailListRet*)pMsg ;
			stRecievedMail sMail ;
			sMail.eType = (eMailType)pMsgRet->pMails.eType ;
			sMail.nRecvTime = pMsgRet->pMails.nPostTime ;
			CAutoBuffer auf (pMsgRet->pMails.nContentLen + 1 );
			auf.addContent((char*)pMsg + sizeof(stMsgReadMailListRet),pMsgRet->pMails.nContentLen ) ;
			sMail.strContent = auf.getBufferPtr() ;

			if ( pMsgRet->nUserUID == 0 )
			{
				s_vPublicMails.push_back(sMail) ;
				if ( pMsgRet->bFinal )
				{
					s_vPublicMails.sort(arrageMailByTime);
				}
				return true  ;
			}

			m_vAllMail.push_back(sMail);

			if ( pMsgRet->bFinal )
			{
				// if have unread mail should tell client ;
				ProcessSpecailMail();
				stPlayerEvetArg arg ;
				arg.eEventType = ePlayerEvent_ReadDBOK;
				GetPlayer()->PostPlayerEvent(&arg);
 				InformRecievedUnreadMails();
				LOGFMTD("read mail finish uid = %d, mail size = %d",GetPlayer()->GetUserUID(),m_vAllMail.size() );
				if ( m_vAllMail.size() >= 2 )
				{
					m_vAllMail.sort(arrageMailByTime);
				}
			}

		}
		break;
	case MSG_PLAYER_REQUEST_MAIL_LIST:
		{
			SendMailListToClient();
			return true ;
		}
		break;
	case MSG_PLAYER_ADVICE:
		{
			stMsgPlayerAdviceRet msgRet ;
			msgRet.nRet = 0 ;
			stMsgPlayerAdvice* pRet = (stMsgPlayerAdvice*)pMsg ;
			if ( pRet->nLen > 512 || pRet->nLen <= 7 )
			{
				LOGFMTE("uid = %d advice too long len = %d",GetPlayer()->GetUserUID(),pRet->nLen) ;
				msgRet.nRet = 1; 
			}
			SendMsg(&msgRet,sizeof(msgRet)) ;

			if ( msgRet.nRet )
			{
				return true ;
			}

			stMsgSavePlayerAdvice msgSaveAdVice ;
			msgSaveAdVice.nUserUID = GetPlayer()->GetUserUID() ;
			msgSaveAdVice.nLen = pRet->nLen ;

			CAutoBuffer buff(msgSaveAdVice.nLen + sizeof(msgSaveAdVice));
			buff.addContent(&msgSaveAdVice,sizeof(msgSaveAdVice)) ;
			buff.addContent(((char*)pMsg)+ sizeof(stMsgPlayerAdvice) ,pRet->nLen ) ;
			SendMsg((stMsg*)buff.getBufferPtr(),buff.getContentSize()) ;
			LOGFMTD("recived uid = %d advice",GetPlayer()->GetUserUID());
		}
		break;
	default:
		return false ;
	}
	return true ;
}

bool CPlayerMailComponent::OnMessage( Json::Value& recvValue , uint16_t nmsgType, eMsgPort eSenderPort)
{
	switch (nmsgType)
	{
	case MSG_REQUEST_NEW_MAIL_LIST:
		{
			SendMailListToClient();
		}
		break ;
	default:
		return false;
	}
	return true ;
}

void CPlayerMailComponent::Reset()
{
	ClearMails();
	stMsgReadMailList msg ;
	msg.nUserUID = GetPlayer()->GetUserUID() ;
	SendMsg(&msg,sizeof(msg)) ;

	if ( s_isReadedPublic == false )
	{
		s_isReadedPublic = true ;
		stMsgReadMailList msgpublic ;
		msgpublic.nUserUID = 0 ;
		SendMsg(&msgpublic,sizeof(msgpublic));
	}
}

void CPlayerMailComponent::Init()
{
	Reset();
}

void CPlayerMailComponent::OnOtherDoLogined()
{
	// send unread mail count to client ;
	InformRecievedUnreadMails();
}

void CPlayerMailComponent::InformRecievedUnreadMails()
{
	stMsgInformNewMail msg ;
	msg.nUnreadMailCount = m_vAllMail.size() + getPublicMailsByTime(m_tReadTimeTag) ;
	msg.nUnreadMailCount = msg.nUnreadMailCount < MAX_KEEP_MAIL_CNT ? msg.nUnreadMailCount : MAX_KEEP_MAIL_CNT ;

	if ( msg.nUnreadMailCount )
	{
		//SendMsg(&msg,sizeof(msg)) ;
		Json::Value jsmsg ;
		jsmsg["newMailCnt"] = msg.nUnreadMailCount ;
		SendMsg(jsmsg,MSG_RECIEVED_NEW_MAIL);
	}
}

void CPlayerMailComponent::OnReactive(uint32_t nSessionID )
{
	IPlayerComponent::OnReactive(nSessionID) ;
	InformRecievedUnreadMails();
}

void CPlayerMailComponent::ClearMails()
{
	m_vAllMail.clear();
}

void CPlayerMailComponent::SendMailListToClient()
{
	LIST_MAIL vSendMailList ;
	vSendMailList.assign(m_vAllMail.begin(),m_vAllMail.end()) ;
	if ( vSendMailList.size() < MAX_KEEP_MAIL_CNT )
	{
		getPublicMailsByTime(m_tReadTimeTag,&vSendMailList,MAX_KEEP_MAIL_CNT - vSendMailList.size() );
	}
	
	if ( vSendMailList.empty() )
	{
		return ;
	}

	vSendMailList.sort(arrageMailByTime);

	Json::Value jsmsg ;
	Json::Value jsmails ;
	for ( auto ref : vSendMailList )
	{
		Json::Value jsmail ;
		jsmail["type"] = ref.eType ;
		jsmail["content"] = ref.strContent ;
		jsmail["recvTime"] = ref.nRecvTime ;
		jsmails[jsmails.size()] = jsmail ;
	}
	jsmsg["mails"] = jsmails ;
	SendMsg(jsmsg,MSG_REQUEST_NEW_MAIL_LIST);

	//stMsgRequestMailListRet msgRet;
	//uint8_t nSize = vSendMailList.size() ;
	//CAutoBuffer auBuff(sizeof(msgRet) + 100 );
	//for ( stRecievedMail& pMail : vSendMailList )
	//{
	//	--nSize ;
	//	msgRet.isFinal = nSize == 0 ;
	//	msgRet.tMail.eType = pMail.eType ;
	//	msgRet.tMail.nPostTime = pMail.nRecvTime ;
	//	msgRet.tMail.nContentLen = pMail.strContent.size() ;
	//	if ( pMail.eType <= eMail_RealMail_Begin )
	//	{
	//		LOGFMTE("send sys process mail to player uid = %u , type = %u",GetPlayer()->GetUserUID(),pMail.eType);
	//		continue;
	//	}
	//	auBuff.clearBuffer();
	//	auBuff.addContent(&msgRet,sizeof(msgRet));
	//	auBuff.addContent(pMail.strContent.c_str(),msgRet.tMail.nContentLen) ;
	//	SendMsg((stMsg*)auBuff.getBufferPtr(),auBuff.getContentSize()) ;
	//}
	LOGFMTD("send mail to client uid = %d ,size = %d",GetPlayer()->GetUserUID(),vSendMailList.size() ) ;

	saveReadTimeTag();
}

void CPlayerMailComponent::PostMailToPlayer( eMailType eType ,const char* pContent, uint16_t nContentLen ,uint32_t nTargetUID )
{
	auto pPlayer = CGameServerApp::SharedGameServerApp()->GetPlayerMgr()->GetPlayerByUserUID(nTargetUID) ;
	if ( pPlayer )
	{
		if ( eType == eMail_SysOfflineEvent )
		{
			LOGFMTE("uid = %u online why send offline event",nTargetUID) ;
			assert(eType != eMail_SysOfflineEvent && "player online why send offline event" );
			return ;
		}

		auto pM = (CPlayerMailComponent*)pPlayer->GetComponent(ePlayerComponent_Mail);
		pM->ReciveMail(eType,pContent,nContentLen);
	}
	else
	{
		stMsgSaveMail msgSave ;
		msgSave.nUserUID = nTargetUID ;
		msgSave.pMailToSave.eType = eType ;
		msgSave.pMailToSave.nContentLen = nContentLen ;
		msgSave.pMailToSave.nPostTime = time(nullptr);
		CAutoBuffer auBuffer(sizeof(msgSave) + nContentLen );
		auBuffer.addContent(&msgSave,sizeof(msgSave)) ;
		auBuffer.addContent(pContent,nContentLen) ;
		CGameServerApp::SharedGameServerApp()->sendMsg(nTargetUID,auBuffer.getBufferPtr(),auBuffer.getContentSize()) ;
		LOGFMTD("mail type = %u to player uid = %u  not onlien send to db",eType,nTargetUID);
	}
}

void CPlayerMailComponent::PostOfflineEvent( eOfflineEvent eEvntType ,Json::Value& pEventArg ,uint32_t nTargetUID )
{
	Json::Value vValue ;
	vValue["event"] = eEvntType ;
	vValue["arg"] = pEventArg ;
	
	Json::StyledWriter jWrite ;
	std::string strConetnt = jWrite.write(vValue) ;

	PostMailToPlayer(eMail_SysOfflineEvent,strConetnt.c_str(),strConetnt.size(),nTargetUID);
	LOGFMTD("post offline event to uid = %d , event = %d",nTargetUID,eEvntType) ;
}

void CPlayerMailComponent::PostDlgNotice( eNoticeType eNotice ,Json::Value& pEventArg ,uint32_t nTargetUID )
{
	auto pPlayer = CGameServerApp::SharedGameServerApp()->GetPlayerMgr()->GetPlayerByUserUID(nTargetUID) ;
	if ( pPlayer )
	{
		Json::StyledWriter jWrite ;
		std::string strConetnt = jWrite.write(pEventArg) ;
		 // send dlg msg ;
		stMsgDlgNotice msg;
		msg.nNoticeType = eNotice ;
		msg.nJsonLen = strConetnt.size() ;
		CAutoBuffer msgBuffer(sizeof(msg) + msg.nJsonLen );
		msgBuffer.addContent(&msg,sizeof(msg)) ;
		msgBuffer.addContent(strConetnt.c_str(),msg.nJsonLen) ;
		pPlayer->SendMsgToClient(msgBuffer.getBufferPtr(),msgBuffer.getContentSize()) ;
		LOGFMTD("uid = %u online show dlg notice",nTargetUID);
	}
	else
	{
		Json::Value vValue ;
		vValue["noticeType"] = eNotice ;
		vValue["notice"] = pEventArg ;
		Json::StyledWriter jWrite ;
		std::string strConetnt = jWrite.write(vValue) ;

		PostMailToPlayer(eMail_DlgNotice,strConetnt.c_str(),strConetnt.size(),nTargetUID);
		LOGFMTD("uid = %u not online dlg to db",nTargetUID);
	}
	LOGFMTD("post dlg notice event to uid = %d , dlg type = %d",nTargetUID,eNotice) ;
}

void CPlayerMailComponent::ReciveMail(eMailType eType ,const char* pContent, uint16_t nContentLen )
{
	stMsgSaveMail msgSave ;
	msgSave.nUserUID = GetPlayer()->GetUserUID() ;
	msgSave.pMailToSave.eType = eType ;
	msgSave.pMailToSave.nContentLen = nContentLen ;
	msgSave.pMailToSave.nPostTime = time(nullptr) ;

	CAutoBuffer auBuffer(sizeof(msgSave) + msgSave.pMailToSave.nContentLen );
	auBuffer.addContent(&msgSave,sizeof(msgSave)) ;
	auBuffer.addContent(pContent,msgSave.pMailToSave.nContentLen) ;
	CGameServerApp::SharedGameServerApp()->sendMsg(msgSave.nUserUID,auBuffer.getBufferPtr(),auBuffer.getContentSize()) ;

	stRecievedMail refMail ;
	refMail.eType = eType ;
	refMail.nRecvTime = msgSave.pMailToSave.nPostTime;
	refMail.strContent = pContent ;
	m_vAllMail.push_back(refMail);
	InformRecievedUnreadMails();
	if ( m_vAllMail.size() > MAX_KEEP_MAIL_CNT )
	{
		m_vAllMail.erase(m_vAllMail.begin()) ;
	}
}

void CPlayerMailComponent::PostPublicMail(stRecievedMail& pMail)
{
	s_vPublicMails.push_back(pMail);
	if ( s_vPublicMails.size() > MAX_KEEP_MAIL_CNT )
	{
		s_vPublicMails.erase(s_vPublicMails.begin()) ;
	}

	stMsgSaveMail msgSave ;
	msgSave.nUserUID = 0 ;
	msgSave.pMailToSave.eType = pMail.eType ;
	msgSave.pMailToSave.nContentLen = pMail.strContent.size() ;
	msgSave.pMailToSave.nPostTime = pMail.nRecvTime ;

	CAutoBuffer auBuffer(sizeof(msgSave) + msgSave.pMailToSave.nContentLen );
	auBuffer.addContent(&msgSave,sizeof(msgSave)) ;
	auBuffer.addContent(pMail.strContent.c_str(),msgSave.pMailToSave.nContentLen) ;
	CGameServerApp::SharedGameServerApp()->sendMsg(msgSave.nUserUID,auBuffer.getBufferPtr(),auBuffer.getContentSize()) ;
	LOGFMTD("save public mail") ;
}

uint16_t CPlayerMailComponent::getPublicMailsByTime( uint32_t nTimeTag, LIST_MAIL* vOutMail,uint16_t nMaxOutCnt )
{
	LIST_MAIL::reverse_iterator iter = s_vPublicMails.rbegin() ;
	uint16_t nGetCnt = 0 ;
	while ( iter != s_vPublicMails.rend() )
	{
		if ( nGetCnt >= nMaxOutCnt )
		{
			break; 
		}

		stRecievedMail& pRef = *iter ;
		if ( pRef.nRecvTime < nTimeTag )
		{
			break;
		}
		++nGetCnt;
		if ( vOutMail )
		{
			vOutMail->push_back(pRef) ;
		}
	}
	return nGetCnt ;
}

void CPlayerMailComponent::ProcessSpecailMail()
{
	LIST_MAIL vNeedProcess ;
	LIST_MAIL vNormal ;
	vNeedProcess.clear(); vNormal.clear() ;
	auto iter = m_vAllMail.begin();
	for ( ; iter != m_vAllMail.end(); ++iter )
	{
		auto mail = *iter ;
		if ( mail.eType >= eMail_RealMail_Begin )
		{
			vNormal.push_back(mail) ;
		}
		else
		{
			vNeedProcess.push_back(mail);
		}
	}
	m_vAllMail.clear() ;
	m_vAllMail.swap(vNormal);
	vNormal.clear();

	if ( vNeedProcess.empty() )
	{
		return ;
	}
	// process sys mail 
	for ( stRecievedMail& refMail : vNeedProcess )
	{
		ProcessMail(refMail);
	}

	stMsgResetMailsState msgReset ;
	msgReset.nUserUID = GetPlayer()->GetUserUID() ;
	msgReset.eType = eMail_Sys_End ;
	SendMsg(&msgReset,sizeof(msgReset)) ;
}

bool CPlayerMailComponent::ProcessMail( stRecievedMail& pMail)
{
	if ( pMail.eType > eMail_Sys_End )
	{
		return false ;
	}

	switch ( pMail.eType)
	{
	case eMail_ReadTimeTag:
		{
			m_tReadTimeTag = pMail.nRecvTime ;
		}
		break;
	case eMail_SysOfflineEvent:
		{
			processSysOfflineEvent(pMail);
		}
		break;
	case eMail_AddRoomCard:
	{
		Json::Value vRoot;
		Json::Reader reader;
		bool bRet = reader.parse(pMail.strContent, vRoot);
		if (!bRet)
		{
			LOGFMTE("uid = %u parse add Room card mail error ", GetPlayer()->GetUserUID());
			break;
		}

		int32_t nAddLeft = vRoot["addCard"].asInt();
		int32_t nAddCoin = vRoot["addCoin"].asInt();
		int32_t nAddDiamond = vRoot["addDiamond"].asInt();

		auto nAddNo = vRoot["addCardNo"].asUInt();
		GetPlayer()->GetBaseData()->addVipRoomCard(nAddLeft);
		GetPlayer()->GetBaseData()->AddMoney(nAddDiamond, true);
		GetPlayer()->GetBaseData()->AddMoney(nAddCoin, false);

		LOGFMTI("uid = %u add room card mail cnt = %d addCardNo = %u", GetPlayer()->GetUserUID(), nAddLeft, nAddNo);
	}
	break;
	case eMail_DlgNotice:
		{
			Json::Value vRoot ;
			Json::Reader reader ;
			reader.parse(pMail.strContent,vRoot);
			Json::Value nNotice = vRoot["notice"] ;

			Json::StyledWriter sWrite ;
			std::string strConetnt = sWrite.write(nNotice) ;
			// send dlg msg ;
			stMsgDlgNotice msg;
			msg.nNoticeType = vRoot["noticeType"].asInt() ;
			msg.nJsonLen = strConetnt.size() ;
			CAutoBuffer msgBuffer(sizeof(msg) + msg.nJsonLen );
			msgBuffer.addContent(&msg,sizeof(msg)) ;
			msgBuffer.addContent(strConetnt.c_str(),msg.nJsonLen) ;
			GetPlayer()->SendMsgToClient(msgBuffer.getBufferPtr(),msgBuffer.getContentSize()) ;
			LOGFMTD("uid = %u login on show offline dlg notice",GetPlayer()->GetUserUID());
		}
		break;
	default:
		LOGFMTE("unknown sys mail type = %u , for user uid = %u",pMail.eType,GetPlayer()->GetUserUID());
		break;
	}

	return true ;
}

void CPlayerMailComponent::processSysOfflineEvent(stRecievedMail& pMail)
{
	Json::Value vRoot ;
	Json::Reader reader ;
	reader.parse(pMail.strContent,vRoot);
	eOfflineEvent eEvent = (eOfflineEvent)(vRoot["event"].asInt());
	Json::Value jArg = vRoot["arg"] ;
	switch ( eEvent )
	{
	case Event_Reward:
		{
			uint16_t nRewardID = jArg["rewardID"].asInt() ;
			uint16_t nGameType = jArg["gameType"].asInt()  ;

			const char* nRoomID = "null name"  ;
			if ( jArg["roomName"].isNull() == false )
			{
				nRoomID = jArg["roomName"].asCString()  ;
			}
			uint16_t nRankIdx = jArg["rankIdx"].asInt()  ;
			GetPlayer()->GetBaseData()->onGetReward(nRankIdx,nRewardID,nGameType,nRoomID) ;
			LOGFMTD("do give reward uid = %u, nreward id = %u",GetPlayer()->GetUserUID(),nRewardID) ;
		}
		break;
	case Event_AddCoin:
		{
			if ( strcmp(jArg["comment"].asCString(),"invitePrize") == 0 )
			{
				GetPlayer()->GetBaseData()->addInvitePrize(jArg["addCoin"].asInt());
				LOGFMTD("uid = %u add invite prize = %u",GetPlayer()->GetUserUID(),jArg["addCoin"].asInt()) ;
			}
			else
			{
				GetPlayer()->GetBaseData()->AddMoney(jArg["addCoin"].asInt());
			}
			LOGFMTD("do give coin uid = %d , coin = %d comment = %s",GetPlayer()->GetUserUID(),jArg["addCoin"].asInt(),jArg["comment"].asCString()) ;
		}
		break;
	case Event_AddFriend:
		{
			uint32_t nTargetUID = jArg["friendUID"].asUInt() ;
			auto pF = (CPlayerFriend*)GetPlayer()->GetComponent(ePlayerComponent_Friend);
			pF->AddFriend(nTargetUID) ;
			LOGFMTD("offline event add friend uid = %u",nTargetUID) ;
		}
		break;
	case Event_DelteFriend:
		{
			uint32_t nTargetUID = jArg["friendUID"].asUInt() ;
			auto pF = (CPlayerFriend*)GetPlayer()->GetComponent(ePlayerComponent_Friend);
			pF->RemoveFriendByUID(nTargetUID) ;
			LOGFMTD("offline event delete friend uid = %u",nTargetUID) ;
		}
		break ;
	default:
		LOGFMTE("unknown event type from offline , %d  uid = %d ",eEvent,GetPlayer()->GetUserUID()) ;
		break;
	}
	LOGFMTE("process offline event here");
}

void CPlayerMailComponent::saveReadTimeTag()
{
	// tell db set state 
	stMsgResetMailsState msgReset ;
	msgReset.nUserUID = GetPlayer()->GetUserUID() ;
	msgReset.eType = eMail_RealMail_Begin ;
	SendMsg(&msgReset,sizeof(msgReset)) ;
	m_tReadTimeTag = time(nullptr) ;

	m_vAllMail.clear() ;

	stMsgSaveMail msgSave ;
	msgSave.nUserUID = GetPlayer()->GetUserUID();
	msgSave.pMailToSave.eType = eMail_ReadTimeTag ;
	msgSave.pMailToSave.nContentLen = 0 ;
	msgSave.pMailToSave.nPostTime = m_tReadTimeTag ;
	SendMsg(&msgSave,sizeof(msgSave)) ;
	LOGFMTD("uid = %u save read mail tag", GetPlayer()->GetUserUID() );
}
