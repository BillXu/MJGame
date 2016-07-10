#pragma once
#include "IRoomInterface.h"
#include "ISitableRoom.h"
#include "ISitableRoomPlayer.h"
#include "IRoomDelegate.h"
#include "Timer.h"
#include <map>
template<class T >
class CPrivateRoom
	:public IRoomInterface
	,public IRoomDelegate
{
public:
	typedef T REAL_ROOM ;
	typedef T* REAL_ROOM_PTR;
	struct stVipPlayer
	{
		uint32_t nSessionID ;
		uint32_t nUID ;
		uint32_t nRealCoin ;
		uint32_t nRoomCoin ;
	};
	typedef std::map<uint32_t,stVipPlayer> MAP_UID_VIP_PLAYERS;
public:
	CPrivateRoom();
	~CPrivateRoom();

	// interface 
	bool onFirstBeCreated(IRoomManager* pRoomMgr,stBaseRoomConfig* pConfig, uint32_t nRoomID, Json::Value& vJsValue )override;
	void serializationFromDB(IRoomManager* pRoomMgr,stBaseRoomConfig* pConfig,uint32_t nRoomID , Json::Value& vJsValue ) override;
	void serializationToDB() override;

	uint8_t canPlayerEnterRoom( stEnterRoomData* pEnterRoomPlayer )override;  // return 0 means ok ;
	void onPlayerEnterRoom(stEnterRoomData* pEnterRoomPlayer,int8_t& nSubIdx ) override;
	bool onPlayerApplyLeaveRoom(uint32_t nUserUID )override;
	void roomItemDetailVisitor(Json::Value& vOutJsValue) override;
	uint32_t getRoomID()override;
	uint8_t getRoomType() override ;
	void update(float fDelta) override;
	void onTimeSave() override;
	bool onMessage( stMsg* prealMsg , eMsgPort eSenderPort , uint32_t nPlayerSessionID )override;
	bool onMsg(Json::Value& prealMsg ,uint16_t nMsgType, eMsgPort eSenderPort , uint32_t nSessionID)override ;
	bool isDeleteRoom()override;
	void deleteRoom()override ;
	uint32_t getOwnerUID()override;
	uint32_t getConfigID()override{ return m_stConfig.nConfigID ;}

	// delegate msg ;
	uint32_t getWaitActTime(ISitableRoom* pRoom){ return 100000000 ;}
	void onDidGameOver(IRoom* pRoom)override ;
	void onPlayerLeave(IRoom* pRoom,uint8_t playerUID )override;
	bool canGameOver(IRoom* pRoom )override ;

	// self 
	uint32_t getRoomState(){ return m_eState ; }
	time_t getCloseTime(){ return m_tCloseTime ;}
	bool isRoomClosed();
	void sendRoomInfo(uint32_t nSessionID );
	void onRoomGameOver( bool isDismissed );
	void onCheckDismissReply( bool bTimerOut );
protected:
	IRoomManager* m_pRoomMgr ;
	uint32_t m_nRoomID ;
	uint32_t m_nOwnerUID ;

	uint8_t m_nLeftCircle ;
	uint8_t m_nInitCircle ;
	uint32_t m_nInitCoin ;
	bool m_bComsumedRoomCards ;
	MAP_UID_VIP_PLAYERS m_vAllPlayers ;

	std::map<uint32_t,uint8_t> m_mapRecievedReply ; // key : uid , value: replayer , 0  agree , 1 disagree ;
	bool m_bWaitDismissReply ;
	CTimer m_tWaitRepklyTimer ;
	bool m_bDoDismissRoom ;

	eRoomState m_eState ;
	stMJRoomConfig m_stConfig ;
	REAL_ROOM_PTR m_pRoom ;

	bool m_bRoomInfoDiry ; 
};




// implement 
#include "LogManager.h"
#include "RoomConfig.h"
#include "ServerMessageDefine.h"
#include "IRoomManager.h"
#include "SeverUtility.h"
#include "ServerStringTable.h"
#include "IRoomState.h"
#include "RewardConfig.h"
#include "AsyncRequestQuene.h"
#define TIME_WAIT_REPLY_DISMISS 60*5

template<class T >
CPrivateRoom<T>::CPrivateRoom()
{
	m_pRoomMgr = nullptr ;
	m_nRoomID = 0 ;
	m_nOwnerUID = 0;
	m_eState = eRoomState_None ;
	m_pRoom = nullptr;
	m_bRoomInfoDiry = false;

	m_nLeftCircle = m_nInitCircle = 0 ;
	m_nInitCoin = 0 ;
	m_bComsumedRoomCards = false ;
	m_mapRecievedReply.clear() ;
	m_bWaitDismissReply = false ;
	m_bDoDismissRoom = false ;
}

template<class T >
CPrivateRoom<T>::~CPrivateRoom()
{
	if ( m_pRoom )
	{
		delete m_pRoom ;
		m_pRoom = nullptr ;
	}
}

// interface 
template<class T >
bool CPrivateRoom<T>::onFirstBeCreated(IRoomManager* pRoomMgr,stBaseRoomConfig* pConfig, uint32_t nRoomID, Json::Value& vJsValue )
{
	m_nLeftCircle = vJsValue["circle"].asUInt() ;
	m_nInitCircle = m_nLeftCircle;
	m_nInitCoin = vJsValue["initCoin"].asUInt() ;
	memset(&m_stConfig,0,sizeof(m_stConfig));
	m_stConfig.nConfigID = 0 ;
	m_stConfig.nBaseBet = vJsValue["baseBet"].asUInt() ;
	m_stConfig.nMaxSeat = vJsValue["seatCnt"].asUInt() ;
	m_stConfig.nGameType = vJsValue["roomType"].asUInt() ;
	if ( m_stConfig.nMaxSeat == 0 || m_stConfig.nBaseBet == 0 )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("create private argument error seatCnt = %u , baseBet = %u",m_stConfig.nMaxSeat,m_stConfig.nBaseBet) ;
		m_stConfig.nMaxSeat = 4;
		m_stConfig.nBaseBet = 1 ;
	}

	m_bComsumedRoomCards = false ;
	m_mapRecievedReply.clear() ;
	m_bWaitDismissReply = false ;
	m_bDoDismissRoom = false ;
	

	m_nRoomID = nRoomID ;
	m_eState = eRoomState_Opening ;
	m_pRoomMgr = pRoomMgr ;
	m_bRoomInfoDiry = true ;

	if ( vJsValue["ownerUID"].isNull() == false )
	{
		m_nOwnerUID = vJsValue["ownerUID"].asUInt() ;
		CLogMgr::SharedLogMgr()->PrintLog("create private room ownerUID is = %u",m_nOwnerUID) ;
	}
	else
	{
		m_nOwnerUID = 0 ;
		CLogMgr::SharedLogMgr()->PrintLog("create private room ownerUID is null ?") ;
	}

	vJsValue["parentRoomID"] = getRoomID() ;

	m_pRoom = new REAL_ROOM ;
	m_pRoom->onFirstBeCreated(pRoomMgr,&m_stConfig,nRoomID ,vJsValue);
	pRoomMgr->reqeustChatRoomID(m_pRoom);
	m_pRoom->setDelegate(this);

	CLogMgr::SharedLogMgr()->PrintLog("create 1 private room") ;
	return true ;
}

template<class T >
void CPrivateRoom<T>::serializationFromDB(IRoomManager* pRoomMgr,stBaseRoomConfig* pConfig,uint32_t nRoomID , Json::Value& vJsValue )
{
	m_nLeftCircle = vJsValue["circle"].asUInt() ;
	m_nInitCoin = vJsValue["initCoin"].asUInt() ;
	memset(&m_stConfig,0,sizeof(m_stConfig));
	m_stConfig.nConfigID = 0 ;
	m_stConfig.nBaseBet = vJsValue["baseBet"].asUInt() ;
	m_stConfig.nMaxSeat = vJsValue["seatCnt"].asUInt() ;
	m_stConfig.nGameType = vJsValue["roomType"].asUInt() ;

	m_bComsumedRoomCards = false ;
	m_mapRecievedReply.clear() ;
	m_bWaitDismissReply = false ;
	m_bDoDismissRoom = false ;

	m_bRoomInfoDiry = false ;
	m_nRoomID = nRoomID ;
	m_pRoomMgr = pRoomMgr ;
	m_eState = eRoomState_Opening;
	
	m_nOwnerUID = vJsValue["ownerUID"].asUInt();
	
	vJsValue["parentRoomID"] = getRoomID() ;

	Json::Value subRoom = vJsValue["subRoom"];
	m_pRoom = new REAL_ROOM ;
	m_pRoom->serializationFromDB(pRoomMgr,&m_stConfig,nRoomID,subRoom);
	m_pRoom->setDelegate(this);

	// read rank data 
	stMsgReadRoomPlayer msgRead ;
	msgRead.nRoomID = getRoomID();
	msgRead.nRoomType = getRoomType() ;
	msgRead.nTermNumber = 0 ;
	m_pRoomMgr->sendMsg(&msgRead,sizeof(msgRead),getRoomID()) ;

	CLogMgr::SharedLogMgr()->PrintLog("read room id = %u ternm = %u rank player",getRoomID(),0) ;
}

template<class T >
void CPrivateRoom<T>::serializationToDB()
{
	Json::StyledWriter jsWrite ;
	Json::Value vValue ;

	vValue["state"] = m_eState ;
	vValue["ownerUID"] = getOwnerUID() ;

	Json::Value subRoom ;
	m_pRoom->willSerializtionToDB(subRoom);
	vValue["subRoom"] = subRoom ;

	std::string strJson = jsWrite.write(vValue);

	stMsgSaveUpdateRoomInfo msgSave ;
	msgSave.nRoomType = getRoomType() ;
	msgSave.nRoomID = getRoomID() ;
	msgSave.nJsonLen = strJson.size() ;
	msgSave.nConfigID = getConfigID();
	msgSave.nRoomOwnerUID = getOwnerUID() ;

	CAutoBuffer autoBuffer(sizeof(msgSave) + msgSave.nJsonLen);
	autoBuffer.addContent((char*)&msgSave,sizeof(msgSave)) ;
	autoBuffer.addContent(strJson.c_str(),msgSave.nJsonLen) ;
	m_pRoomMgr->sendMsg((stMsg*)autoBuffer.getBufferPtr(),autoBuffer.getContentSize(),0) ;
}

template<class T >
uint8_t CPrivateRoom<T>::canPlayerEnterRoom( stEnterRoomData* pEnterRoomPlayer )  // return 0 means ok ;
{
	if ( getRoomState() != eRoomState_Opening )
	{
		CLogMgr::SharedLogMgr()->PrintLog("vip room not open");
		return 7 ;  // room not open 
	}

	if ( m_pRoom )
	{
		return m_pRoom->canPlayerEnterRoom(pEnterRoomPlayer);
	}
	return 7 ;
}

template<class T >
void CPrivateRoom<T>::onPlayerEnterRoom(stEnterRoomData* pEnterRoomPlayer,int8_t& nSubIdx )
{
	pEnterRoomPlayer->nPlayerType = ePlayer_Robot ; // avoid robot dispatch take effect ;
	nSubIdx = 0 ;
	if ( m_pRoom )
	{
		auto iter = m_vAllPlayers.find(pEnterRoomPlayer->nUserUID);
		if ( iter == m_vAllPlayers.end() )
		{
			stVipPlayer svp ;
			svp.nSessionID = pEnterRoomPlayer->nUserSessionID ;
			svp.nUID = pEnterRoomPlayer->nUserUID ;
			svp.nRealCoin = pEnterRoomPlayer->nCoin ;
			svp.nRoomCoin = m_nInitCoin ;
			m_vAllPlayers[svp.nUID] = svp ;

			pEnterRoomPlayer->nCoin = svp.nRoomCoin ;
			CLogMgr::SharedLogMgr()->PrintLog("uid = %u first enter room give coin = %u",pEnterRoomPlayer->nUserUID,svp.nRoomCoin) ;
		}
		else
		{
			iter->second.nRealCoin = pEnterRoomPlayer->nCoin ;
			iter->second.nSessionID = pEnterRoomPlayer->nUserSessionID ;
			pEnterRoomPlayer->nCoin = iter->second.nRoomCoin ;
			CLogMgr::SharedLogMgr()->PrintLog("uid = %u  enter room again room coin = %u",pEnterRoomPlayer->nUserUID,iter->second.nRoomCoin) ;
		}
		m_pRoom->onPlayerEnterRoom(pEnterRoomPlayer,nSubIdx) ;
		sendRoomInfo(pEnterRoomPlayer->nUserSessionID) ;
	}
}

template<class T >
bool CPrivateRoom<T>::onPlayerApplyLeaveRoom(uint32_t nUserUID )
{
	if ( m_pRoom )
	{
		return m_pRoom->onPlayerApplyLeaveRoom(nUserUID) ;
	}
	return false ;
}

template<class T >
void CPrivateRoom<T>::roomItemDetailVisitor(Json::Value& vOutJsValue)
{
	vOutJsValue["configID"] = getConfigID() ;
	vOutJsValue["state"] = (uint32_t)getRoomState() ;
}

template<class T >
uint32_t CPrivateRoom<T>::getRoomID()
{
	return m_nRoomID ;
}

template<class T >
uint8_t CPrivateRoom<T>::getRoomType()
{
	if ( m_pRoom )
	{
		return m_pRoom->getRoomType();
	}
	return eRoom_None ;
}

template<class T >
bool CPrivateRoom<T>::isRoomClosed()
{
	if ( m_pRoom )
	{
		//return eRoomState_Close == m_pRoom->getCurRoomState()->getStateID();
	}
	return false ;
}

template<class T >
void CPrivateRoom<T>::update(float fDelta)
{
	if ( m_pRoom && eRoomState_Opening == getRoomState() )
	{
		m_pRoom->update(fDelta);
	}
}

template<class T >
void CPrivateRoom<T>::onTimeSave()
{
	//CLogMgr::SharedLogMgr()->PrintLog("time save room info room id = %u",getRoomID());
	// save room rank ;
	stMsgSaveRoomPlayer msgSave ;
	msgSave.nRoomID = getRoomID() ;
	msgSave.nRoomType = getRoomType() ;
	msgSave.nTermNumber = 0 ;
	for ( auto& pp : m_vSortedRankItems )
	{
		if ( !pp->bIsDiryt )
		{
			continue;
		}
		pp->bIsDiryt = false ;

		msgSave.savePlayer.nUserUID = pp->nUserUID ;
		msgSave.savePlayer.nGameOffset = pp->nGameOffset ;
		msgSave.savePlayer.nOtherOffset = pp->nOtherOffset ;
		m_pRoomMgr->sendMsg(&msgSave,sizeof(msgSave),0) ;
		CLogMgr::SharedLogMgr()->PrintLog("update rank uid = %u , offset = %d",pp->nUserUID,pp->nGameOffset) ;
	}

	if ( m_bRoomInfoDiry )
	{
		serializationToDB();
		m_bRoomInfoDiry = false ;
	}
}

template<class T >
bool CPrivateRoom<T>::onMessage( stMsg* prealMsg , eMsgPort eSenderPort , uint32_t nPlayerSessionID )
{
	switch ( prealMsg->usMsgType )
	{
	case MSG_READ_ROOM_PLAYER:
		{
			stMsgReadRoomPlayerRet* pRet = (stMsgReadRoomPlayerRet*)prealMsg ; 
			if ( pRet->nTermNumber != 0 )
			{
				CLogMgr::SharedLogMgr()->SystemLog("recieved last wrong term player data , skip id  room id = %u",getRoomID()) ;
				break; 
			}

			CLogMgr::SharedLogMgr()->PrintLog("room id = %d recive room player data cnt = %d",getRoomID(),pRet->nCnt) ;
			stSaveRoomPlayerEntry* pp = (stSaveRoomPlayerEntry*)(((char*)prealMsg) + sizeof(stMsgReadRoomPlayerRet));
			while ( pRet->nCnt-- )
			{
				stRoomRankItem* p = new stRoomRankItem ;
				p->bIsDiryt = false ;
				p->nUserUID = pp->nUserUID ;
				p->nGameOffset = pp->nGameOffset ;
				p->nOtherOffset = pp->nOtherOffset ;
				 // current term rank item ;
				m_vRoomRankHistroy[p->nUserUID] = p ;
				m_vSortedRankItems.push_back(p) ;
				++pp;
			}
		}
		break;
	case MSG_REQUEST_ROOM_RANK:
		{
			uint32_t nUserID = 0 ;
			if ( m_pRoom )
			{
				auto pp = m_pRoom->getPlayerBySessionID(nPlayerSessionID) ;
				if ( pp )
				{
					nUserID = pp->nUserUID ;
				}
			}

			std::map<uint32_t,stRoomRankEntry> vWillSend ;
			sortRoomRankItem();
			int16_t nSelfRankIdx = -1 ;
			uint16_t nSendRankCnt = 80 ;
			int16_t nMaxCheckCount = 80 ;
			// add 5 player into list ;
			LIST_ROOM_RANK_ITEM::iterator iter = m_vSortedRankItems.begin();
			for ( uint8_t nIdx = 0 ; iter != m_vSortedRankItems.end(); ++iter,++nIdx )
			{
				// push rank 5 to send to client ;
				stRoomRankItem* pItem = (*iter) ;
				if ( pItem->nUserUID == nUserID )
				{
					nSelfRankIdx = nIdx ;
				}

				if ( nIdx < nSendRankCnt || pItem->nUserUID == nUserID )
				{
					stRoomRankEntry entry ;
					entry.nGameOffset = pItem->nGameOffset ;
					entry.nOtherOffset = pItem->nOtherOffset ;
					entry.nUserUID = pItem->nUserUID ;
					vWillSend[pItem->nUserUID] = entry ;
				}
				else
				{
					if ( nSelfRankIdx != -1 || nUserID == 0 )  // find self or not in this room ;
					{
						break;
					}
				}

				--nMaxCheckCount ;
				if ( nMaxCheckCount <= 0 )
				{
					break;
				}
			}

			CLogMgr::SharedLogMgr()->PrintLog("uid = %d request rank room id = %u",nUserID,getRoomID());
			// send room info to player ;
			stMsgRequestRoomRankRet msgRet ;
			msgRet.nCnt = vWillSend.size() ;
			msgRet.nSelfRankIdx = nSelfRankIdx ;
			CAutoBuffer msgBuffer(sizeof(msgRet) + msgRet.nCnt * sizeof(stRoomRankEntry));
			msgBuffer.addContent(&msgRet,sizeof(msgRet));
			for ( auto& itemSendPlayer : vWillSend )
			{
				msgBuffer.addContent(&itemSendPlayer.second,sizeof(stRoomRankEntry));
				CLogMgr::SharedLogMgr()->PrintLog("room id = %u rank player uid = %u offset = %d",getRoomID(),itemSendPlayer.second.nUserUID,itemSendPlayer.second.nGameOffset);
			}
			m_pRoomMgr->sendMsg((stMsg*)msgBuffer.getBufferPtr(),msgBuffer.getContentSize(),nPlayerSessionID) ;
		}
		break;
	case MSG_REQUEST_ROOM_INFO:
		{
			stMsgToRoom* pRet = (stMsgToRoom*)prealMsg ;
			sendRoomInfo(nPlayerSessionID);
		}
		break;
	default:
		{
			if ( m_pRoom )
			{
				return m_pRoom->onMessage(prealMsg,eSenderPort,nPlayerSessionID) ;
			}
			else
			{
				 
			}
		}
		return false ;
	}

	return true ;
}

template<class T >
bool CPrivateRoom<T>::onMsg(Json::Value& prealMsg ,uint16_t nMsgType, eMsgPort eSenderPort , uint32_t nSessionID)
{
	switch ( nMsgType )
	{
	case MSG_APPLY_DISMISS_VIP_ROOM:
		{
			auto pp = m_pRoom->getSitdownPlayerBySessionID(nSessionID) ;
			if ( pp == nullptr )
			{
				CLogMgr::SharedLogMgr()->ErrorLog("pp is null why , you apply dismiss , but , you are not sit in room, session id = %u",nSessionID) ;
				return true;
			}

			m_mapRecievedReply[pp->getUserUID()] = 1 ;
			if ( m_bWaitDismissReply )
			{
				onCheckDismissReply(false);
			}
			else
			{
				Json::Value jsMsg ;
				jsMsg["applyerIdx"] = pp->getIdx() ;
				m_pRoom->sendRoomMsg(jsMsg,MSG_ROOM_APPLY_DISMISS_VIP_ROOM);
				m_tWaitRepklyTimer.setInterval(TIME_WAIT_REPLY_DISMISS);
				m_tWaitRepklyTimer.setIsAutoRepeat(false) ;
				m_tWaitRepklyTimer.setCallBack([this](CTimer*p ,float f){
				
					onCheckDismissReply(true) ;
				}) ;
				m_tWaitRepklyTimer.start() ;
				m_bWaitDismissReply = true ;
			}
		}
		break ;
	case MSG_REPLY_DISSMISS_VIP_ROOM_APPLY:
		{
			if ( !m_bWaitDismissReply )
			{
				CLogMgr::SharedLogMgr()->ErrorLog("nobody apply to dismiss room ,why you reply ? session id = %u",nSessionID) ;
				return true ;
			}

			auto pp = m_pRoom->getSitdownPlayerBySessionID(nSessionID) ;
			if ( pp == nullptr )
			{
				CLogMgr::SharedLogMgr()->ErrorLog("pp is null why , you apply dismiss , but , you are not sit in room, session id = %u",nSessionID) ;
				return true;
			}

			CLogMgr::SharedLogMgr()->PrintLog("received player session id = %u , reply dismiss ret = %u",nSessionID,prealMsg["reply"].asUInt()) ;
			m_mapRecievedReply[pp->getUserUID()] = prealMsg["reply"].asUInt();
			onCheckDismissReply(false);
		}
		break ;
	default:
		if ( m_pRoom )
		{
			return m_pRoom->onMsg(prealMsg,nMsgType,eSenderPort,nSessionID) ;
		}
	}

	return true ;
}

template<class T >
void CPrivateRoom<T>::onCheckDismissReply( bool bTimerOut )
{
	uint8_t nAgreeCnt =  0 ;
	uint8_t nDisAgreeCnt = 0 ;
	for ( auto& ref : m_mapRecievedReply )
	{
		if ( ref.second )
		{
			++nDisAgreeCnt ;
		}
		else
		{
			++nAgreeCnt ;
		}
	}

	uint8_t nSeatCnt = (uint8_t)m_pRoom->getSeatCount() ;
	if ( bTimerOut )
	{
		if ( ( nSeatCnt - nDisAgreeCnt) * 2 > nSeatCnt )  // not disagree means agree ;
		{
			CLogMgr::SharedLogMgr()->PrintLog("most player want dismiss room time out") ;
			onRoomGameOver(true) ;
		}
	}
	else
	{
		if ( nAgreeCnt * 2 > nSeatCnt )
		{
			CLogMgr::SharedLogMgr()->PrintLog("most player want dismiss room") ;
			onRoomGameOver(true) ;
		}
		else
		{
			return ;
		}
	}

	m_mapRecievedReply.clear() ;
	m_bWaitDismissReply = false ;
	m_tWaitRepklyTimer.canncel() ;
}

template<class T >
bool CPrivateRoom<T>::isDeleteRoom()
{
	return m_eState == eRoomState_Close ;
}

template<class T >
void CPrivateRoom<T>::deleteRoom()
{
	m_eState = eRoomState_Close ;
}

template<class T >
uint32_t CPrivateRoom<T>::getOwnerUID()
{
	return m_nOwnerUID ;
}

// delegate msg ;
template<class T >
void CPrivateRoom<T>::onDidGameOver(IRoom* pRoom )
{
	if ( m_bDoDismissRoom )
	{
		CLogMgr::SharedLogMgr()->PrintLog("do dismiss room so skip this game over event") ;
		return ;
	}

	if ( m_bComsumedRoomCards == false )
	{
		m_bComsumedRoomCards = true ;
		// comsum room card ;
		uint16_t nCardCnt = m_nLeftCircle / ROOM_CIRCLES_PER_VIP_ROOM_CARDS;
		CLogMgr::SharedLogMgr()->PrintLog("send msg to consumed vip room card") ;
		Json::Value jsConsumed ;
		jsConsumed["cardCnt"] = nCardCnt ;
		jsConsumed["uid"] = getOwnerUID() ;
		m_pRoomMgr->sendMsg(jsConsumed,MSG_CONSUM_VIP_ROOM_CARDS,0,ID_MSG_PORT_DATA) ;
	}

	// decrease circle ;
	std::vector<uint8_t> vLoseOver ;
	bool bHaveOver = m_pRoom->checkHavePlayerLoseOver(vLoseOver);
	--m_nLeftCircle ;
	if ( m_nLeftCircle > 0 && bHaveOver == false )
	{
		CLogMgr::SharedLogMgr()->PrintLog("vip room over , leftCircle = %u, haveLoseOverPlayer = %b",m_nLeftCircle,bHaveOver) ;
		return ;
	}

	// on game over ;
	onRoomGameOver(false);
}

template<class T >
void CPrivateRoom<T>::onRoomGameOver( bool isDismissed )
{
	// do close room ;
	m_eState = eRoomState_Close ;
	if ( isDismissed )
	{
		m_bDoDismissRoom = true ;
		auto stateID = m_pRoom->getCurRoomState()->getStateID() ;
		if ( stateID != eRoomState_GameEnd && stateID != eRoomSate_WaitReady  )
		{
			CLogMgr::SharedLogMgr()->PrintLog("vip room dismiss , but not in end state , so go to end state ") ;
			m_pRoom->goToState(eRoomState_GameEnd);
		}
	}
	// force all player leave room ;
	m_pRoom->forcePlayersLeaveRoom() ;
	// send room bills ;
	Json::Value jsMsg ;
	jsMsg["ret"] = isDismissed ? 1 : 0 ;
	jsMsg["initCoin"] = m_nInitCoin ;

	Json::Value jsVBills ;
	Json::Value jsPlayedPlayers;
	for ( auto& ref : m_vAllPlayers )
	{
		Json::Value jsPlayer ;
		jsPlayer["uid"] = ref.second.nUID;
		jsPlayer["curCoin"] = ref.second.nRoomCoin ;
		jsVBills[jsVBills.size()] = jsPlayer ;

		jsPlayedPlayers[jsPlayedPlayers.size()] = ref.second.nUID;
	}
	jsMsg["bills"] = jsVBills ;

	for ( auto& ref : m_vAllPlayers )
	{
		if ( ref.second.nSessionID )
		{
			CLogMgr::SharedLogMgr()->PrintLog("send game room over bill to session id = %u",ref.second.nSessionID) ;
			m_pRoomMgr->sendMsg(jsMsg,MSG_VIP_ROOM_GAME_OVER,ref.second.nSessionID );
		}
	}

	Json::Value jsClosed ;
	jsClosed["uid"] = getOwnerUID() ;
	jsClosed["roomID"] = getRoomID() ;
	jsClosed["eType"] = getRoomType();
	m_pRoomMgr->sendMsg(jsClosed,MSG_VIP_ROOM_CLOSED,0,ID_MSG_PORT_DATA);

	// add vip room bill 
	auto pBill = m_pRoomMgr->createVipRoomBill();
	pBill->jsDetail = jsVBills ;
	pBill->nBillTime = (uint32_t)time(nullptr);
	pBill->nCreateUID = getOwnerUID() ;
	pBill->nRoomID = getRoomID() ;
	pBill->nRoomType = getRoomType() ;
	pBill->nRoomInitCoin = m_nInitCoin ;
	pBill->nCircleCnt = m_nInitCircle - m_nLeftCircle ;
	m_pRoomMgr->addVipRoomBill(pBill,true ) ;

	// sys bill id to data svr 
	Json::Value jsReqSync ;
	jsReqSync["billID"] = pBill->nBillID;
	jsReqSync["useUIDs"] = jsPlayedPlayers ;
	auto asynQueue = m_pRoomMgr->getSvrApp()->getAsynReqQueue();
	asynQueue->pushAsyncRequest(ID_MSG_PORT_DATA,eAsync_SyncVipRoomBillID,jsReqSync) ;
}

template<class T >
void CPrivateRoom<T>::onPlayerLeave(IRoom* pRoom,uint8_t playerUID )
{
	auto pp = pRoom->getPlayerByUserUID(playerUID) ;
	if ( pp == nullptr )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("leave player uid = %u , already leave ? prt is null" ,playerUID ) ;
		assert(0 && "player ptr is null , for leave");
		return ;
	}

	auto iter = m_vAllPlayers.find(playerUID) ;
	if ( iter == m_vAllPlayers.end() )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("why enter player not register in vip room ?") ;
		assert(0 && "never shold come here" );
		return ;
	}

	iter->second.nRoomCoin = pp->nCoin ;

	stMsgSvrDoLeaveRoom msgdoLeave ;
	msgdoLeave.nCoin = iter->second.nRealCoin  ;
	msgdoLeave.nGameType = getRoomType() ;
	msgdoLeave.nRoomID = getRoomID() ;
	msgdoLeave.nUserUID = pp->nUserUID ;
	msgdoLeave.nMaxFangXingType = pp->nMaxFangXingType ;
	msgdoLeave.nMaxFanShu = pp->nMaxFanShu ;
	msgdoLeave.nRoundsPlayed = pp->nRoundsPlayed ;
	msgdoLeave.nGameOffset = pp->nGameOffset ;
	m_pRoom->sendMsgToPlayer(&msgdoLeave,sizeof(msgdoLeave),pp->nUserSessionID) ;

	CLogMgr::SharedLogMgr()->PrintLog("player uid = %u leave vip room",pp->nUserUID) ;
}

template<class T >
bool CPrivateRoom<T>::canGameOver(IRoom* pRoom )
{
	std::vector<uint8_t> vLoseOver ;
	return m_pRoom->checkHavePlayerLoseOver(vLoseOver);
}

template<class T >
void CPrivateRoom<T>::sendRoomInfo(uint32_t nSessionID )
{
	typename REAL_ROOM_PTR pRoom = m_pRoom ;
	if ( pRoom == nullptr )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("session id = %u requesiont room info room id = %u, subIdx = %d is null",nSessionID , getRoomID(),0) ;
		return ;
	}

	CLogMgr::SharedLogMgr()->PrintLog("send vip room info ext to player session id = %u",nSessionID) ;
	Json::Value jsMsg ;
	jsMsg["leftCircle"] = m_nLeftCircle ;
	jsMsg["baseBet"] = m_pRoom->getBaseBet();
	jsMsg["creatorUID"] = m_nOwnerUID ;
	jsMsg["initCoin"] = m_nInitCoin ;
	m_pRoomMgr->sendMsg(jsMsg,MSG_VIP_ROOM_INFO_EXT,nSessionID) ;
}