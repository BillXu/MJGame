#include "Player.h"
#include "LogManager.h"
#include "PlayerManager.h"
#include "GameServerApp.h"
#include "PlayerBaseData.h"
#include "PlayerMail.h"
#include "PlayerItem.h"
//#include "PlayerMission.h"
#include "Timer.h"
#include "PlayerShop.h"
#include "ServerMessageDefine.h"
#include "RobotManager.h"
#include "PlayerFriend.h"
#include "ServerCommon.h"
#include "AutoBuffer.h"
#include "RoomConfig.h"
#include "PlayerGameData.h"
#include "PlayerBag.h"
#define TIME_SAVE 60*10
#define TIME_DELAY_DELETE 2*60
CPlayer::CPlayer( )
{
	m_nUserUID = 0 ;
	m_eSate = ePlayerState_Online ;
	m_nSessionID = 0 ;
	m_nDisconnectTime = 0 ;
	for ( int i = ePlayerComponent_None; i < ePlayerComponent_Max ; ++i )
	{
		 m_vAllComponents[i] = NULL;
	}

	m_pTimerSave.setCallBack([this](CTimer* p , float fDeta){ this->OnTimerSave(0,0) ;} ) ;
	m_pTimerSave.setInterval(TIME_SAVE) ;
	m_pTimerSave.setIsAutoRepeat(true) ;
}

CPlayer::~CPlayer()
{
	for ( int i = ePlayerComponent_None; i < ePlayerComponent_Max ; ++i )
	{
		IPlayerComponent* p = m_vAllComponents[i] ;
		if ( p )
			delete p ;
		m_vAllComponents[i] = NULL ;
	}

	//m_pTimerSave.canncel() ;
}

void CPlayer::Init(unsigned int nUserUID, unsigned int nSessionID )
{
	m_nSessionID = nSessionID ;
	m_nUserUID = nUserUID ;
	m_eSate = ePlayerState_Online ;
	m_pTimerSave.start() ;
	m_nDisconnectTime = 0 ;
	/// new components ;here ;
	m_vAllComponents[ePlayerComponent_BaseData] = new CPlayerBaseData(this) ;
	m_vAllComponents[ePlayerComponent_Mail] = new CPlayerMailComponent(this);
	m_vAllComponents[ePlayerComponent_PlayerGameData] = new CPlayerGameData(this);
	//m_vAllComponents[ePlayerComponent_PlayerItemMgr] = new CPlayerItemComponent(this);
	//m_vAllComponents[ePlayerComponent_PlayerMission] = new CPlayerMission(this);
	//m_vAllComponents[ePlayerComponent_PlayerShop] = new CPlayerShop(this);
	m_vAllComponents[ePlayerComponent_Friend] = new CPlayerFriend(this);
	m_vAllComponents[ePlayerComponet_Bag] = new CPlayerBag(this);
	for ( int i = ePlayerComponent_None; i < ePlayerComponent_Max ; ++i )
	{
		IPlayerComponent* p = m_vAllComponents[i] ;
		if ( p )
		{
			p->Init();
		}
	}
}

void CPlayer::Reset(unsigned int nUserUID, unsigned int nSessionID )
{
	m_nDisconnectTime = 0 ;
	m_nSessionID = nSessionID ;
	m_nUserUID = nUserUID ;
	m_eSate = ePlayerState_Online ;
	// inform components;
	for ( int i = ePlayerComponent_None; i < ePlayerComponent_Max ; ++i )
	{
		IPlayerComponent* p = m_vAllComponents[i] ;
		if ( p )
		{
			p->Reset();
		}
	}

	m_pTimerSave.reset() ;
	m_pTimerSave.start() ;
}

bool CPlayer::OnMessage( stMsg* pMsg , eMsgPort eSenderPort )
{
	if ( ProcessPublicPlayerMsg(pMsg,eSenderPort) )
	{
		return true; 
	}

	for ( int i = ePlayerComponent_None; i < ePlayerComponent_Max ; ++i )
	{
		IPlayerComponent* p = m_vAllComponents[i] ;
		if ( p )
		{
			if ( p->OnMessage(pMsg,eSenderPort) )
			{
				return true;
			}
		}
	}

	CLogMgr::SharedLogMgr()->ErrorLog("Unprocessed msg id = %d, from = %d  uid = %d",pMsg->usMsgType,eSenderPort,GetUserUID() ) ;

	return false ;

// 	switch ( pMsg->usMsgType )
// 	{
// 	case MSG_PLAYER_CREATE_PRIVATE_ROOM:
// 		{
// 			//stMsgPlayerCreatePrivateRoomRet msgBack ;
// 			//msgBack.nRet = 0 ;
// 			//msgBack.nPassword = 0;
// 			//msgBack.nRoomID = 0 ;
// 			//if ( GetState() != ePlayerState_Free )
// 			//{
// 			//	msgBack.nRet = 1 ;
// 			//	SendMsgToClient((char*)&msgBack,sizeof(msgBack) ) ;
// 			//	return ;
// 			//}
// 
// 			//stMsgPlayerCreatePrivateRoom* pMsgRet = (stMsgPlayerCreatePrivateRoom*)pMsg;
// 			//
// 			//if ( (pMsgRet->bDiamond && GetBaseData()->GetAllDiamoned() < pMsgRet->nOwnMoneyNeedToEnter) || (pMsgRet->bDiamond == false && GetBaseData()->GetAllCoin() < pMsgRet->nOwnMoneyNeedToEnter) )
// 			//{
// 			//	msgBack.nRet = 3 ;
// 			//	SendMsgToClient((char*)&msgBack,sizeof(msgBack) ) ;
// 			//	return ;
// 			//}
// 
// 			//if ( pMsgRet->nBigBinld > pMsgRet->nOwnMoneyNeedToEnter )
// 			//{
// 			//	msgBack.nRet = 4 ;
// 			//	SendMsgToClient((char*)&msgBack,sizeof(msgBack) ) ;
// 			//	return ;
// 			//}
// 
// 			//CLogMgr::SharedLogMgr()->ErrorLog("do you have create private room card ? " ) ;
// 			//CPlayerItemComponent* pItemMgr = (CPlayerItemComponent*)GetComponent(ePlayerComponent_PlayerItemMgr);	
// 			//if ( !pItemMgr->OnUserItem(ITEM_ID_CREATE_ROOM) )
// 			//{
// 			//	msgBack.nRet = 2 ;
// 			//	SendMsgToClient((char*)&msgBack,sizeof(msgBack) ) ;
// 			//	return ;
// 			//}
// 
// 			//CRoomBase* pBase = CGameServerApp::SharedGameServerApp()->GetRoomMgr()->CreateRoom(eRoom_TexasPoker_Private,eRoomLevel_Junior) ;
// 			//if ( !pBase )
// 			//{
// 			//	CLogMgr::SharedLogMgr()->ErrorLog("create room failed") ;
// 			//	msgBack.nRet = 5 ;
// 			//	SendMsgToClient((char*)&msgBack,sizeof(msgBack) ) ;
// 			//	pItemMgr->AddItemByID(ITEM_ID_CREATE_ROOM);
// 			//	return ;
// 			//}
// 			//pBase->SetRoomName(pMsgRet->cRoomName); 
// 			//pBase->SetIsDiamonedRoom(pMsgRet->bDiamond) ;
// 			//pBase->SetPassword(pMsgRet->nPassword) ;
// 			//pBase->SetAntesCoin(pMsgRet->nOwnMoneyNeedToEnter) ;
// 			//((CRoomTexasPoker*)pBase)->SetBigBlindBet(pMsgRet->nBigBinld) ;
// 			//msgBack.nPassword = pMsgRet->nPassword ;
// 			//msgBack.nRoomID = pBase->GetRoomID() ;
// 			//SendMsgToClient((char*)&msgBack,sizeof(msgBack) ) ;
// 
// 			//// after crate success , you default enter the room you create ;
// 			//CRoomPeer* peer = (CTaxasPokerPeer*)GetComponent(ePlayerComponent_RoomPeerTaxasPoker) ;
// 			//pBase->AddPeer(peer) ;
// 			//peer->SetRoom(pBase) ;
// 			//m_eSate = ePlayerState_InRoom ;
// 			//pBase->SendCurRoomInfoToPlayer(peer) ;
// 		}
// 		return ;
// 	case MSG_ROOM_ENTER:
// 		{
// 			stMsgRoomEnterRet msgRet ;
// 			msgRet.nRet = 0 ;
// 			if ( m_eSate == ePlayerState_InRoom )
// 			{
// 				msgRet.nRet = 3 ;   // aready in room ;
// 				SendMsgToClient((char*)&msgRet,sizeof(msgRet)) ;
// 				return ;
// 			}
// 
// 			stMsgRoomEnter* msgEnter = (stMsgRoomEnter*)pMsg ;
// 			CRoomBaseNew* pRoom = CGameServerApp::SharedGameServerApp()->GetRoomMgr()->GetRoom(msgEnter->nRoomType,msgEnter->nRoomLevel,msgEnter->nRoomID) ; 
// 			if ( pRoom == NULL )
// 			{
// 				pRoom = CGameServerApp::SharedGameServerApp()->GetRoomMgr()->CreateRoom(msgEnter->nRoomType,msgEnter->nRoomLevel);
// 			}
// 
// 			if ( pRoom == NULL )
// 			{
// 				msgRet.nRet = 6 ;   // room is NULL;
// 				SendMsgToClient((char*)&msgRet,sizeof(msgRet)) ;
// 				return ;
// 			}
// 
// 			unsigned char nRet = pRoom->CheckCanJoinThisRoom(this);
// 			if ( nRet != 0 )
// 			{
// 				msgRet.nRet = 5 ;   // room is NULL;
// 				SendMsgToClient((char*)&msgRet,sizeof(msgRet)) ;
// 				return ;
// 			}
// 			SendMsgToClient((char*)&msgRet,sizeof(msgRet)) ;
// 			pRoom->Enter(this);
// 			m_pCurRoom = pRoom ;
// 			m_eSate = ePlayerState_InRoom ;
// 		}
// 		return ;
// 	default:
// 		break; 
// 	}

}

bool CPlayer::OnMessage( Json::Value& recvValue , uint16_t nmsgType, eMsgPort eSenderPort  )
{
	for ( int i = ePlayerComponent_None; i < ePlayerComponent_Max ; ++i )
	{
		IPlayerComponent* p = m_vAllComponents[i] ;
		if ( p )
		{
			if ( p->OnMessage(recvValue,nmsgType,eSenderPort) )
			{
				return true;
			}
		}
	}

	CLogMgr::SharedLogMgr()->ErrorLog("Unprocessed msg id = %d, from = %d  uid = %d",nmsgType,eSenderPort,GetUserUID() ) ;

	return false ;
}

void CPlayer::OnPlayerDisconnect()
{
	m_nDisconnectTime = time(NULL) ;
	// inform components;
	for ( int i = ePlayerComponent_None; i < ePlayerComponent_Max ; ++i )
	{
		IPlayerComponent* p = m_vAllComponents[i] ;
		if ( p )
		{
			p->OnPlayerDisconnect();
		}
	}

	OnTimerSave(0,0);

	m_pTimerSave.canncel() ;
	
	SetState(ePlayerState_Offline) ;
	CLogMgr::SharedLogMgr()->ErrorLog("player disconnect should inform other sever");

	// save log 
	stMsgSaveLog msgLog ;
	memset(msgLog.vArg,0,sizeof(msgLog.vArg));
	msgLog.nJsonExtnerLen = 0 ;
	msgLog.nLogType = eLog_PlayerLogOut ;
	msgLog.nTargetID = GetUserUID() ;
	memset(msgLog.vArg,0,sizeof(msgLog.vArg));
	msgLog.vArg[0] = GetBaseData()->getCoin() ;
	msgLog.vArg[1] = GetBaseData()->GetAllDiamoned();
	SendMsgToClient((char*)&msgLog,sizeof(msgLog));
}

void CPlayer::PostPlayerEvent(stPlayerEvetArg* pEventArg )
{
	for ( int i = ePlayerComponent_None; i < ePlayerComponent_Max ; ++i )
	{
		IPlayerComponent* p = m_vAllComponents[i] ;
		if ( p )
		{
			p->OnPlayerEvent(pEventArg);
		}
	}
}

void CPlayer::SendMsgToClient(const char* pBuffer, unsigned short nLen,bool bBrocat )
{
	stMsg* pmsg = (stMsg*)pBuffer ;
	if ( IsState(ePlayerState_Online) || pmsg->cSysIdentifer != ID_MSG_PORT_CLIENT  )
	{
		CGameServerApp::SharedGameServerApp()->sendMsg(GetSessionID(),pBuffer,nLen,bBrocat) ;
		return ;
	}
	CLogMgr::SharedLogMgr()->PrintLog("player uid = %d not online so , can not send msg" ,GetUserUID() ) ;
}

void CPlayer::SendMsgToClient(Json::Value& jsContent , unsigned short nMsgType , bool bBrocast )
{
	if ( IsState(ePlayerState_Online))
	{
		CGameServerApp::SharedGameServerApp()->sendMsg(GetSessionID(),jsContent,nMsgType,ID_MSG_PORT_CLIENT,bBrocast) ;
		return ;
	}
	CLogMgr::SharedLogMgr()->PrintLog("player uid = %d not online so , can not send msg" ,GetUserUID() ) ;
}

bool CPlayer::IsState( ePlayerState eState )
{
	if ( (m_eSate & eState) == eState )
	{
		return true ;
	}
	return false ;
}

void CPlayer::OnAnotherClientLoginThisPeer(unsigned int nSessionID )
{
	// tell prelogin client to disconnect ;
	//stMsgPlayerOtherLogin msg ;
	//SendMsgToClient((char*)&msg,sizeof(msg)) ;

	Json::Value jsMsg ;
	SendMsgToClient(jsMsg,MSG_PLAYER_OTHER_LOGIN,false);

	CLogMgr::SharedLogMgr()->ErrorLog("pls remember inform other server this envent OnAnotherClientLoginThisPeer ") ;

	for ( int i = ePlayerComponent_None; i < ePlayerComponent_Max ; ++i )
	{
		IPlayerComponent* p = m_vAllComponents[i] ;
		if ( p )
		{
			p->OnOtherWillLogined();
		}
	}
	// bind new client ;
	m_nSessionID = nSessionID ;
	for ( int i = ePlayerComponent_None; i < ePlayerComponent_Max ; ++i )
	{
		IPlayerComponent* p = m_vAllComponents[i] ;
		if ( p )
		{
			p->OnOtherDoLogined();
		}
	}
}

bool CPlayer::ProcessPublicPlayerMsg(stMsg* pMsg , eMsgPort eSenderPort)
{
	switch ( pMsg->usMsgType )
	{
// 	case MSG_ROBOT_INFORM_IDLE:
// 		{
// 			CRobotManager::SharedRobotMgr()->AddIdleRobotPlayer(this) ;
// 		}
// 		break;
	case MSG_ADD_MONEY:
		{
			stMsgRobotAddMoney* pAdd = (stMsgRobotAddMoney*)pMsg ;
			GetBaseData()->AddMoney(pAdd->nWantCoin);
			stMsgRobotAddMoneyRet msgBack ;
			msgBack.cRet = 0 ;
			msgBack.nFinalCoin = GetBaseData()->GetAllCoin();
			SendMsgToClient((char*)&msgBack,sizeof(msgBack)) ;

			// save log 
			stMsgSaveLog msgLog ;
			memset(msgLog.vArg,0,sizeof(msgLog.vArg));
			msgLog.nJsonExtnerLen = 0 ;
			msgLog.nLogType = eLog_RobotAddCoin ;
			msgLog.nTargetID = GetUserUID() ;
			memset(msgLog.vArg,0,sizeof(msgLog.vArg));
			msgLog.vArg[0] = pAdd->nWantCoin ;
			SendMsgToClient((char*)&msgLog,sizeof(msgLog));
		}
		break;
// 	case MSG_REQUEST_RANK:
// 		{
// 			stMsgPlayerRequestRank* pMsgRet = (stMsgPlayerRequestRank*)pMsg ;
// 			CGameServerApp::SharedGameServerApp()->GetGameRanker()->SendRankToPlayer(this,(eRankType)pMsgRet->nRankType,pMsgRet->nFromIdx,pMsgRet->nCount ) ;
// 		}
// 		break;
// 	case MSG_REQUEST_RANK_PEER_DETAIL:
// 		{
// 			stMsgPlayerRequestRankPeerDetail* pRetMsg = (stMsgPlayerRequestRankPeerDetail*)pMsg ;
// 			CGameServerApp::SharedGameServerApp()->GetGameRanker()->SendRankDetailToPlayer(this,pRetMsg->nRankPeerUID,(eRankType)pRetMsg->nRankType);
// 		}
// 		break;
// 	case MSG_PLAYER_SAY_BROCAST:
// 		{
// 			stMsgPlayerSayBrocast* pMsgRet = (stMsgPlayerSayBrocast*)pMsg ;
// 			CPlayerItemComponent* pItemMgr = (CPlayerItemComponent*)GetComponent(ePlayerComponent_PlayerItemMgr);	
// 			stMsgPlayerSayBrocastRet msg ;
// 			msg.nRet = 0 ;
// 			if ( pItemMgr->OnUserItem(ITEM_ID_LA_BA) )
// 			{
// 				CGameServerApp::SharedGameServerApp()->GetBrocaster()->PostPlayerSayMsg(this,((char*)pMsg) + sizeof(stMsgPlayerSayBrocast),pMsgRet->nContentLen) ;
// 			}
// 			else
// 			{
// 				msg.nRet = 1 ;
// 				CLogMgr::SharedLogMgr()->ErrorLog(" you have no la ba") ;
// 			}
// 			SendMsgToClient((char*)&msg,sizeof(msg)) ;
// 		}
// 		break;
// 	case MSG_PLAYER_REPLAY_BE_INVITED:
// 		{
// 			//stMsgPlayerRecievedInviteReply toMsgInviter; // who invite me ;
// 			stMsgPlayerReplayBeInvitedToJoinRoom* pMsgRet = (stMsgPlayerReplayBeInvitedToJoinRoom*)pMsg ;
// 			//toMsgInviter.nRet = 0 ;
// 			stMsgPlayerReplayBeInvitedToJoinRoomRet msgBack ;
// 			if ( pMsgRet->nReplyResult == 1 ) // refused 
// 			{
// 				//toMsgInviter.nRet = 1 ;
// 				msgBack.nRet = 0 ;
// 				SendMsgToClient((char*)&msgBack,sizeof(msgBack)) ;
// 				break;
// 			}
// 			else  // i agreed ;
// 			{
// 				msgBack.nRet = 0 ;
// 				CTaxasPokerPeer* pThisPeer = (CTaxasPokerPeer*)GetComponent(ePlayerComponent_RoomPeerTaxasPoker);
// 				CRoomTexasPoker* pRoomToEnter = (CRoomTexasPoker*)CGameServerApp::SharedGameServerApp()->GetRoomMgr()->GetRoom(pMsgRet->nRoomType,pMsgRet->nRoomLevel,pMsgRet->nRoomID) ;
// 				if ( !pRoomToEnter || pRoomToEnter->CanPeerSitDown(pThisPeer) == false )
// 				{
// 					//toMsgInviter.nRet = 4 ;
// 					msgBack.nRet = 2 ;
// 				}
// 				else
// 				{
// 					// join room ;
// 					pThisPeer->LeaveRoom();
// 					if ( pRoomToEnter->AddBeInvitedPlayer(this,pMsgRet->nSitIdx) == false )
// 					{
// 						//toMsgInviter.nRet = 3 ;
// 						msgBack.nRet = 3 ;
// 					}
// 				}
// 
// 				if ( msgBack.nRet != 0 )  // only failed , tell client , when success , directly enter room ;
// 				{
// 					SendMsgToClient((char*)&msgBack,sizeof(msgBack)) ;
// 				}
// 			}
// 			//CPlayer* pToInvid = CGameServerApp::SharedGameServerApp()->GetPlayerMgr()->GetPlayerByUserUID(pMsgRet->nReplyToUserUID) ;
// 			//if ( pToInvid && toMsgInviter.nRet != 0 ) // only failed situation ,tell inviter ;
// 			//{
// 			//	memcpy(toMsgInviter.nReplyerName,GetBaseData()->GetPlayerName(),MAX_LEN_CHARACTER_NAME);
// 			//	pToInvid->SendMsgToClient((char*)&toMsgInviter,sizeof(toMsgInviter)) ;
// 			//}
// 			//else
// 			//{
// 			//	CLogMgr::SharedLogMgr()->PrintLog("the one who invite me had offline , his uid = %d",pMsgRet->nReplyToUserUID) ;
// 			//}
// 		}
// 		break;
// 	case MSG_PLAYER_FOLLOW_TO_ROOM:
// 		{
// 			stMsgPlayerFollowToRoom* pRetMsg = (stMsgPlayerFollowToRoom*)pMsg ;
// 			CPlayer* pTargetPlayer = CGameServerApp::SharedGameServerApp()->GetPlayerMgr()->GetPlayerByUserUID(pRetMsg->nTargetPlayerUID) ;
// 			stMsgPlayerFollowToRoomRet msgBack ;
// 			msgBack.nRet = 0 ;
// 			if ( pTargetPlayer == NULL )
// 			{
// 				msgBack.nRet = 1 ;
// 				SendMsgToClient((char*)&msgBack,sizeof(msgBack)) ;
// 				break ;
// 			}
// 
// 			if ( ePlayerState_Free == pTargetPlayer->GetState() )
// 			{
// 				msgBack.nRet = 2 ;
// 				SendMsgToClient((char*)&msgBack,sizeof(msgBack)) ;
// 				break ;
// 			}
// 
// 			if ( ePlayerState_Free != GetState() )
// 			{
// 				msgBack.nRet = 4 ;
// 				SendMsgToClient((char*)&msgBack,sizeof(msgBack)) ;
// 				break ;
// 			}
// 
// 			CRoomBaseNew* pStateRoom = pTargetPlayer->GetRoomCurStateIn() ;
// 			if ( !pStateRoom )
// 			{
// 				CLogMgr::SharedLogMgr()->ErrorLog("follow to a null room , but target player is not free , how , why ?") ;
// 				msgBack.nRet = 2 ;
// 				SendMsgToClient((char*)&msgBack,sizeof(msgBack)) ;
// 				break;
// 			}
// 
// 
// 			if ( pStateRoom->CheckCanJoinThisRoom(pTargetPlayer) != 0)
// 			{
// 				msgBack.nRet = 3 ;
// 				SendMsgToClient((char*)&msgBack,sizeof(msgBack)) ;
// 				break;
// 			}
// 
// 			// add to room 
// 			stMsgRoomEnter msgToEnterRoom ;
// 			msgToEnterRoom.nPassword = 0 ;
// 			msgToEnterRoom.nRoomID = pStateRoom->GetRoomID();
// 			msgToEnterRoom.nRoomLevel = pStateRoom->GetRoomLevel() ;
// 			msgToEnterRoom.nRoomType = pStateRoom->GetRoomType() ;
// 			OnMessage(&msgToEnterRoom);
// 		}
// 		break;
// 	case MSG_PLAYER_SLOT_MACHINE:
// 		{
// 			stMsgPlayerSlotMachineRet msgBack ;
// 			msgBack.nRet = 0 ;
// 			stMsgPlayerSlotMachine* pRetMsg = (stMsgPlayerSlotMachine*)pMsg ;
// 			if ( GetBaseData()->GetAllCoin() < pRetMsg->nBetCoin || GetBaseData()->GetAllDiamoned() < pRetMsg->nBetDiamoned )
// 			{
// 				msgBack.nRet = 1 ;
// 			}
// 			else
// 			{
// 				CSlotMachine* pMachine = (CSlotMachine*)CGameServerApp::SharedGameServerApp()->GetConfigMgr()->GetConfig(CConfigManager::eConfig_SlotMachine) ;
// 				float fRate = 0 ;
// 				pMachine->RandSlotMachine(pRetMsg->cLevel,msgBack.vCard,fRate) ;
// 				int64_t nOffsetCoin = pRetMsg->nBetCoin * ( fRate - 1.0 );
// 				int nOffsetDiamoned = pRetMsg->nBetDiamoned * ( fRate - 1.0 );
// 				nOffsetDiamoned = abs(nOffsetDiamoned) ;
// 				nOffsetCoin = abs(nOffsetCoin) ;
// 				int nOffset = fRate > 1 ? 1 : -1 ;
// 				GetBaseData()->ModifyMoney(nOffsetCoin * nOffset);
// 				GetBaseData()->ModifyMoney(nOffsetDiamoned * nOffset,true);				
// 				msgBack.nFinalAllCoin = GetBaseData()->GetAllCoin();
// 				msgBack.nFinalDiamoned = GetBaseData()->GetAllDiamoned() ;
// 				msgBack.nTakeInCoin = GetBaseData()->GetTakeInMoney() ;
// 				msgBack.nTakeInDiamoned = GetBaseData()->GetTakeInMoney(true) ;
// 				SendMsgToClient((char*)&msgBack,sizeof(msgBack)) ;
// 			}
// 		}
// 		break;
// 	case MSG_ROOM_REQUEST_PEER_DETAIL:
// 		{
// 			stMsgRoomRequestPeerDetailRet msgBack ;
// 			stMsgRoomRequestPeerDetail* pMsgRet = (stMsgRoomRequestPeerDetail*)pMsg ;
// 			msgBack.nPeerSessionID = pMsgRet->nPeerSessionID ;
// 			CPlayer* pDetailPlayer = CGameServerApp::SharedGameServerApp()->GetPlayerMgr()->GetPlayerBySessionID(pMsgRet->nPeerSessionID) ;
// 			if ( pDetailPlayer == NULL )
// 			{
// 				msgBack.nRet = 1 ;
// 				SendMsgToClient((char*)&msgBack,sizeof(msgBack)) ;
// 				return true ;
// 			}
// 
// 			pDetailPlayer->GetBaseData()->GetPlayerDetailData(&msgBack.stDetailInfo);
// 			SendMsgToClient((char*)&msgBack,sizeof(msgBack)) ;
// 		}
// 		break;
 	default:
 		return false ;
	}
	return true ;
}

void CPlayer::OnTimerSave(float fTimeElaps,unsigned int nTimerID )
{
	for ( int i = ePlayerComponent_None; i < ePlayerComponent_Max ; ++i )
	{
		IPlayerComponent* p = m_vAllComponents[i] ;
		if ( p )
		{
			p->TimerSave();
		}
	}
}

void CPlayer::PushTestAPNs()
{
#ifdef NDEBUG
	return ;
#endif
	//if ( GetBaseData()->bPlayerEnableAPNs == false )
	//{
	//	CLogMgr::SharedLogMgr()->ErrorLog("you not enable apns ") ;
	//	return ;
	//}
	//char* pString = "\"you disconnected \"" ;
	//stMsgToAPNSServer msg ;
	//msg.nAlertLen = strlen(pString) ;
	//msg.nBadge = 1 ;
	//msg.nSoundLen = 0 ;
	//memcpy(msg.pDeveiceToken,GetBaseData()->vAPNSToken,32);
	//char* pBuffer = new char[sizeof(msg) + msg.nAlertLen ] ;
	//unsigned short nOffset = 0 ;
	//memcpy(pBuffer,&msg,sizeof(msg));
	//nOffset += sizeof(msg);
	//memcpy(pBuffer + nOffset , pString ,msg.nAlertLen);
	//nOffset += msg.nAlertLen ;
	//CGameServerApp::SharedGameServerApp()->SendMsgToAPNsServer(pBuffer,nOffset);
	//delete[] pBuffer ;
}

void CPlayer::OnReactive(uint32_t nSessionID )
{
	CLogMgr::SharedLogMgr()->PrintLog("uid = %d reactive with session id = %d", GetUserUID(), nSessionID) ;
	m_nSessionID = nSessionID ;
	SetState(ePlayerState_Online) ;
	m_nDisconnectTime = 0 ;
	
	m_pTimerSave.reset();
	m_pTimerSave.start() ;


	for ( int i = ePlayerComponent_None; i < ePlayerComponent_Max ; ++i )
	{
		IPlayerComponent* p = m_vAllComponents[i] ;
		if ( p )
		{
			p->OnReactive(nSessionID);
		}
	}
}

bool CPlayer::onCrossServerRequest(stMsgCrossServerRequest* pRequest , eMsgPort eSenderPort,Json::Value* vJsValue)
{
	for ( int i = ePlayerComponent_None; i < ePlayerComponent_Max ; ++i )
	{
		IPlayerComponent* p = m_vAllComponents[i] ;
		if ( p && p->onCrossServerRequest(pRequest,eSenderPort,vJsValue) )
		{
			return true ;
		}
	}
	return false ;
}

bool CPlayer::onCrossServerRequestRet(stMsgCrossServerRequestRet* pResult,Json::Value* vJsValue)
{
	for ( int i = ePlayerComponent_None; i < ePlayerComponent_Max ; ++i )
	{
		IPlayerComponent* p = m_vAllComponents[i] ;
		if ( p && p->onCrossServerRequestRet(pResult,vJsValue) )
		{
			return true;
		}
	}
	return false ;
}


uint8_t CPlayer::getMsgPortByRoomType(uint8_t nType )
{
	switch ( nType )
	{
	case eRoom_MJ:
		return ID_MSG_PORT_MJ ;
	case eRoom_TexasPoker:
		return ID_MSG_PORT_TAXAS ;
	case eRoom_Golden:
		return ID_MSG_PORT_GOLDEN;
	default:
		return ID_MSG_PORT_NONE ;
	}

	return ID_MSG_PORT_NONE ;
}

void CPlayer::delayDelete()
{ 
	m_nDisconnectTime = time(nullptr) + TIME_DELAY_DELETE ;
	CLogMgr::SharedLogMgr()->PrintLog("uid = %d delay delete player object" , GetUserUID() ) ;
}


