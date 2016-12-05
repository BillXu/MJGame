#include "PlayerGameData.h"
#include <sstream>
#include "Player.h"
#include "log4z.h"
#include "GameServerApp.h"
#include "PlayerBaseData.h"
#include <json/json.h>
#include "AutoBuffer.h"
#include "TaxasPokerPeerCard.h"
#include "RoomConfig.h"
#include "AsyncRequestQuene.h"
#include "RobotCenter.h"
void CPlayerGameData::Reset()
{
	IPlayerComponent::Reset();
	m_nStateInRoomID = 0;
	m_ePlayerGameState = ePlayerGameState_NotIn;
	//m_nSubRoomIdx = 0 ;
	memset(&m_vData,0,sizeof(m_vData));
	for (auto& r : m_vMyOwnRooms )
	{
		r.clear() ;
	}
	m_bIsCreating = false ;

	// read player game recorder 
	char pBuffer[256] = { 0 } ;
	Json::Value jsReq ;
	sprintf_s(pBuffer,sizeof(pBuffer),"select * from playergamerecoder where userUID = %u ;",GetPlayer()->GetUserUID()) ;
	jsReq["sql"] = pBuffer ;
	auto pAsq = CGameServerApp::SharedGameServerApp()->getAsynReqQueue();
	pAsq->pushAsyncRequest(ID_MSG_PORT_DB,eAsync_DB_Select,jsReq,[this]( uint16_t nReqType ,const Json::Value& retContent,Json::Value& jsUserData){
		uint16_t nfact = retContent["afctRow"].asUInt() ;
		auto jsData = retContent["data"] ;
		for ( uint16_t nIdx = 0 ; nIdx < jsData.size() ; ++nIdx )
		{
			auto jsRow = jsData[nIdx] ;
			eRoomType eType = (eRoomType)jsRow["gameType"].asUInt();
			if ( eType >= eRoom_MJ_MAX )
			{
				LOGFMTE("invalid game type = %u",eType) ;
				continue;
			}

			auto& ref = m_vData[eType];
			ref.bDirty = false ;
			ref.nGameType = eType ;
			ref.nMaxFangXingType = jsRow["maxWinCardType"].asUInt();
			ref.nMaxFanShu = jsRow["maxWinTimes"].asUInt() ;
			ref.nRoundsPlayed = jsRow["roundsPlayed"].asUInt() ;
		}
	}) ;


	// read bill ids 
	std::ostringstream ss ;
	ss << "select billID from playerbillids where userUID = " << GetPlayer()->GetUserUID() << " order by billID desc limit 10 ;" ;
	Json::Value jsReadBillIDs ;
	jsReadBillIDs["sql"] = ss.str() ;
	pAsq->pushAsyncRequest(ID_MSG_PORT_DB,eAsync_DB_Select,jsReadBillIDs,[this]( uint16_t nReqType ,const Json::Value& retContent,Json::Value& jsUserData){
		uint16_t nfact = retContent["afctRow"].asUInt() ;
		auto jsData = retContent["data"] ;
		for ( uint16_t nIdx = 0 ; nIdx < jsData.size() ; ++nIdx )
		{
			auto jsRow = jsData[nIdx] ;
			uint32_t nBillID = jsRow["billID"].asUInt();
			m_vVipBillIDs.push_back(nBillID) ;
		}
	}) ;
}

void CPlayerGameData::Init()
{
	IPlayerComponent::Init();
	m_eType = ePlayerComponent_PlayerGameData ;
	Reset();
}

bool CPlayerGameData::OnMessage( Json::Value& recvValue , uint16_t nmsgType, eMsgPort eSenderPort )
{
	switch ( nmsgType )
	{
	case MSG_REQ_VIP_ROOM_BILL_IDS:
		{
			Json::Value jsUIDs ;
			for ( auto& ref : m_vVipBillIDs )
			{
				jsUIDs[jsUIDs.size()] = ref ;
			}

			Json::Value jsmsg ;
			jsmsg["billIDs"] = jsUIDs ;
			SendMsg(jsmsg,nmsgType) ;
		}
		break ;
	case MSG_REQ_GAME_DATA:
		{
			eRoomType typer = (eRoomType)recvValue["gameType"].asUInt();
			if ( typer >= eRoom_MJ_MAX )
			{
				recvValue["ret"] = 1 ;
				SendMsg(recvValue,nmsgType) ;
				break;
			}

			recvValue["maxFanXing"] = m_vData[typer].nMaxFangXingType;
			recvValue["maxFanShu"] = m_vData[typer].nMaxFanShu;
			recvValue["roundPlayed"] = m_vData[typer].nRoundsPlayed;
			recvValue["ret"] = 0 ;
			SendMsg(recvValue,nmsgType) ;
		}
		break;
	case MSG_REQ_ENTER_ROOM:
		{
			//if ( isNotInAnyRoom() )
			{
				stMsgSvrEnterRoom msgEnter ;
				msgEnter.cSysIdentifer = GetPlayer()->getMsgPortByRoomType(eRoom_MJ) ;
				if ( msgEnter.cSysIdentifer == ID_MSG_PORT_NONE )
				{
					//stMsgPlayerEnterRoomRet msgRet ;
					//msgRet.nRet = 6;
					//SendMsg(&msgRet,sizeof(msgRet)) ;
					LOGFMTE("player uid = %d enter game , can not find game port type = %d ",GetPlayer()->GetUserUID(), eRoom_MJ ) ;
					break;
				}

				if ( ePlayerGameState_Entering == m_ePlayerGameState )
				{
					LOGFMTD("you are entering player do not do twice") ;
					break ;
				}

				if ( ePlayerGameState_StayIn == m_ePlayerGameState )
				{
					recvValue["type"] = 1;
					recvValue["targetID"] = m_nStateInRoomID ;
					LOGFMTD("player reEnter room , already in room ") ;
				}

				msgEnter.nType = recvValue["type"].asUInt() ;
				msgEnter.nTargetID = recvValue["targetID"].asUInt() ;

				msgEnter.tPlayerData.isRegisted = GetPlayer()->GetBaseData()->isPlayerRegistered() ;
				msgEnter.tPlayerData.nCoin = GetPlayer()->GetBaseData()->getCoin() ;
				msgEnter.tPlayerData.nUserSessionID = GetPlayer()->GetSessionID() ;
				msgEnter.tPlayerData.nUserUID = GetPlayer()->GetUserUID() ;
				msgEnter.tPlayerData.nNewPlayerHaloWeight = GetPlayer()->GetBaseData()->getNewPlayerHaloWeight() ;
				msgEnter.tPlayerData.nPlayerType = GetPlayer()->GetBaseData()->getPlayerType();
				CGameServerApp::SharedGameServerApp()->sendMsg(msgEnter.tPlayerData.nUserSessionID,(char*)&msgEnter,sizeof(msgEnter)) ;

				m_nStateInRoomID = 0;
				m_ePlayerGameState = ePlayerGameState_Entering;
				LOGFMTD("player uid = %d enter to enter room id = %d ,  coin = %u", GetPlayer()->GetUserUID(), msgEnter.nTargetID ,msgEnter.tPlayerData.nCoin ) ;
			}

		}
		break;
	case MSG_CREATE_VIP_ROOM:
		{
			Json::Value jsBack ;
			jsBack["roomID"] = 0 ;
			uint16_t nCardNeed = recvValue["circle"].asUInt() / ROOM_CIRCLES_PER_VIP_ROOM_CARDS  ;
			if ( nCardNeed == 0 || nCardNeed > GetPlayer()->GetBaseData()->getVipRoomCard() )
			{
				jsBack["ret"] = 1 ;
				SendMsg(jsBack,nmsgType) ;
				return true;
			}

			if ( isCreateRoomCntReachLimit(eRoom_MJ) )
			{
				jsBack["ret"] = 2 ;
				SendMsg(jsBack,nmsgType) ;
				return true;
			}

			if ( isNotInAnyRoom() == false)
			{
				jsBack["ret"] = 3;
				SendMsg(jsBack, nmsgType);
				return true;
			}

#ifdef NDEBUG

			if ( m_bIsCreating )
			{
				jsBack["ret"] = 3 ;
				SendMsg(jsBack,nmsgType) ;
				LOGFMTE("already creating , don't try again uid = %u",GetPlayer()->GetUserUID()) ;
				return true;
			}
#endif // NDEBUG


			m_bIsCreating = true ;

			if (eRoom_MJ_Two_Bird_God == recvValue["roomType"].asUInt())
			{
				stMsgCrossServerRequest msgReq;
				msgReq.cSysIdentifer = CPlayer::getMsgPortByRoomType(eRoom_MJ);
				msgReq.nReqOrigID = GetPlayer()->GetUserUID();
				msgReq.nRequestSubType = eCrossSvrReqSub_Default;
				msgReq.nRequestType = eCrossSvrReq_CreateRoom;
				msgReq.nTargetID = 0;
				msgReq.vArg[0] = 0;
				msgReq.vArg[1] = 0;
				msgReq.vArg[2] = 0;
				CON_REQ_MSG_JSON(msgReq, recvValue, autoBuf);
				CGameServerApp::SharedGameServerApp()->sendMsg(msgReq.nReqOrigID, autoBuf.getBufferPtr(), autoBuf.getContentSize());
				LOGFMTD("uid = %u create vip room ", GetPlayer()->GetUserUID());
				return true;
			}

			// create private room ;
			auto pAsyncQ = CGameServerApp::SharedGameServerApp()->getAsynReqQueue();
			recvValue["createUID"] = GetPlayer()->GetUserUID();
			pAsyncQ->pushAsyncRequest(ID_MSG_PORT_MJ, eAsync_CreateRoom, recvValue, [this](uint16_t nReqType, const Json::Value& retContent, Json::Value& jsUserData)
			{
				// ret : 0 leave direct, 1 delay leave room , 2 not in room  ;
				uint8_t nRet = retContent["ret"].asUInt();
				Json::Value jsBack;
				jsBack["ret"] = nRet;
				jsBack["roomID"] = 0;
				if ( nRet == 0)
				{
					eRoomType eType = (eRoomType)retContent["roomType"].asUInt();
					uint32_t nroomid = retContent["roomID"].asUInt();
					jsBack["roomID"] = nroomid;
					if (eRoom_Max > eType )
					{
						addOwnRoom(eType, nroomid, 0);
					}
					else
					{
						LOGFMTE("add my own room , unknown room type = %d , uid = %d", eType, GetPlayer()->GetUserUID());
					}
					LOGFMTD("uid = %d , create room id = %d , config id = %d", GetPlayer()->GetUserUID(), nroomid, 0);
				}
				else
				{
					LOGFMTD("result create failed give back coin uid = %d", GetPlayer()->GetUserUID());
				}
				SendMsg(jsBack, MSG_CREATE_VIP_ROOM);
				m_bIsCreating = false;
				LOGFMTD("uid = %u , create vip room ok ", GetPlayer()->GetUserUID());
			});
		}
		break;
	case MSG_VIP_ROOM_CLOSED:
		{
			deleteOwnRoom((eRoomType)recvValue["eType"].asUInt(),recvValue["roomID"].asUInt());
			LOGFMTD("uid = %u created vip room closed , room id = %u",GetPlayer()->GetUserUID(),recvValue["roomID"].asUInt()) ;
		}
		break; 
	default:
		return false;
	}	
	return true ;
}

bool CPlayerGameData::OnMessage( stMsg* pMessage , eMsgPort eSenderPort)
{
	if ( IPlayerComponent::OnMessage(pMessage,eSenderPort) )
	{
		return true ;
	}
	switch ( pMessage->usMsgType )
	{
	//case MSG_PLAYER_ENTER_ROOM:
	//	{
	//		stMsgPlayerEnterRoom* pRet = (stMsgPlayerEnterRoom*)pMessage ;
	//		//if ( isNotInAnyRoom()  )
	//		{
	//			stMsgSvrEnterRoom msgEnter ;
	//			msgEnter.cSysIdentifer = GetPlayer()->getMsgPortByRoomType(pRet->nRoomGameType) ;
	//			if ( msgEnter.cSysIdentifer == ID_MSG_PORT_NONE )
	//			{
	//				stMsgPlayerEnterRoomRet msgRet ;
	//				msgRet.nRet = 6;
	//				SendMsg(&msgRet,sizeof(msgRet)) ;
	//				LOGFMTE("player uid = %d enter game , can not find game port type = %d ",GetPlayer()->GetUserUID(), pRet->nRoomGameType ) ;
	//				break;
	//			}

	//			if ( isNotInAnyRoom() == false )
	//			{
	//				pRet->nRoomGameType = m_nStateInRoomType;
	//				pRet->nRoomID = m_nStateInRoomID ;
	//				pRet->nSubIdx = m_nSubRoomIdx ;

	//				LOGFMTD("player reEnter room ") ;
	//			}

	//			msgEnter.nGameType = pRet->nRoomGameType ;
	//			msgEnter.nRoomID = pRet->nRoomID ;
	//			msgEnter.nSubIdx = pRet->nSubIdx ;
	//			msgEnter.tPlayerData.isRegisted = GetPlayer()->GetBaseData()->isPlayerRegistered() ;
	//			msgEnter.tPlayerData.nCoin = GetPlayer()->GetBaseData()->getCoin() ;
	//			msgEnter.tPlayerData.nUserSessionID = GetPlayer()->GetSessionID() ;
	//			msgEnter.tPlayerData.nUserUID = GetPlayer()->GetUserUID() ;
	//			msgEnter.tPlayerData.nNewPlayerHaloWeight = GetPlayer()->GetBaseData()->getNewPlayerHaloWeight() ;
	//			msgEnter.tPlayerData.nPlayerType = GetPlayer()->GetBaseData()->getPlayerType();
	//			CGameServerApp::SharedGameServerApp()->sendMsg(msgEnter.tPlayerData.nUserSessionID,(char*)&msgEnter,sizeof(msgEnter)) ;

	//			m_nStateInRoomID = pRet->nRoomID;
	//			m_nStateInRoomType = pRet->nRoomGameType;
	//			LOGFMTD("player uid = %d enter to enter room id = %d , type = %d coin = %u", GetPlayer()->GetUserUID(), m_nStateInRoomID, m_nStateInRoomType,msgEnter.tPlayerData.nCoin ) ;
	//		}
			//else
			//{
			//	stMsgPlayerEnterRoomRet msgRet ;
			//	msgRet.nRet = 1;
			//	SendMsg(&msgRet,sizeof(msgRet)) ;
			//	LOGFMTD("player uid = %d already in room type = %d , id = %d ", GetPlayer()->GetUserUID() , m_nStateInRoomType,m_nStateInRoomID ) ;
			//}
		//}
		//break;
	case MSG_SVR_ENTER_ROOM:
		{
			stMsgSvrEnterRoomRet* pRet = (stMsgSvrEnterRoomRet*)pMessage ;

			Json::Value jsMsg ;
			jsMsg["ret"] = pRet->nRet ;
			SendMsg(jsMsg,MSG_REQ_ENTER_ROOM) ;

			if ( pRet->nRet )  // enter room failed ;
			{
				m_nStateInRoomID = 0;
				m_ePlayerGameState = ePlayerGameState_NotIn;
				//m_nSubRoomIdx = 0 ;
				LOGFMTD("player enter room failed ret = %d uid = %d",pRet->nRet,GetPlayer()->GetUserUID()) ;
			}
			else
			{
				m_nStateInRoomID = pRet->nRoomID;
				//m_nStateInRoomType = pRet->nGameType;
				//m_nSubRoomIdx = (uint8_t)pRet->nSubIdx ;
				m_ePlayerGameState = ePlayerGameState_StayIn;
				LOGFMTD("player do enter room id = %d , uid = %d ",m_nStateInRoomID,GetPlayer()->GetUserUID()) ;
			}
		}
		break;
	case MSG_SVR_DO_LEAVE_ROOM:
		{
			m_nStateInRoomID = 0;
			m_ePlayerGameState = ePlayerGameState_NotIn;
			stMsgSvrDoLeaveRoom* pRet = (stMsgSvrDoLeaveRoom*)pMessage ;
			LOGFMTD("uid = %d leave room coin = %u , back coin = %u, temp coin = %u",GetPlayer()->GetUserUID(),GetPlayer()->GetBaseData()->getCoin(),pRet->nCoin ) ;
			GetPlayer()->GetBaseData()->setCoin(pRet->nCoin) ;
			GetPlayer()->GetBaseData()->addTodayGameCoinOffset(pRet->nGameOffset);
			
			m_vData[pRet->nGameType].nRoundsPlayed += pRet->nRoundsPlayed ;
			if ( m_vData[pRet->nGameType].nMaxFangXingType < pRet->nMaxFangXingType )
			{
				m_vData[pRet->nGameType].nMaxFangXingType = pRet->nMaxFangXingType ;
			}

			if ( m_vData[pRet->nGameType].nMaxFanShu < pRet->nMaxFanShu )
			{
				m_vData[pRet->nGameType].nMaxFanShu = pRet->nMaxFanShu ;
			}

			if ( pRet->nRoundsPlayed != 0 )
			{
				m_vData[pRet->nGameType].bDirty = true ;
			}

			// decrease halo weight 
			if ( GetPlayer()->GetBaseData()->getNewPlayerHaloWeight() >= pRet->nRoundsPlayed )
			{
				GetPlayer()->GetBaseData()->setNewPlayerHalo(GetPlayer()->GetBaseData()->getNewPlayerHaloWeight() - pRet->nRoundsPlayed );
			}
			else
			{
				GetPlayer()->GetBaseData()->setNewPlayerHalo(0);
			}

			LOGFMTD("uid = %d do leave room final coin = %u, playertimes = %u , wintimes = %u ,offset = %d",GetPlayer()->GetUserUID(), GetPlayer()->GetBaseData()->getCoin(),pRet->nRoundsPlayed,pRet->nMaxFanShu,pRet->nGameOffset) ;
			/*stMsg msg ;
			msg.usMsgType = MSG_PLAYER_UPDATE_MONEY ;
			GetPlayer()->GetBaseData()->OnMessage(&msg,ID_MSG_PORT_CLIENT) ;*/
			Json::Value jsq ;
			jsq[JS_KEY_MSG_TYPE] = MSG_REQ_UPDATE_COIN ;
			GetPlayer()->GetBaseData()->OnMessage(jsq,MSG_REQ_UPDATE_COIN,ID_MSG_PORT_CLIENT ) ;

			/// if robot must tell idle ;
			if (GetPlayer()->GetBaseData()->getPlayerType() == ePlayer_Robot)
			{
				Json::Value js;
				CGameServerApp::SharedGameServerApp()->getRobotCenter()->onMsg(js, MSG_TELL_ROBOT_IDLE,ID_MSG_PORT_CLIENT,GetPlayer()->GetSessionID());
				LOGFMTD("robot leave room svr tell idle uid = %u ",GetPlayer()->GetUserUID());
			}
		}
		break;
	case MSG_SVR_DELAYED_LEAVE_ROOM:
		{
			stMsgSvrDelayedLeaveRoom* pRet = (stMsgSvrDelayedLeaveRoom*)pMessage ;
			if ( isNotInAnyRoom() )
			{
				GetPlayer()->GetBaseData()->setCoin( pRet->nCoin + GetPlayer()->GetBaseData()->getCoin() ) ;
				LOGFMTD("player not enter other room just uid = %d add coin = %u, final = %u,",GetPlayer()->GetUserUID(),pRet->nCoin,GetPlayer()->GetBaseData()->getCoin()) ;
			}
			else
			{
				GetPlayer()->GetBaseData()->AddMoney(pRet->nCoin) ;
				LOGFMTD("player enter other room so uid = %d  final = %u,",GetPlayer()->GetUserUID(),pRet->nCoin,GetPlayer()->GetBaseData()->getCoin() ) ;
			}

			GetPlayer()->GetBaseData()->addTodayGameCoinOffset(pRet->nGameOffset);

			m_vData[pRet->nGameType].nRoundsPlayed += pRet->nRoundsPlayed ;
			if ( m_vData[pRet->nGameType].nMaxFangXingType < pRet->nMaxFangXingType )
			{
				m_vData[pRet->nGameType].nMaxFangXingType = pRet->nMaxFangXingType ;
			}

			if ( m_vData[pRet->nGameType].nMaxFanShu < pRet->nMaxFanShu )
			{
				m_vData[pRet->nGameType].nMaxFanShu = pRet->nMaxFanShu ;
			}

			if ( pRet->nRoundsPlayed != 0 )
			{
				m_vData[pRet->nGameType].bDirty = true ;
			}

			LOGFMTD("uid = %d delay leave room coin = %u",GetPlayer()->GetUserUID(), GetPlayer()->GetBaseData()->getCoin()) ;
			//stMsg msg ;
			//msg.usMsgType = MSG_PLAYER_UPDATE_MONEY ;
			//GetPlayer()->GetBaseData()->OnMessage(&msg,ID_MSG_PORT_CLIENT) ;
			Json::Value jsq ;
			jsq[JS_KEY_MSG_TYPE] = MSG_REQ_UPDATE_COIN ;
			GetPlayer()->GetBaseData()->OnMessage(jsq,MSG_REQ_UPDATE_COIN,ID_MSG_PORT_CLIENT ) ;
		}
		break;
	case MSG_READ_PLAYER_NIUNIU_DATA:
		{
			stMsgReadPlayerNiuNiuDataRet* pRet = (stMsgReadPlayerNiuNiuDataRet*)pMessage ;
			auto& gdata = m_vData[eRoom_NiuNiu] ;
			gdata.bDirty = false ;
			memcpy(&gdata,&pRet->tData,sizeof(pRet->tData)) ;
		}
		break;
	case MSG_READ_PLAYER_TAXAS_DATA:
		{
			stMsgReadPlayerTaxasDataRet* pRet = (stMsgReadPlayerTaxasDataRet*)pMessage ;
			if ( pRet->nRet )
			{
				LOGFMTE("uid = %d read taxas data error",pRet->nUserUID);
				return true ;
			}

			auto& gdata = m_vData[eRoom_TexasPoker] ;
			gdata.bDirty = false ;
			memcpy(&gdata,&pRet->tData,sizeof(pRet->tData)) ;
			LOGFMTD("uid taxas data  , owner rooms = %d",m_vMyOwnRooms[eRoom_TexasPoker].size());
			sendGameDataToClient();
		}
		break;
	case MSG_REQUEST_MY_OWN_ROOMS:
		{
			stMsgRequestMyOwnRooms* pRet = (stMsgRequestMyOwnRooms*)pMessage ;
			if ( pRet->nRoomType >= eRoom_Max )
			{
				return false;
			}

			stMsgRequestMyOwnRoomsRet msgRet ;
			msgRet.nRoomType = pRet->nRoomType ;
			msgRet.nCnt = m_vMyOwnRooms[pRet->nRoomType].size() ;
			if ( msgRet.nCnt == 0 )
			{
				SendMsg(&msgRet,sizeof(msgRet)) ;
				return true ;
			}

			CAutoBuffer autoBuffer(sizeof(msgRet) + sizeof(uint32_t)* msgRet.nCnt);
			autoBuffer.addContent((char*)&msgRet,sizeof(msgRet)) ;
			MAP_ID_MYROOW::iterator iter = m_vMyOwnRooms[pRet->nRoomType].begin() ;
			for ( ; iter != m_vMyOwnRooms[pRet->nRoomType].end() ; ++iter )
			{
				uint32_t n = iter->first ;
				autoBuffer.addContent((char*)&n,sizeof(uint32_t));
			}
			SendMsg((stMsg*)autoBuffer.getBufferPtr(),autoBuffer.getContentSize()) ;
		}
		break;
	case MSG_READ_MY_OWN_ROOMS:
		{
			stMsgReadMyOwnRoomsRet* pRet = (stMsgReadMyOwnRoomsRet*)pMessage ;
			if ( pRet->nRoomType >= eRoom_Max )
			{
				return false;
			}
			stMyOwnRoom* pRoomPtr = (stMyOwnRoom*)((char*)pMessage + sizeof(stMsgReadMyOwnRoomsRet));
			while ( pRet->nCnt-- )
			{
				m_vMyOwnRooms[pRet->nRoomType].insert(MAP_ID_MYROOW::value_type(pRoomPtr->nRoomID,*pRoomPtr));
				++pRoomPtr ;
			}
			LOGFMTD("uid = %d ,read own creator room" , GetPlayer()->GetUserUID() ) ;
		}
		break;
	case MSG_DELETE_ROOM:
		{
			stMsgDeleteRoomRet msgBack ;
			stMsgDeleteRoom* pRet = (stMsgDeleteRoom*)pMessage ;
			msgBack.nRoomID = pRet->nRoomID ;
			msgBack.nRoomType = pRet->nRoomType ;
			bool bDeleted = false ;
			if ( eRoom_Max > pRet->nRoomType )
			{
				bDeleted = deleteOwnRoom((eRoomType)pRet->nRoomType,pRet->nRoomID) ;
			}
			else
			{
				msgBack.nRet = 2 ;
				SendMsg(&msgBack,sizeof(msgBack)) ;
				return true ;
			}

			if ( !bDeleted )
			{
				msgBack.nRet = 1 ;
				SendMsg(&msgBack,sizeof(msgBack)) ;
				return true ;
			}

			// do delete room ;
			stMsgCrossServerRequest msgReq ;
			msgReq.nJsonsLen = 0 ;
			msgReq.cSysIdentifer = CPlayer::getMsgPortByRoomType(pRet->nRoomType) ;
			msgReq.nReqOrigID = GetPlayer()->GetUserUID() ;
			msgReq.nRequestSubType = eCrossSvrReqSub_Default;
			msgReq.nRequestType = eCrossSvrReq_DeleteRoom ;
			msgReq.nTargetID = 0 ;
			memset(msgReq.vArg,0,sizeof(msgReq.vArg)) ;
			msgReq.vArg[0] = pRet->nRoomType ;
			msgReq.vArg[1] = pRet->nRoomID ;
			CGameServerApp::SharedGameServerApp()->sendMsg(msgReq.nReqOrigID, (char*)&msgReq,sizeof(msgReq)) ;
			LOGFMTD("uid = %d , delete room = %d , type = %d",GetPlayer()->GetUserUID(),pRet->nRoomID,pRet->nRoomType) ;

			msgBack.nRet = 0 ;
			SendMsg(&msgBack,sizeof(msgBack)) ;
		}
		break;
	//case MSG_CREATE_ROOM:
	//	{
	//		stMsgCreateRoom* pRet = (stMsgCreateRoom*)pMessage ;

	//		stMsgCreateRoomRet msgBack ;
	//		msgBack.nRoomID = 0 ;
	//		msgBack.nRoomType = pRet->nRoomType ;
	//		msgBack.nFinalCoin = GetPlayer()->GetBaseData()->getCoin() ;

	//		CRoomConfigMgr* pConfigMgr = (CRoomConfigMgr*)CGameServerApp::SharedGameServerApp()->GetConfigMgr()->GetConfig(CConfigManager::eConfig_Room);
	//		stTaxasRoomConfig* pRoomConfig = (stTaxasRoomConfig*)pConfigMgr->GetConfigByConfigID(pRet->nConfigID) ;
	//		if ( pRoomConfig == nullptr )
	//		{
	//			msgBack.nRet = 1 ;
	//			msgBack.nRoomID = 0 ;
	//			SendMsg(&msgBack,sizeof(msgBack)) ;
	//			return true ;
	//		}

	//		// check if create room count reach limit ;
	//		bool bReachLimit = false ;
	//		if ( eRoom_Max > pRet->nRoomType )
	//		{
	//			bReachLimit = isCreateRoomCntReachLimit((eRoomType)pRet->nRoomType) ;
	//		}
	//		else
	//		{
	//			LOGFMTE("add my own room , unknown room type = %d , uid = %d",msgBack.nRoomType,GetPlayer()->GetUserUID()) ;
	//			msgBack.nRet = 1 ;
	//			msgBack.nRoomID = 0 ;
	//			SendMsg(&msgBack,sizeof(msgBack)) ;
	//			return true ;
	//		}

	//		if ( bReachLimit && GetPlayer()->GetUserUID() != MATCH_MGR_UID )
	//		{
	//			msgBack.nRet = 5 ;
	//			msgBack.nRoomID = 0 ;
	//			SendMsg(&msgBack,sizeof(msgBack)) ;
	//			return true ;
	//		}

	//		// check coin weather engough 
	//		if ( GetPlayer()->GetBaseData()->getCoin() < pRoomConfig->nRentFeePerDay * pRet->nMinites )
	//		{
	//			msgBack.nRet = 4 ;
	//			msgBack.nRoomID = 0 ;
	//			SendMsg(&msgBack,sizeof(msgBack)) ;
	//			return true ;
	//		}

	//		stMsgCrossServerRequest msgReq ;
	//		msgReq.cSysIdentifer = CPlayer::getMsgPortByRoomType(pRet->nRoomType) ;
	//		if ( msgReq.cSysIdentifer == ID_MSG_PORT_NONE )
	//		{
	//			msgBack.nRet = 6 ;
	//			SendMsg(&msgBack,sizeof(msgBack)) ;
	//			LOGFMTE("crate room but , unknown room type = %d , uid = %d",pRet->nRoomType, GetPlayer()->GetUserUID() ) ;
	//			return true ;
	//		}

	//		GetPlayer()->GetBaseData()->decressMoney(pRoomConfig->nRentFeePerDay * pRet->nMinites );

	//		msgReq.nReqOrigID = GetPlayer()->GetUserUID() ;
	//		msgReq.nRequestSubType = eCrossSvrReqSub_Default;
	//		msgReq.nRequestType = eCrossSvrReq_CreateRoom ;
	//		msgReq.nTargetID = 0 ;
	//		msgReq.vArg[0] = pRet->nConfigID ;
	//		msgReq.vArg[1] = pRet->nMinites ;
	//		msgReq.vArg[2] = pRet->nRoomType ;
	//		pRet->vRoomName[MAX_LEN_ROOM_NAME-1] = 0 ;

	//		Json::Value vArg ;
	//		vArg["roonName"] = pRet->vRoomName;
	//		if (msgReq.nReqOrigID == MATCH_MGR_UID )
	//		{
	//			vArg["subRoomCnt"] = 8 ;
	//		}
	//		CON_REQ_MSG_JSON(msgReq,vArg,autoBuf) ;
	//		CGameServerApp::SharedGameServerApp()->sendMsg(msgReq.nReqOrigID,autoBuf.getBufferPtr(),autoBuf.getContentSize()) ;
	//	}
	//	break;
	default:
		return false;
	}
	return true ;
}

bool CPlayerGameData::onCrossServerRequest(stMsgCrossServerRequest* pRequest , eMsgPort eSenderPort,Json::Value* vJsValue)
{
	if ( IPlayerComponent::onCrossServerRequest(pRequest,eSenderPort,vJsValue) )
	{
		return true ;
	}

	switch ( pRequest->nRequestType )
	{
	case eCrossSvrReq_LeaveRoomRet:
		{
			m_ePlayerGameState = ePlayerGameState_NotIn;
			m_nStateInRoomID = 0 ;
			LOGFMTE("uid = %d leave room state error ",GetPlayer()->GetUserUID() ) ;
		}
		break;
	default:
		return false ;
	}
	return true ;
}

bool CPlayerGameData::onCrossServerRequestRet(stMsgCrossServerRequestRet* pResult,Json::Value* vJsValue )
{
	if ( IPlayerComponent::onCrossServerRequestRet(pResult,vJsValue) )
	{
		return true ;
	}

	if ( eCrossSvrReq_CreateRoom == pResult->nRequestType  )
	{
		Json::Value jsBack ;
		jsBack["ret"] = pResult->nRet ;
		eRoomType eType = (eRoomType)pResult->vArg[2] ;
		uint32_t nroomid = pResult->vArg[1] ;
		jsBack["roomID"] = nroomid ;
		if ( pResult->nRet == 0 )
		{
			if ( eRoom_Max > pResult->vArg[2] )
			{
				addOwnRoom(eType,nroomid,pResult->vArg[0]) ;
			}
			else
			{
				LOGFMTE("add my own room , unknown room type = %d , uid = %d",eType,GetPlayer()->GetUserUID()) ;
			}
			LOGFMTD("uid = %d , create room id = %d , config id = %d", GetPlayer()->GetUserUID(),nroomid,(int32_t)pResult->vArg[0] ) ;
		}
		else
		{
			LOGFMTD("result create failed give back coin uid = %d",GetPlayer()->GetUserUID());
		}
		SendMsg(jsBack,MSG_CREATE_VIP_ROOM) ;
		m_bIsCreating = false ;
		LOGFMTD("uid = %u , create vip room ok ",GetPlayer()->GetUserUID()) ;
		return true ;
	}

	if ( eCrossSvrReq_RoomProfit == pResult->nRequestType )
	{
		stMsgCaculateRoomProfitRet msgBack ;
		msgBack.nRoomID = pResult->nReqOrigID ;
		msgBack.bDiamond = !pResult->vArg[0] ;
		msgBack.nProfitMoney = pResult->vArg[1] ;
		msgBack.nRoomType = pResult->vArg[2] ;
		msgBack.nRet = pResult->nRet ? 3 : 0 ;
		SendMsg(&msgBack,sizeof(msgBack)) ;
		if ( msgBack.nRet )
		{
			return true ;
		}
		GetPlayer()->GetBaseData()->AddMoney(msgBack.nProfitMoney,msgBack.bDiamond);
		// save log 
		stMsgSaveLog msgLog ;
		memset(msgLog.vArg,0,sizeof(msgLog.vArg));
		msgLog.nJsonExtnerLen = 0 ;
		msgLog.nLogType = eLog_AddMoney ;
		msgLog.nTargetID = GetPlayer()->GetUserUID() ;
		msgLog.vArg[0] = !msgBack.bDiamond ;
		msgLog.vArg[1] = msgBack.nProfitMoney;
		msgLog.vArg[2] = GetPlayer()->GetBaseData()->GetData()->nCoin;
		msgLog.vArg[3] = GetPlayer()->GetBaseData()->GetData()->nDiamoned ;
		msgLog.vArg[4] = eCrossSvrReq_RoomProfit ;
		msgLog.vArg[5] = pResult->nReqOrigID ;
		CGameServerApp::SharedGameServerApp()->sendMsg(pResult->nReqOrigID,(char*)&msgLog,sizeof(msgLog));

		LOGFMTD("uid = %d get profit = %llu",GetPlayer()->GetUserUID(),msgBack.nProfitMoney) ;
		return true ;
	}

	if ( eCrossSvrReq_AddRentTime == pResult->nRequestType )
	{
		stMsgAddRoomRentTimeRet msgRet ;
		msgRet.nRet = pResult->nRet ? 4 : 0  ;
		msgRet.nAddDays = pResult->vArg[0] ;
		msgRet.nRoomID = pResult->nReqOrigID ;
		msgRet.nRoomType = pResult->vArg[1] ;
		SendMsg(&msgRet,sizeof(msgRet)) ;
		if ( msgRet.nRet )
		{
			GetPlayer()->GetBaseData()->AddMoney(pResult->vArg[2]) ;
		}
		LOGFMTD("uid = %d add rent time = %d",GetPlayer()->GetUserUID(),msgRet.nAddDays) ;
		return true ;
	}
	return false ;
}

void CPlayerGameData::OnPlayerDisconnect()
{
	IPlayerComponent::OnPlayerDisconnect();
	if ( isNotInAnyRoom() == false )
	{
		stMsgCrossServerRequest msgEnter ;
		msgEnter.cSysIdentifer = ID_MSG_PORT_MJ ;//GetPlayer()->getMsgPortByRoomType(m_nStateInRoomType) ;
		msgEnter.nJsonsLen = 0 ;
		msgEnter.nReqOrigID = GetPlayer()->GetUserUID();
		msgEnter.nRequestSubType = eCrossSvrReqSub_Default ;
		msgEnter.nRequestType = eCrossSvrReq_ApplyLeaveRoom ;
		msgEnter.nTargetID = m_nStateInRoomID ;
		msgEnter.vArg[0] = m_nStateInRoomID ;
		msgEnter.vArg[1] = GetPlayer()->GetSessionID() ;
		//msgEnter.vArg[2] = m_nStateInRoomType ;
		SendMsg(&msgEnter,sizeof(msgEnter)) ;
		LOGFMTD("uid = %d disconnected , apply to leave room id = %d ",GetPlayer()->GetUserUID(),m_nStateInRoomID) ;
		GetPlayer()->delayDelete();
		doApplyLeaveRoom(0);
	}
}

void CPlayerGameData::OnOtherWillLogined()
{
	IPlayerComponent::OnOtherWillLogined();
	if ( isNotInAnyRoom() == false )
	{
		stMsgCrossServerRequest msgEnter ;
		msgEnter.cSysIdentifer = ID_MSG_PORT_MJ ; //GetPlayer()->getMsgPortByRoomType(m_nStateInRoomType) ;
		msgEnter.nJsonsLen = 0 ;
		msgEnter.nReqOrigID = GetPlayer()->GetUserUID();
		msgEnter.nRequestSubType = eCrossSvrReqSub_Default ;
		msgEnter.nRequestType = eCrossSvrReq_ApplyLeaveRoom ;
		msgEnter.nTargetID = m_nStateInRoomID ;
		msgEnter.vArg[0] = m_nStateInRoomID ;
		msgEnter.vArg[1] = GetPlayer()->GetSessionID() ;
		SendMsg(&msgEnter,sizeof(msgEnter)) ;

		doApplyLeaveRoom(1);
		LOGFMTD("uid = %d other device login , apply to leave room id = %d ",GetPlayer()->GetUserUID(),m_nStateInRoomID) ;
	}
}

void CPlayerGameData::TimerSave()
{
	char pBuffer[255] = { 0 } ;
	auto asyQ = CGameServerApp::SharedGameServerApp()->getAsynReqQueue();
	for ( uint8_t nIdx = eRoom_None; nIdx < eRoom_Max ; ++nIdx )
	{
		auto& gameData = m_vData[nIdx] ;
		if ( gameData.bDirty == false )
		{
			continue;
		}

		gameData.bDirty = false ;
		
		//stMsgSavePlayerGameData msgSave ;
		//msgSave.nGameType = nIdx ;
		//msgSave.nUserUID = GetPlayer()->GetUserUID() ;
		//memcpy((char*)&msgSave.tData,&gameData,sizeof(msgSave.tData));
		//SendMsg(&msgSave,sizeof(msgSave)) ;

		Json::Value jsReq ;
		memset(pBuffer,0,sizeof(pBuffer)) ;
		sprintf_s(pBuffer,sizeof(pBuffer),"INSERT INTO playergamerecoder ( userUID,gameType,roundsPlayed,maxWinTimes,maxWinCardType) VALUES ( %u ,%u,%u,%u,%u) ON DUPLICATE KEY UPDATE \
			roundsPlayed = %u ,maxWinTimes = %u,maxWinCardType = %u ",GetPlayer()->GetUserUID(),gameData.nGameType,gameData.nRoundsPlayed,gameData.nMaxFanShu,gameData.nMaxFangXingType
			,gameData.nRoundsPlayed,gameData.nMaxFanShu,gameData.nMaxFangXingType );
		jsReq["sql"] = pBuffer ;
		asyQ->pushAsyncRequest(ID_MSG_PORT_DB,eAsync_DB_Add,jsReq);
	}
}

void CPlayerGameData::addOwnRoom(eRoomType eType , uint32_t nRoomID , uint16_t nConfigID )
{
	if ( eType >= eRoom_Max )
	{
		return ;
	}
	stMyOwnRoom myroom ;
	myroom.nRoomID = nRoomID ;
	m_vMyOwnRooms[eType].insert(MAP_ID_MYROOW::value_type(myroom.nRoomID,myroom));
}

bool CPlayerGameData::isCreateRoomCntReachLimit(eRoomType eType)
{
	if ( GetPlayer()->GetUserUID() == MATCH_MGR_UID )
	{
		return false ;
	}

	if ( eType >= eRoom_Max )
	{
		return true;
	}

	for (auto& ref : m_vMyOwnRooms)
	{
		if (ref.size() >= 1)
		{
			return true;
		}
	}
	return false;
}

bool CPlayerGameData::deleteOwnRoom(eRoomType eType , uint32_t nRoomID )
{
	if ( eType >= eRoom_Max )
	{
		return false;
	}

	auto iter = m_vMyOwnRooms[eType].find(nRoomID) ;
	if ( iter != m_vMyOwnRooms[eType].end() )
	{
		m_vMyOwnRooms[eType].erase(iter) ;
		return true ;
	}
	return false ;
}

uint32_t CPlayerGameData::getOwnRoomID()
{
	for (auto& vR : m_vMyOwnRooms)
	{
		for (auto& r : vR)
		{
			if (r.first)
			{
				return r.first;
			}
		}
	}

	return 0;
}

//uint16_t CPlayerGameData::getMyOwnRoomConfig( eRoomType eType , uint32_t nRoomID ) 
//{
//	if ( eType >= eRoom_Max )
//	{
//		return 0;
//	}
//
//	MAP_ID_MYROOW::iterator iter = m_vMyOwnRooms[eType].find(nRoomID) ;
//	if ( iter != m_vMyOwnRooms[eType].end() )
//	{
//		return iter->second.nConfigID ;
//	}
//	return 0 ;
//}

bool CPlayerGameData::isRoomIDMyOwn(eRoomType eType , uint32_t nRoomID)
{
	if ( eType >= eRoom_Max )
	{
		return false;
	}
	MAP_ID_MYROOW::iterator iter = m_vMyOwnRooms[eType].find(nRoomID) ;
	return iter != m_vMyOwnRooms[eType].end() ;
}

void CPlayerGameData::sendGameDataToClient()
{
	// send niuniu data ;
	//stMsgPlayerBaseDataNiuNiu msg ;
	//memcpy(&msg.tNiuNiuData,&m_vData[eRoom_NiuNiu],sizeof(msg.tNiuNiuData));
	//SendMsg(&msg,sizeof(msg)) ;

	//// send taxas data 
	//stMsgPlayerBaseDataTaxas msgT ;
	//memcpy(&msgT.tTaxasData,&m_vData[eRoom_TexasPoker],sizeof(msgT.tTaxasData));
	//SendMsg(&msgT,sizeof(msgT)) ;
}

void CPlayerGameData::addNewBillIDs(uint32_t nBillID )
{
	auto iter = std::find(m_vVipBillIDs.begin(),m_vVipBillIDs.end(),nBillID) ;
	if ( iter != m_vVipBillIDs.end() )
	{
		LOGFMTE("already added the bill id = %u",nBillID) ;
		return ;
	}

	m_vVipBillIDs.push_front(nBillID);
	if ( m_vVipBillIDs.size() > 10 )
	{
		m_vVipBillIDs.pop_back();
	}

	// save to db 
	Json::Value jsReq ;
	std::ostringstream ss ;
	ss << "insert into playerbillids (userUID,billID) values ( " << GetPlayer()->GetUserUID() << "," << nBillID << " ) ; " ;
	jsReq["sql"] = ss.str();
	CGameServerApp::SharedGameServerApp()->getAsynReqQueue()->pushAsyncRequest(ID_MSG_PORT_DB,eAsync_DB_Add,jsReq) ;
}

void CPlayerGameData::doApplyLeaveRoom(uint8_t nReason)
{
	if (getCurGameState() != ePlayerGameState_StayIn)
	{
		return;
	}

	auto pAsyncQ = CGameServerApp::SharedGameServerApp()->getAsynReqQueue();
	Json::Value jsReq;
	jsReq["uid"] = GetPlayer()->GetUserUID();
	jsReq["roomID"] = m_nStateInRoomID;
	jsReq["reason"] = nReason;
	//pAsyncQ->pushAsyncRequest(ID_MSG_PORT_MJ, eAsync_ApplyLeaveRoom, jsReq);
	pAsyncQ->pushAsyncRequest(ID_MSG_PORT_MJ, eAsync_ApplyLeaveRoom, jsReq, [this](uint16_t nReqType, const Json::Value& retContent, Json::Value& jsUserData)
	{
		// ret : 0 leave direct, 1 delay leave room , 2 not in room  ;
		uint8_t nRet = retContent["ret"].asUInt();
		//uint32_t nCoin = retContent["coin"].asUInt();
		//if (0 == nRet)
		//{
		//	//m_ePlayerGameState = ePlayerGameState_NotIn;
		//	//m_nStateInRoomID = 0;
		//	LOGFMTD("player uid = %u leave room old coin = %u",GetPlayer()->GetUserUID(),GetPlayer()->GetBaseData()->getCoin());
		//	//GetPlayer()->GetBaseData()->setCoin(nCoin);
		//}
		//else if ( 1 == nRet )
		//{ 
		//	LOGFMTD("player uid = %u will delay leave room coin = %u", GetPlayer()->GetUserUID(), GetPlayer()->GetBaseData()->getCoin() );
		//}
		//else if (2 == nRet || 3 == nRet )
		if (2 == nRet || 3 == nRet)
		{
			m_ePlayerGameState = ePlayerGameState_NotIn;
			m_nStateInRoomID = 0;
			LOGFMTD("player uid = %u not in room but reset state  coin = %u or room is null ", GetPlayer()->GetUserUID(), GetPlayer()->GetBaseData()->getCoin());
		}
		else
		{
			LOGFMTE("apply result unknown ret = %u , uid = %u",nRet,GetPlayer()->GetUserUID());
		}
	});
}