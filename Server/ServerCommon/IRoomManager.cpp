#include "IRoomManager.h"
#include "log4z.h"
#include "IRoomInterface.h"
#include "ServerMessageDefine.h"
#include "RoomConfig.h"
#include "AutoBuffer.h"
#include <cassert>
#include "IRoomState.h"
#include "RoomConfig.h"
#include "ISeverApp.h"
#include "AsyncRequestQuene.h"
#include <sstream>
#include "ISitableRoom.h"
#define ROOM_LIST_ITEM_CNT_PER_PAGE 5 
#define TIME_SAVE_ROOM_INFO 60*10
uint32_t IRoomManager::s_MaxBillID = 0 ;
IRoomManager::IRoomManager(CRoomConfigMgr* pConfigMgr)
{
	m_pConfigMgr = pConfigMgr ;
}

IRoomManager::~IRoomManager()
{
	for ( auto& ref : m_vRooms )
	{
		delete ref.second ;
		ref.second = nullptr ;
	}
	m_vRooms.clear() ;
	m_vSystemRooms.clear() ;
	m_mapPrivateRooms.clear() ;
}

void IRoomManager::init(IServerApp* svrApp)
{
	IGlobalModule::init(svrApp);

	m_vReqingBillInfoPlayers.clear();
	m_nMaxRoomID = 1 ;
	m_vRooms.clear();
	m_mapPrivateRooms.clear() ;
	m_pGoTyeAPI.init("https://qplusapi.gotye.com.cn:8443/api/");
	m_pGoTyeAPI.setDelegate(this);
}

void IRoomManager::sendMsg(stMsg* pmsg, uint32_t nLen , uint32_t nSessionID )
{
	getSvrApp()->sendMsg(nSessionID,(char*)pmsg,nLen);
}

void IRoomManager::sendMsg( Json::Value& jsContent , unsigned short nMsgType , uint32_t nSessionID, eMsgPort ePort )
{
	getSvrApp()->sendMsg(nSessionID,jsContent,nMsgType,ePort,false);
}

bool IRoomManager::onMsg( stMsg* prealMsg , eMsgPort eSenderPort , uint32_t nSessionID )
{
	if ( IGlobalModule::onMsg(prealMsg,eSenderPort,nSessionID) )
	{
		return true ;
	}

	if ( onPublicMsg(prealMsg,eSenderPort,nSessionID) )
	{
		return true ;
	}

	if ( MSG_CROSS_SERVER_REQUEST == prealMsg->usMsgType )
	{
		stMsgCrossServerRequest* pRet = (stMsgCrossServerRequest*)prealMsg ;
		//LOGFMTI("request crose type = %d",pRet->nRequestType);
		Json::Value* pJsValue = nullptr ;
		Json::Value rootValue ;
		if ( pRet->nJsonsLen )
		{
			Json::Reader reader;
			char* pstr = ((char*)&pRet->nJsonsLen) + sizeof(pRet->nJsonsLen) ;
			reader.parse(pstr,pstr + pRet->nJsonsLen,rootValue,false);
			pJsValue = &rootValue ;
		}

		if ( onCrossServerRequest(pRet,eSenderPort,pJsValue) == false )
		{
			{
				LOGFMTE("unprocess cross svr request type = %d",pRet->nRequestType) ;
				return false ;
			}
		}

		return true ;
	}

	if ( MSG_CROSS_SERVER_REQUEST_RET == prealMsg->usMsgType )
	{
		stMsgCrossServerRequestRet* pRet = (stMsgCrossServerRequestRet*)prealMsg ;
		Json::Value* pJsValue = nullptr ;
		Json::Value rootValue ;
		if ( pRet->nJsonsLen )
		{
			Json::Reader reader;
			char* pstr = ((char*)&pRet->nJsonsLen) + sizeof(pRet->nJsonsLen) ;
			reader.parse(pstr,pstr + pRet->nJsonsLen,rootValue,false);
			pJsValue = &rootValue ;
		}

		if ( onCrossServerRequestRet(pRet,pJsValue) == false )
		{
			return false ;
		}
		return true ;
	}

	////if ( prealMsg->usMsgType <= MSG_TP_BEGIN || MSG_TP_END <= prealMsg->usMsgType )
	////{
	////	LOGFMTE("why this msg send here msg = %d ",prealMsg->usMsgType ) ;
	////	return false ;
	////}

	// msg give to room process 
	stMsgToRoom* pRoomMsg = (stMsgToRoom*)prealMsg;
	IRoomInterface* pRoom = GetRoomByID(pRoomMsg->nRoomID) ;
	if ( pRoom == NULL )
	{
		LOGFMTE("can not find room to process id = %d ,from = %d, room id = %d",prealMsg->usMsgType,eSenderPort,pRoomMsg->nRoomID ) ;
		return  false ;
	}

	return pRoom->onMessage(prealMsg,eSenderPort,nSessionID) ;
}

bool IRoomManager::onMsg(Json::Value& prealMsg ,uint16_t nMsgType, eMsgPort eSenderPort , uint32_t nSessionID)
{
	if ( IGlobalModule::onMsg(prealMsg,nMsgType,eSenderPort,nSessionID) )
	{
		return true ;
	}

	if ( MSG_REQ_VIP_ROOM_BILL_INFO == nMsgType )
	{
		uint32_t nBillID = prealMsg["billID"].asUInt();
		if ( isHaveVipRoomBill(nBillID) )
		{
			sendVipRoomBillToPlayer(nBillID,nSessionID) ;
			return true ;
		}

		// add to requesting queue ;
		auto iter = m_vReqingBillInfoPlayers.find(nBillID) ;
		if ( iter == m_vReqingBillInfoPlayers.end() )
		{
			auto p = std::shared_ptr<stReqVipRoomBillPlayers>(new stReqVipRoomBillPlayers());
			p->nReqBillID = nBillID ;
			p->vReqPlayers.insert(nSessionID);
			m_vReqingBillInfoPlayers[nBillID] = p ;
		}
		else
		{
			auto p = iter->second ;
			p->vReqPlayers.insert(nSessionID);
		}
		// read from db ;
		auto async = getSvrApp()->getAsynReqQueue();
		Json::Value jsReq ;
		std::ostringstream ss ;
		ss << "select roomID,roomType,createUID,unix_timestamp(billTime) as bTime,detail,roomInitCoin,circleCnt from viproombills where billID = " << nBillID << " ;" ;
		jsReq["sql"] = ss.str();
		Json::Value jsUserData ;
		jsUserData["billID"] = nBillID ;
		async->pushAsyncRequest(ID_MSG_PORT_DB,eAsync_DB_Select,jsReq,[this](uint16_t nReqType ,const Json::Value& retContent,Json::Value& jsUserData){
			uint8_t nAfcRow = retContent["afctRow"].asUInt() ;
			uint32_t nBillID = jsUserData["billID"].asUInt() ;
			if ( nAfcRow == 1 )
			{
				auto jsRow = retContent["data"][(uint32_t)0] ;
				auto pBill = createVipRoomBill();
				--s_MaxBillID;
				pBill->nBillID = nBillID ;
				pBill->nBillTime = jsRow["bTime"].asUInt();
				pBill->nCircleCnt = jsRow["circleCnt"].asUInt();
				pBill->nCreateUID = jsRow["createUID"].asUInt();
				pBill->nRoomID = jsRow["roomID"].asUInt();
				pBill->nRoomInitCoin = jsRow["roomInitCoin"].asUInt();
				pBill->nRoomType = jsRow["roomType"].asUInt();

				Json::Reader jsRead ;
				jsRead.parse(jsRow["detail"].asString(),pBill->jsDetail,false) ;
				addVipRoomBill(pBill,false) ;
			}

			// send all req players ;
			auto iter = m_vReqingBillInfoPlayers.find(nBillID) ;
			if ( iter == m_vReqingBillInfoPlayers.end() )
			{
				LOGFMTE("here must error , must have players waiting the result") ;
				return ;
			}

			for ( auto& nSessionID : iter->second->vReqPlayers )
			{
				sendVipRoomBillToPlayer(nBillID,nSessionID) ;
			}
			m_vReqingBillInfoPlayers.erase(iter) ;
			
		},jsUserData) ;

		return true;
	}

	// msg give to room process 
	if ( prealMsg["dstRoomID"].isNull() )
	{
		if ( nMsgType == MSG_PLAYER_LEAVE_ROOM )
		{
			Json::Value jsMsg ;
			jsMsg["ret"] = 1 ;
			sendMsg(jsMsg,nMsgType,nSessionID);
			LOGFMTE("player leave room msg , lack of dstRoom argument") ;
			return true ;
		}
		return false ;
	}

	IRoomInterface* pRoom = GetRoomByID(prealMsg["dstRoomID"].asUInt()) ;
	if ( pRoom == NULL )
	{
		LOGFMTE("can not find room to process id = %d ,from = %d, room id = %d",nMsgType,eSenderPort,prealMsg["dstRoomID"].asUInt() ) ;
		return  false ;
	}

	return pRoom->onMsg(prealMsg,nMsgType,eSenderPort,nSessionID) ;
}

bool IRoomManager::onPublicMsg(stMsg* prealMsg , eMsgPort eSenderPort , uint32_t nSessionID)
{
	switch ( prealMsg->usMsgType )
	{
	case MSG_GET_MAX_ROOM_ID:
		{
			stMsgGetMaxRoomIDRet* pRet = (stMsgGetMaxRoomIDRet*)prealMsg ;
			m_nMaxRoomID = pRet->nMaxRoomID ;
		}
		break;
	case MSG_READ_MY_OWN_ROOMS:
		{
			stMsgReadMyOwnRooms* pRet = (stMsgReadMyOwnRooms*)prealMsg ;
			VEC_INT vRL ;
			if ( getPrivateRooms(pRet->nUserUID,vRL) == false )
			{
				LOGFMTD("uid = %d do not create room so , need not respone list" ,pRet->nUserUID ) ;
				return true ;
			}

			stMsgReadMyOwnRoomsRet msgRead ;
			msgRead.nCnt = vRL.size() ;
			msgRead.nRoomType = getMgrRoomType();
			CAutoBuffer auBuffer(msgRead.nCnt * sizeof(stMyOwnRoom) + sizeof(msgRead));
			auBuffer.addContent(&msgRead,sizeof(msgRead)) ;
			stMyOwnRoom info ;
			for ( auto& nRoomIDs : vRL )
			{
				info.nRoomID = nRoomIDs ;
				auBuffer.addContent(&info,sizeof(info)) ;
			}
			sendMsg((stMsg*)auBuffer.getBufferPtr(),auBuffer.getContentSize(),nSessionID) ;
			LOGFMTD("respone uid = %d have owns cnt = %d",pRet->nUserUID,vRL.size()) ;
		}
		break;
	case MSG_READ_ROOM_INFO:
		{
			stMsgReadRoomInfoRet* pRet = (stMsgReadRoomInfoRet*)prealMsg ;
			LOGFMTD("read room info room id = %d",pRet->nRoomID);
			IRoomInterface* pRoom = doCreateRoomObject((eRoomType)pRet->nRoomType,pRet->nRoomOwnerUID != MATCH_MGR_UID ) ;
			Json::Reader jsReader ;
			Json::Value jsRoot;
			CAutoBuffer auBufo (pRet->nJsonLen + 1 );
			auBufo.addContent( ((char*)pRet) + sizeof(stMsgReadRoomInfoRet),pRet->nJsonLen);
			jsReader.parse(auBufo.getBufferPtr(),jsRoot);
			stBaseRoomConfig* pConfig = m_pConfigMgr->GetConfigByConfigID(pRet->nConfigID) ;
			if ( pConfig == nullptr )
			{
				delete pRoom ;
				pRoom = nullptr ;
				LOGFMTE("read room info , room = %d , config = %d is null",pRet->nRoomID,pRet->nConfigID) ;
				break;
			}
			pRoom->serializationFromDB(this,pConfig,pRet->nRoomID,jsRoot);

			if ( pRet->nRoomID > m_nMaxRoomID )
			{
				m_nMaxRoomID = pRet->nRoomID ;
			}

			if ( pRoom->getOwnerUID() == 0 )
			{
				addRoomToSystem(pRoom) ;
			}
			else
			{
				addRoomToPrivate(pRoom->getOwnerUID(),pRoom);
			}
		}
		break;
	case MSG_SVR_ENTER_ROOM:
		{
			stMsgSvrEnterRoomRet msgBack ;
			msgBack.nRet = 0 ;
			stMsgSvrEnterRoom* pRet = (stMsgSvrEnterRoom*)prealMsg ;
			msgBack.nGameType = getMgrRoomType() ;
			msgBack.nRoomID = pRet->nTargetID ;
			bool isRobot = pRet->tPlayerData.nPlayerType == ePlayer_Robot;
			if (isRobot)
			{
				LOGFMTI("收到机器人进入房间的请求 = %u",pRet->tPlayerData.nUserUID);
			}
			//// temp set 
			//pRet->nType = 1 ;
			//pRet->nTargetID = 2 ;
			LOGFMTD("session id = %u enter room type = %u , roomID = %u",nSessionID,pRet->nType,pRet->nTargetID) ;
			IRoomInterface* pRoomEnter = nullptr ;
			if ( pRet->nType == 1 )
			{
				pRoomEnter = GetRoomByID(pRet->nTargetID) ;
				if ( pRoomEnter == nullptr )
				{
					msgBack.nRet = 5 ;
					sendMsg(&msgBack,sizeof(msgBack),nSessionID) ;
					LOGFMTD("target room id = %u is null",pRet->nTargetID) ;
					if (isRobot)
					{
						LOGFMTI("机器人进入房间失败 uid = %u",pRet->tPlayerData.nUserUID);
					}
					break;
				}

				// robot adjust coin deponed on room limit ;
				if (pRet->tPlayerData.nPlayerType == ePlayer_Robot)
				{
					uint32_t nStandCoin = 8000 + rand() % 1000 ;
					auto pConfig = m_pConfigMgr->GetConfigByConfigID(pRoomEnter->getConfigID());
					if ( !pConfig)
					{
						LOGFMTE("why this room config id is null ? id = %u",pRoomEnter->getConfigID());
						assert(0&&"room config is null");
					}
					else
					{
						uint32_t nLowLimit = 1000;
						uint32_t nTopLimit = 3000;
						if (pConfig->nEnterLowLimit != 0)
						{
							nLowLimit = pConfig->nEnterLowLimit;
						}

						if (0 != pConfig->nEnterTopLimit)
						{
							nTopLimit = pConfig->nEnterTopLimit;
						}
						else
						{
							nTopLimit = nLowLimit * 2;
						}

						if (nTopLimit <= nLowLimit)
						{
							nTopLimit = nLowLimit * 2;
						}

						nStandCoin = nLowLimit + rand() % (nTopLimit - nLowLimit);
						nStandCoin = float(nStandCoin) * 1.1f;
						LOGFMTD("adjust robot coin to : %u" , nStandCoin );
					}

					//auto pMJRoom = dynamic_cast<ISitableRoom*>(pRoomEnter);
					//if (pMJRoom)
					//{

					//}

					//if (pRet->tPlayerData.nCoin < nStandCoin)
					{
						pRet->tPlayerData.nCoin = nStandCoin;
						LOGFMTE("temp set robot coin uid = %u", pRet->tPlayerData.nUserUID);
					}
				}

				msgBack.nRet = pRoomEnter->canPlayerEnterRoom(&pRet->tPlayerData) ;
				msgBack.nRoomID = pRoomEnter->getRoomID() ;
				if ( msgBack.nRet )
				{
					sendMsg(&msgBack,sizeof(msgBack),nSessionID) ;
					LOGFMTD("you are not proper to enter this room target id = %u , ret = %d",pRet->nTargetID,msgBack.nRet) ;
					if (isRobot)
					{
						LOGFMTI("机器人进入房间失败 uid = %u", pRet->tPlayerData.nUserUID);
					}
					
					break;
				}

				if (isRobot)
				{
					LOGFMTI("机器人进入房间成功 ，会有room info 返回 uid = %u", pRet->tPlayerData.nUserUID);
				}
				
			}
			else
			{
				//LOGFMTE("temp set enter er ren que shen %s",__FUNCTION__);
				//pRet->nTargetID = 7 ;
				auto pConfig = m_pConfigMgr->GetConfigByConfigID(pRet->nTargetID) ;
				if ( pConfig == nullptr )
				{
					msgBack.nRet = 2 ;
					sendMsg(&msgBack,sizeof(msgBack),nSessionID) ;
					LOGFMTE("svr do not have room config with id = %u",pRet->nTargetID );
					break; 
				}


				if ( pRet->tPlayerData.nPlayerType == ePlayer_Robot )
				{
					LOGFMTE("机器人不应该走到这里");
				}
				else
				{
					if ( pRet->tPlayerData.nCoin < pConfig->nEnterLowLimit && pConfig->nEnterLowLimit != 0 )
					{
						msgBack.nRet = 3 ;
						sendMsg(&msgBack,sizeof(msgBack),nSessionID) ;
						break; 
					}

					if ( pRet->tPlayerData.nCoin > pConfig->nEnterTopLimit && pConfig->nEnterTopLimit != 0 )
					{
						msgBack.nRet = 4 ;
						sendMsg(&msgBack,sizeof(msgBack),nSessionID) ;
						break; 
					}
				}


				VEC_INT vSysRooms  ;
				getSystemRooms(pRet->nTargetID,vSysRooms) ;
				std::vector<IRoomInterface*> vCanEnterRoom ;
				for ( auto& nRoomID : vSysRooms )
				{
					IRoomInterface* pCheckRoom = GetRoomByID(nRoomID) ;
					if ( pCheckRoom->canPlayerEnterRoom(&pRet->tPlayerData) == 0 )
					{
						vCanEnterRoom.push_back(pCheckRoom) ;
					}
				}

				if ( vCanEnterRoom.empty() )
				{
					Json::Value vDefault ;
					IRoomInterface* pNewRoom = doCreateInitedRoomObject(++m_nMaxRoomID,false,pRet->nTargetID,(eRoomType)pConfig->nGameType,vDefault);
					addRoomToSystem(pNewRoom) ;
					vCanEnterRoom.push_back(pNewRoom) ;
					LOGFMTD("create a new for player to enter , config id = %u",pRet->nTargetID) ;
				}

				pRoomEnter = vCanEnterRoom[rand() % vCanEnterRoom.size()];
			}
			
			msgBack.nRet = pRoomEnter->canPlayerEnterRoom(&pRet->tPlayerData) ;
			msgBack.nRoomID = pRoomEnter->getRoomID() ;
			if ( msgBack.nRet )
			{
				sendMsg(&msgBack,sizeof(msgBack),nSessionID) ;
				LOGFMTD("you are not proper to enter this room target id = %u , ret = %d",pRet->nTargetID,msgBack.nRet) ;
				if (isRobot)
				{
					LOGFMTI("机器人进入房间失败 uid = %u", pRet->tPlayerData.nUserUID);
				}
				break;
			}

			int8_t nidx = 0 ;
			pRoomEnter->onPlayerEnterRoom(&pRet->tPlayerData,nidx);
			msgBack.nGameType = eRoom_MJ ;
			msgBack.nRoomID = pRoomEnter->getRoomID() ;
			msgBack.nSubIdx = 0 ;
			msgBack.nRet = 0 ;
			sendMsg(&msgBack,sizeof(msgBack),nSessionID) ;

			if (isRobot)
			{
				LOGFMTI("机器人进入房间成功 ，会有room info 返回 , 进入房间成功消息也发 uid = %u", pRet->tPlayerData.nUserUID);
			}
			LOGFMTI("有人进入房间成功 ，会有room info 返回 , 进入房间成功消息也发 uid = %u", pRet->tPlayerData.nUserUID);
		}
		break;
	case MSG_REQUEST_ROOM_LIST:
		{
			VEC_INT vRL ;
			if ( getPrivateRooms(MATCH_MGR_UID,vRL) == false )
			{
				LOGFMTD("system not create room so , need not respone list" ) ;
				return true ;
			}

			stMsgRequestRoomListRet msgRet ;
			msgRet.nRoomCnt = vRL.size() ;
			msgRet.nRoomType = getMgrRoomType();
			CAutoBuffer auBuffer(sizeof(msgRet) + sizeof(uint32_t) * msgRet.nRoomCnt );
			auBuffer.addContent((char*)&msgRet,sizeof(msgRet)) ;
			for ( auto& pRoom : vRL )
			{
				uint32_t nid = pRoom;
				auBuffer.addContent(&nid,sizeof(nid)) ;
			}
			sendMsg((stMsg*)auBuffer.getBufferPtr(),auBuffer.getContentSize(),nSessionID);
			LOGFMTD("send msg room list ") ;
		}
		break;
	case MSG_REQUEST_ROOM_ITEM_DETAIL:
		{
			stMsgRequestRoomItemDetail* pret = (stMsgRequestRoomItemDetail*)prealMsg ;
			auto pRoom = GetRoomByID(pret->nRoomID);

			stMsgRequestRoomItemDetailRet msgRet ;
			msgRet.nRet = 1 ;
			msgRet.nRoomType = getMgrRoomType() ;
			msgRet.nRoomID = pret->nRoomID ;
			msgRet.nOwnerUID = 0 ;
			if ( pRoom )
			{
				if ( pRoom->getOwnerUID() != MATCH_MGR_UID )
				{
					msgRet.nOwnerUID = pRoom->getOwnerUID() ;
				}

				msgRet.nRet = 0 ;
				Json::Value vDetail ;
				pRoom->roomItemDetailVisitor(vDetail) ;
				Json::StyledWriter write ;
				std::string strDe = write.write(vDetail);
				msgRet.nJsonLen = strDe.size() ;
				CAutoBuffer auBffer (sizeof(msgRet) + msgRet.nJsonLen );
				auBffer.addContent(&msgRet,sizeof(msgRet)) ;
				auBffer.addContent(strDe.c_str(),msgRet.nJsonLen) ;
				sendMsg((stMsg*)auBffer.getBufferPtr(),auBffer.getContentSize(),nSessionID) ;
				LOGFMTD("send item detial room id = %d detail: %s",msgRet.nRoomID,strDe.c_str()) ;
			}
			else
			{
				msgRet.nJsonLen = 0 ;
				sendMsg(&msgRet,sizeof(msgRet),nSessionID) ;
				LOGFMTE("can not find room detail room id = %d",msgRet.nRoomID) ;
			}
		}
		break;
	default:
		return false;
	}
	return true ;
}

IRoomInterface* IRoomManager::GetRoomByID(uint32_t nRoomID )
{
	MAP_ID_ROOM::iterator iter = m_vRooms.find(nRoomID) ;
	if ( iter != m_vRooms.end() )
	{
		return iter->second ;
	}
	return NULL ;
}

void IRoomManager::update(float fDelta )
{
	IGlobalModule::update(fDelta);
	// check delete rooms ;
	LIST_ROOM vDoDelteRoom ;

	MAP_ID_ROOM::iterator iter = m_vRooms.begin() ;
	for ( ; iter != m_vRooms.end() ; ++iter )
	{
		if ( iter->second )
		{
			iter->second->update(fDelta) ;
		}

		// for check delete 
		if ( iter->second->isDeleteRoom())
		{
			vDoDelteRoom.push_back(iter->second) ;
		}
	}

	for ( IRoomInterface* pRoom : vDoDelteRoom )
	{
		removePrivateRoom(pRoom);
	}
}

void IRoomManager::onHttpCallBack(char* pResultData, size_t nDatalen , void* pUserData , size_t nUserTypeArg)
{
	if ( nUserTypeArg == eCrossSvrReq_CreateRoom )
	{
		uint32_t nChatRoomID = 0 ;
		bool bSuccess = nDatalen > 0 ;
		if ( bSuccess )
		{
			Json::Reader reader ;
			Json::Value cValue ;
			reader.parse(pResultData,pResultData + nDatalen,cValue) ;
			bSuccess = cValue["errcode"].asInt() == 200 ;
			nChatRoomID = cValue["room_id"].asUInt();
			LOGFMTD("error code = %d room id = %d",cValue["errcode"].asInt(), cValue["room_id"].asUInt() );

		}

		IRoom* pRoom = (IRoom*)pUserData ;
		if ( bSuccess )
		{
			pRoom->setChatRoomID(nChatRoomID);
		}
		else
		{
			LOGFMTE("request chat room id error ") ;
		}
	}
	else if ( nUserTypeArg == eCrossSvrReq_DeleteRoom )
	{
		bool bSuccess = nDatalen > 0 ;
		if ( bSuccess )
		{
			Json::Reader reader ;
			Json::Value cValue ;
			reader.parse(pResultData,pResultData + nDatalen,cValue) ;
			bSuccess = cValue["errcode"].asInt() == 200 ;
			LOGFMTD("delete room chat id code = %d",cValue["errcode"].asInt());
		}
	}
}

bool IRoomManager::onCrossServerRequest(stMsgCrossServerRequest* pRequest , eMsgPort eSenderPort,Json::Value* vJsValue)
{
	if ( eCrossSvrReq_CreateRoom == pRequest->nRequestType )
	{
		uint16_t nConfigID = (uint16_t)pRequest->vArg[0];
		assert(vJsValue&&"must not be null");

		stMsgCrossServerRequestRet msgRet ;
		msgRet.cSysIdentifer = eSenderPort ;
		msgRet.nReqOrigID = pRequest->nTargetID ;
		msgRet.nTargetID = pRequest->nReqOrigID ;
		msgRet.nRequestType = pRequest->nRequestType ;
		msgRet.nRequestSubType = pRequest->nRequestSubType ;
		msgRet.nRet = 0 ;
		msgRet.vArg[0] = pRequest->vArg[0];
		msgRet.vArg[1] = 0 ;
		msgRet.vArg[2] = pRequest->vArg[2] ;
		msgRet.vArg[3] = pRequest->vArg[1] ;
		Json::Value vCreateJs = *vJsValue;
		vCreateJs["ownerUID"] = (uint32_t)pRequest->nReqOrigID;
		LOGFMTD("recived create room uid = %d",pRequest->nReqOrigID) ;
		IRoomInterface* pRoom = doCreateInitedRoomObject(((uint32_t)time(nullptr)) % 1000000,pRequest->nReqOrigID != MATCH_MGR_UID,nConfigID,(eRoomType)pRequest->vArg[2],vCreateJs);
		if ( pRoom == nullptr )
		{
			--m_nMaxRoomID;
			msgRet.nRet = 1;
			sendMsg(&msgRet,sizeof(msgRet),msgRet.nTargetID);
			return true ;
		}

		if ( pRoom->getOwnerUID() == 0 )
		{
			addRoomToSystem(pRoom) ;
		}
		else
		{
			addRoomToPrivate(pRoom->getOwnerUID(),pRoom);
		}

		msgRet.vArg[1] = pRoom->getRoomID();
		msgRet.vArg[2] = pRoom->getRoomType() ;
		sendMsg(&msgRet,sizeof(msgRet),msgRet.nTargetID);
		return true ;
	}
	else if ( eCrossSvrReq_DeleteRoom == pRequest->nRequestType )
	{
		IRoomInterface* pRoom = GetRoomByID((uint32_t)pRequest->vArg[1] );
		if ( pRoom == nullptr )
		{
			LOGFMTE("uid = %d delete room = %d , not exsit",pRequest->nReqOrigID,pRequest->vArg[1]) ;
			return true ;
		}
		pRoom->deleteRoom();
		return true ;
	}
	else if ( eCrossSvrReq_ApplyLeaveRoom == pRequest->nRequestType )
	{
		IRoomInterface* pRoom = GetRoomByID(pRequest->nTargetID );
		if ( pRoom )
		{
			if ( pRoom->onPlayerApplyLeaveRoom(pRequest->nReqOrigID) )
			{

			}
			else
			{
				stMsgCrossServerRequest msgEnter ;
				msgEnter.cSysIdentifer = ID_MSG_PORT_DATA ;
				msgEnter.nJsonsLen = 0 ;
				msgEnter.nReqOrigID = pRoom->getRoomID();
				msgEnter.nRequestSubType = eCrossSvrReqSub_Default ;
				msgEnter.nRequestType = eCrossSvrReq_LeaveRoomRet ;
				msgEnter.nTargetID = pRequest->nReqOrigID ;
				msgEnter.vArg[0] = pRoom->getRoomType() ;
				msgEnter.vArg[1] = pRoom->getRoomID() ;
				sendMsg(&msgEnter,sizeof(msgEnter),pRoom->getRoomID()) ;
				LOGFMTD("you are not in room but i let you go!") ;
			}
		}
		else
		{
			stMsgCrossServerRequest msgEnter ;
			msgEnter.cSysIdentifer = ID_MSG_PORT_DATA ;
			msgEnter.nJsonsLen = 0 ;
			msgEnter.nReqOrigID = pRequest->nTargetID;
			msgEnter.nRequestSubType = eCrossSvrReqSub_Default ;
			msgEnter.nRequestType = eCrossSvrReq_LeaveRoomRet ;
			msgEnter.nTargetID = pRequest->nReqOrigID ;
			msgEnter.vArg[0] = pRequest->vArg[2] ;
			msgEnter.vArg[1] = pRequest->nTargetID ;
			sendMsg(&msgEnter,sizeof(msgEnter),pRequest->nTargetID);
			LOGFMTD("can not find room ,  but i let you go!") ;
		}
		return true ;
	}
	return false ;
}

bool IRoomManager::onCrossServerRequestRet(stMsgCrossServerRequestRet* pResult,Json::Value* vJsValue)
{
	return false ;
}

bool IRoomManager::reqeustChatRoomID(IRoom* pRoom)
{
#ifdef _DEBUG
	return true ;
#endif // _DEBUG

	Json::Value cValue ;
	cValue["email"] = "378569952@qq.com" ;
	cValue["devpwd"] = "bill007" ;
	cValue["appkey"] = "abffee4b-deea-4e96-ac8d-b9d58f246c3f" ;
	cValue["room_name"] = pRoom->getRoomID() ;
	cValue["room_type"] = 1;
	cValue["room_create_type"] = 0 ;
	Json::StyledWriter sWrite ;
	std::string str = sWrite.write(cValue);
	return m_pGoTyeAPI.performRequest("CreateRoom",str.c_str(),str.size(),pRoom,eCrossSvrReq_CreateRoom);
}

void IRoomManager::onConnectedSvr()
{
	IGlobalModule::onConnectedSvr();
	stMsgGetMaxRoomID msgBack ;
	sendMsg(&msgBack,sizeof(msgBack),0) ;

	if ( m_vRooms.empty() )
	{
		stMsgReadRoomInfo msgRead ;
		msgRead.nRoomType = getMgrRoomType() ;
		sendMsg(&msgRead,sizeof(msgRead),0) ;
		//LOGFMTE("test stage do not read room info");
		LOGFMTD("read room info ") ;
	}


	auto asyq = getSvrApp()->getAsynReqQueue();
	Json::Value jsReq ;
	jsReq["sql"] = "SELECT max(billID) as maxBillID FROM viproombills ;" ;
	asyq->pushAsyncRequest(ID_MSG_PORT_DB,eAsync_DB_Select,jsReq,[](uint16_t nReqType ,const Json::Value& retContent,Json::Value& jsUserData){
		uint32_t nAft = retContent["afctRow"].asUInt() ;
		auto jsData = retContent["data"] ;
		if ( nAft == 0 || jsData.isNull() )
		{
			LOGFMTE("read max bill id error ") ;
			return ;
		}

		auto jsRow = jsData[(uint32_t)0] ;
		s_MaxBillID = jsRow["maxBillID"].asUInt();
		LOGFMTD("max bill id  = %u",s_MaxBillID ) ;
	}) ;
}

void IRoomManager::addRoomToPrivate(uint32_t nOwnerUID ,IRoomInterface* pRoom)
{
	assert(pRoom->getOwnerUID() > 0 && "private create room, ownerUID must not be 0 " );
	MAP_UID_CR::iterator iter =  m_mapPrivateRooms.find(nOwnerUID);
	if ( iter != m_mapPrivateRooms.end() )
	{
		iter->second.vRoomIDs.push_back(pRoom->getRoomID()) ;
	}
	else
	{
		stRoomCreatorInfo sInfo ;
		sInfo.nPlayerUID = nOwnerUID ;
		sInfo.vRoomIDs.push_back(pRoom->getRoomID()) ;
		m_mapPrivateRooms[sInfo.nPlayerUID] = sInfo ;
	}

	auto room = m_vRooms.find(pRoom->getRoomID()) ;
	assert(room == m_vRooms.end() && "why have duplicate room ids? or this room ptr already added to vRooms " );
	m_vRooms[pRoom->getRoomID()] = pRoom ;
	LOGFMTD("room id = %u add to system room",pRoom->getRoomID()) ;
}

void IRoomManager::addRoomToSystem(IRoomInterface* pRoom)
{
	assert(pRoom->getOwnerUID() == 0 && "system create room, ownerUID must be 0 " );
	auto iterSysRoom = m_vSystemRooms.find( pRoom->getConfigID()) ;
	if ( iterSysRoom == m_vSystemRooms.end() )
	{
		stSystemRoomInfo stInfo ;
		stInfo.nConfigID = pRoom->getConfigID() ;
		stInfo.vRoomIDs.push_back(pRoom->getRoomID()) ;
		m_vSystemRooms[stInfo.nConfigID] = stInfo ;
	}
	else
	{
		iterSysRoom->second.vRoomIDs.push_back(pRoom->getRoomID());
	}
	m_vRooms[pRoom->getRoomID()] = pRoom ;
	LOGFMTD("add to system room config id = %u ,room id = %u",pRoom->getConfigID(),pRoom->getRoomID()) ;
}

bool IRoomManager::getPrivateRooms(uint32_t nCreatorUID, VEC_INT& vInfo )
{
	MAP_UID_CR::iterator iter = m_mapPrivateRooms.find(nCreatorUID) ;
	if ( iter == m_mapPrivateRooms.end() )
	{
		return false ;
	}
	vInfo = iter->second.vRoomIDs ;
	return true ;
}

bool IRoomManager::getSystemRooms(uint32_t nconfigID,VEC_INT& vRoomIDsInfo )
{
	auto iter = m_vSystemRooms.find(nconfigID) ;
	if ( iter == m_vSystemRooms.end() )
	{
		return false ;
	}
	vRoomIDsInfo = iter->second.vRoomIDs ;
	return true ;
}

void IRoomManager::sendVipRoomBillToPlayer( uint32_t nBillID , uint32_t nTargetSessionD )
{
	Json::Value jsMsg ;
	jsMsg["ret"] = 0 ;
	jsMsg["billID"] = nBillID ;

	if ( !isHaveVipRoomBill(nBillID) )
	{
		jsMsg["ret"] = 1 ;
		sendMsg(jsMsg,MSG_REQ_VIP_ROOM_BILL_INFO,nTargetSessionD) ;
		return ;
	}

	auto pBill = m_vVipRoomBills.find(nBillID)->second ;
	jsMsg["billTime"] = pBill->nBillTime;
	jsMsg["circle"] = pBill->nCircleCnt ;
	jsMsg["creatorUID"] = pBill->nCreateUID ;
	jsMsg["roomID"] = pBill->nRoomID ;
	jsMsg["initCoin"] = pBill->nRoomInitCoin ;
	jsMsg["roomType"] = pBill->nRoomType ;
	jsMsg["detail"] = pBill->jsDetail ;
	sendMsg(jsMsg,MSG_REQ_VIP_ROOM_BILL_INFO,nTargetSessionD) ;
}

void IRoomManager::addVipRoomBill(std::shared_ptr<stVipRoomBill>& pBill, bool isAddtoDB )
{
	if ( isHaveVipRoomBill(pBill->nBillID) )
	{
		LOGFMTE("already have this bill id = %u",pBill->nBillID) ;
		return ;
	}

	m_vVipRoomBills[pBill->nBillID] = pBill ;

	if ( isAddtoDB )
	{
		auto asy = getSvrApp()->getAsynReqQueue();
		Json::Value jsReq ;
		char pBuffer[500] = { 0 } ;
		Json::StyledWriter jsWrite ;
		auto str = jsWrite.write(pBill->jsDetail) ;
		sprintf_s(pBuffer,sizeof(pBuffer),"insert into viproombills (billID,roomID,roomType,createUID,billTime,detail,roomInitCoin ,circleCnt ) values( %u,%u,%u,%u,now(),'%s',%u;"
			,pBill->nBillID,pBill->nRoomID,pBill->nRoomType,pBill->nCreateUID,str.c_str(),pBill->nRoomInitCoin,pBill->nCircleCnt ) ;
		jsReq["sql"] = pBuffer ;
		asy->pushAsyncRequest(ID_MSG_PORT_DB,eAsync_DB_Add,jsReq) ;
	}
}

bool IRoomManager::isHaveVipRoomBill(uint32_t nVipBillID )
{
	auto iter = m_vVipRoomBills.find(nVipBillID);
	return iter != m_vVipRoomBills.end();
}

IRoomManager::VIP_ROOM_BILL_SHARED_PTR IRoomManager::createVipRoomBill()
{
	auto p = VIP_ROOM_BILL_SHARED_PTR(new stVipRoomBill() );
	p->nBillID = ++s_MaxBillID ;
	return p ;
}

void IRoomManager::onTimeSave()
{
	IGlobalModule::onTimeSave();
	MAP_ID_ROOM::iterator iter = m_vRooms.begin() ;
	for ( ; iter != m_vRooms.end() ; ++iter )
	{
		if ( iter->second )
		{
			iter->second->onTimeSave();
		}
	}
}

void IRoomManager::removePrivateRoom( IRoomInterface* pRoom )
{
	// remove m_vCreatorAndRooms ; maybe sys create
	bool bisPrivateRoom = false ;
	auto iter_Create = m_mapPrivateRooms.find( pRoom->getOwnerUID() ) ;
	if ( iter_Create != m_mapPrivateRooms.end() )
	{
		VEC_INT& list = iter_Create->second.vRoomIDs ;
		auto iterC = list.begin() ;
		for ( ; iterC != list.end(); ++iterC )
		{
			if ( (*iterC) == pRoom->getRoomID() )
			{
				list.erase(iterC) ;
				bisPrivateRoom = true ;
				break; ;
			}
		}
	}

	if ( bisPrivateRoom == false )
	{
		LOGFMTD("room id = %u is not private room , so can not deleted from private room",pRoom->getRoomID()) ;
		return ;
	}
	// remove from db ;
	stMsgSaveDeleteRoom msgSaveDelte ;
	msgSaveDelte.nRoomID = pRoom->getRoomID() ;
	msgSaveDelte.nRoomType = pRoom->getRoomType() ;
	sendMsg(&msgSaveDelte,sizeof(msgSaveDelte),0 ) ;

	// remove m_vRooms ;
	auto iter = m_vRooms.find(pRoom->getRoomID()) ;
	if ( iter != m_vRooms.end() )
	{
		m_vRooms.erase(iter) ;
	}

	delete pRoom ;
	pRoom = nullptr ;
	LOGFMTD("deleted private room id = %u ",msgSaveDelte.nRoomID) ;
}

void IRoomManager::deleteRoomChatID( uint32_t nChatID )
{
#ifdef _DEBUG
	return ;
#endif // _DEBUG
	// delete chat room id ;
	Json::Value cValue ;
	cValue["email"] = "378569952@qq.com" ;
	cValue["devpwd"] = "bill007" ;
	cValue["appkey"] = "abffee4b-deea-4e96-ac8d-b9d58f246c3f" ;
	cValue["room_id"] = nChatID ;
	Json::StyledWriter sWrite ;
	std::string str = sWrite.write(cValue);
	m_pGoTyeAPI.performRequest("DeleteRoom",str.c_str(),str.size(),nullptr,eCrossSvrReq_DeleteRoom);

	LOGFMTD("delte chat room id = %u",nChatID) ;
}

bool IRoomManager::onAsyncRequest(uint16_t nRequestType, const Json::Value& jsReqContent, Json::Value& jsResult)
{
	if (eAsync_SendUpdateCoinToClient != nRequestType)
	{
		return false;
	}

	uint32_t nSessionID = jsReqContent["sessionID"].asUInt();
	uint32_t nUID = jsReqContent["uid"].asUInt();
	uint32_t nCoin = jsReqContent["coin"].asUInt();
	uint32_t nDiamond = jsReqContent["diamond"].asUInt();
	uint32_t nRoomID = jsReqContent["roomID"].asUInt();

	auto pRoom = GetRoomByID(nRoomID);
	if (pRoom)
	{
		Json::Value js = jsReqContent;
		bool bProcessed = pRoom->onMsg(js, MSG_INTERAL_ROOM_SEND_UPDATE_COIN, ID_MSG_PORT_DATA, nSessionID);
		if (!bProcessed)
		{
			LOGFMTE("find the room you stay in to update coin msg, but can not find you uid = %u",nUID);
			Json::Value jsmsgBack;
			jsmsgBack["coin"] = nCoin;
			jsmsgBack["diamond"] = nDiamond;
			sendMsg(jsmsgBack, MSG_REQ_UPDATE_COIN, nSessionID);
		}
	}
	else
	{
		LOGFMTE("can not find the room you stay in to update coin msg  uid = %u",nUID);
		Json::Value jsmsgBack;
		jsmsgBack["coin"] = nCoin;
		jsmsgBack["diamond"] = nDiamond;
		sendMsg(jsmsgBack, MSG_REQ_UPDATE_COIN,nSessionID);

	}
	return true;
}

