#include "IRoom.h"
#include <cassert>
#include "IRoomPlayer.h"
#include "IRoomState.h"
#include <time.h>
#include "AutoBuffer.h"
#include "ServerDefine.h"
#include "ServerMessageDefine.h"
#include "LogManager.h"
#include "RoomConfig.h"
#include <algorithm>
#include <json/json.h>
#include "IRoomDelegate.h"
#include "IRoomManager.h"
#define TIME_SECONDS_PER_DAY (60*60*24)

IRoom::IRoom()
{
	m_pDelegate = nullptr ;
	m_nRoomID = 0 ;
	m_vReseverPlayerObjects.clear();
	m_vInRoomPlayers.clear();
	m_vRoomStates.clear();
	m_pCurRoomState = nullptr ;

	m_nTotalProfit = 0;
	m_nChatRoomID = 0;

	m_bRoomInfoDiry = false ;
	m_pRoomMgr = nullptr ;
}

IRoom::~IRoom()
{
	for ( auto pPlayer : m_vReseverPlayerObjects )
	{
			if ( pPlayer )
			{
				delete pPlayer ;
				pPlayer = nullptr ;
			}
	}
	m_vReseverPlayerObjects.clear() ;

	MAP_UID_STAND_PLAYER::iterator iter = m_vInRoomPlayers.begin() ;
	for ( ; iter != m_vInRoomPlayers.end(); ++iter )
	{
		if ( iter->second )
		{
			delete iter->second ;
			iter->second = nullptr ;
		}
	}
	m_vInRoomPlayers.clear() ;

	MAP_ID_ROOM_STATE::iterator idState = m_vRoomStates.begin() ;
	for ( ; idState != m_vRoomStates.end() ; ++idState )
	{
		delete idState->second ;
		idState->second = nullptr ;
	}
	m_vRoomStates.clear() ;
}

bool IRoom::onFirstBeCreated(IRoomManager* pRoomMgr,stBaseRoomConfig* pConfig, uint32_t nRoomID, Json::Value& vJsValue )
{
	m_pRoomMgr = pRoomMgr ;
	m_nRoomID = nRoomID ;
	m_nDeskFree = pConfig->nDeskFee ;
	m_fDividFeeRate = 0 ;

	m_nChatRoomID = 0;
	m_nTotalProfit = 0 ;
	prepareState();
	return true ;
}

void IRoom::prepareState()
{
	// create room state ;
	//IRoomState* vState[] = {
	//	new IRoomStateWaitPlayerJoin(),new IRoomStateClosed(),new IRoomStateDidGameOver()
	//};
	//for ( uint8_t nIdx = 0 ; nIdx < sizeof(vState) / sizeof(IRoomState*); ++nIdx )
	//{
	//	addRoomState(vState[nIdx]) ;
	//}
	//setInitState(vState[0]);
}

void IRoom::serializationFromDB(IRoomManager* pRoomMgr,stBaseRoomConfig* pConfig ,uint32_t nRoomID , Json::Value& vJsValue )
{
	m_pRoomMgr = pRoomMgr ;
	m_nRoomID = nRoomID ;
	m_nTotalProfit = vJsValue["profit"].asUInt() ;;
	m_nChatRoomID = vJsValue["chatId"].asUInt() ;

	m_nDeskFree = pConfig->nDeskFee;
	m_fDividFeeRate = 0 ;
	prepareState();
}

void IRoom::serializationToDB()
{
	//Json::StyledWriter jsWrite ;
	//Json::Value vValue ;
	//willSerializtionToDB(vValue) ;
	//std::string strJson = jsWrite.write(vValue);

	//stMsgSaveUpdateRoomInfo msgSave ;
	//msgSave.bIsNewCreate = false ;
	//msgSave.nRoomType = getRoomType() ;
	//msgSave.nRoomID = getRoomID() ;
	//msgSave.nJsonLen = strJson.size() ;
	//msgSave.nConfigID = 0;
	//msgSave.nRoomOwnerUID = 0 ;

	//CAutoBuffer autoBuffer(sizeof(msgSave) + msgSave.nJsonLen);
	//autoBuffer.addContent((char*)&msgSave,sizeof(msgSave)) ;
	//autoBuffer.addContent(strJson.c_str(),msgSave.nJsonLen) ;
	//sendMsgToPlayer((stMsg*)autoBuffer.getBufferPtr(),autoBuffer.getContentSize(),0) ;

	//m_bRoomInfoDiry = false ;
}

void IRoom::willSerializtionToDB(Json::Value& vOutJsValue)
{
	vOutJsValue["chatId"] = m_nChatRoomID ;
	vOutJsValue["profit"] = m_nTotalProfit ;
}

uint8_t IRoom::canPlayerEnterRoom( stEnterRoomData* pEnterRoomPlayer )  // return 0 means ok ;
{
	//stStandPlayer* pp = getPlayerByUserUID(pEnterRoomPlayer->nUserUID);
	//if ( pp )
	//{
	//	CLogMgr::SharedLogMgr()->ErrorLog("player uid = %d , already in this room, can not enter twice",pEnterRoomPlayer->nUserUID) ;
	//	return 1;
	//}

	//if ( getDelegate() )
	//{
	//	return getDelegate()->canPlayerEnterRoom(this,pEnterRoomPlayer) ;
	//}
	return 0 ;
}

void IRoom::onPlayerEnterRoom(stEnterRoomData* pEnterRoomPlayer ,int8_t& nSubIdx)
{
	stStandPlayer* pp = getPlayerByUserUID(pEnterRoomPlayer->nUserUID);
	stStandPlayer * pStandPlayer = nullptr ;
	if ( pp )
	{
		CLogMgr::SharedLogMgr()->PrintLog("player uid = %d , already in this room, can not enter twice, data svr crashed ?",pEnterRoomPlayer->nUserUID) ;
		pStandPlayer = pp ;
		memcpy(pStandPlayer,pEnterRoomPlayer,sizeof(stEnterRoomData));
	}
	else
	{
		pStandPlayer = new stStandPlayer ;
		memset(pStandPlayer,0,sizeof(stStandPlayer));
		memcpy(pStandPlayer,pEnterRoomPlayer,sizeof(stEnterRoomData));
		addRoomPlayer(pStandPlayer) ;
	}
}

bool IRoom::canStartGame()
{
	if ( m_vInRoomPlayers.empty() )
	{
		//CLogMgr::SharedLogMgr()->PrintLog("room = %u have room player , so do not start game ",getRoomID());
		return false ;
	}
	// if have any player not robot ?
	if ( isHaveRealPlayer() )
	{
		return true ;
	}
	//CLogMgr::SharedLogMgr()->PrintLog("room = %u all player are robot so need not start game ",getRoomID());
	return false ;
}

bool IRoom::isHaveRealPlayer()
{
	for ( auto pp : m_vInRoomPlayers )
	{
		if ( pp.second->nPlayerType != ePlayer_Robot )
		{
			return true ;
		}
	}
	return false ;
}

void IRoom::roomItemDetailVisitor(Json::Value& vOutJsValue)
{
	vOutJsValue["state"] = getCurRoomState()->getStateID();
}

void IRoom::playerDoLeaveRoom( stStandPlayer* pp )
{
	// send msg to data svr tell player leave room ;
	if ( pp )
	{
		stMsgSvrDoLeaveRoom msgdoLeave ;
		msgdoLeave.nCoin = pp->nCoin ;
		msgdoLeave.nGameType = getRoomType() ;
		msgdoLeave.nRoomID = getRoomID() ;
		msgdoLeave.nUserUID = pp->nUserUID ;
		msgdoLeave.nWinTimes = pp->nWinTimes ;
		msgdoLeave.nPlayerTimes = pp->nPlayerTimes ;
		msgdoLeave.nSingleWinMost = pp->nSingleWinMost ;
		msgdoLeave.nGameOffset = pp->nGameOffset ;
		sendMsgToPlayer(&msgdoLeave,sizeof(msgdoLeave),pp->nUserSessionID) ;

		removePlayer(pp);
		CLogMgr::SharedLogMgr()->PrintLog("uid = %d , do leave this room ",msgdoLeave.nUserUID ) ;
	}
	else
	{
		CLogMgr::SharedLogMgr()->ErrorLog("player , not in this room can not do leave room" ) ;
	}
	
}

uint32_t IRoom::getRoomID()
{
	return m_nRoomID ;
}

void IRoom::update(float fDelta)
{
	m_pCurRoomState->update(fDelta);
}

bool IRoom::addRoomPlayer(stStandPlayer* pPlayer )
{
	assert(pPlayer&&"pPlayer is null") ;
	if ( isPlayerInRoom(pPlayer) )
	{
		return false;
	}
	m_vInRoomPlayers[pPlayer->nUserUID] = pPlayer ;
	return true ;
}

void IRoom::removePlayer(stStandPlayer* pPlayer )
{
	STAND_PLAYER_ITER iter = m_vInRoomPlayers.find(pPlayer->nUserUID) ;
	if ( iter != m_vInRoomPlayers.end() )
	{
		m_vReseverPlayerObjects.push_back(pPlayer);
		m_vInRoomPlayers.erase(iter) ;
	}
}

IRoom::stStandPlayer* IRoom::getPlayerByUserUID(uint32_t nUserUID )
{
	STAND_PLAYER_ITER iter = m_vInRoomPlayers.find(nUserUID) ;
	if ( iter != m_vInRoomPlayers.end() )
	{
		return iter->second ;
	}
	return nullptr ;
}

IRoom::stStandPlayer* IRoom::getPlayerBySessionID(uint32_t nSessionID )
{
	STAND_PLAYER_ITER iter = m_vInRoomPlayers.begin();
	for ( ; iter != m_vInRoomPlayers.end(); ++iter )
	{
		if ( iter->second->nUserSessionID == nSessionID )
		{
			return iter->second ;
		}
	}
	return nullptr ;
}

bool IRoom::isPlayerInRoom(stStandPlayer* pPlayer )
{
	return isPlayerInRoomWithUserUID(pPlayer->nUserUID);
}

bool IRoom::isPlayerInRoomWithSessionID(uint32_t nSessioID )
{
	return getPlayerBySessionID(nSessioID) != nullptr ;
}

bool IRoom::isPlayerInRoomWithUserUID(uint32_t nUserUID )
{
	return getPlayerByUserUID(nUserUID) != nullptr ;
}

uint16_t IRoom::getPlayerCount()
{
	return m_vInRoomPlayers.size() ;
}

IRoom::STAND_PLAYER_ITER IRoom::beginIterForPlayers()
{
	return m_vInRoomPlayers.begin() ;
}

IRoom::STAND_PLAYER_ITER IRoom::endIterForPlayers()
{
	return m_vInRoomPlayers.end() ;
}

void IRoom::sendRoomMsg( stMsg* pmsg , uint16_t nLen )
{
	STAND_PLAYER_ITER iter = m_vInRoomPlayers.begin() ;
	for ( ; iter != m_vInRoomPlayers.end() ; ++iter )
	{
		sendMsgToPlayer(pmsg,nLen,iter->second->nUserSessionID) ;
	}
}

void IRoom::sendMsgToPlayer( stMsg* pmsg , uint16_t nLen , uint32_t nSessionID )
{
	m_pRoomMgr->sendMsg(pmsg,nLen,nSessionID);
}

void IRoom::sendRoomMsg( Json::Value& jsContent , unsigned short nMsgType , eMsgPort ePort)
{
	STAND_PLAYER_ITER iter = m_vInRoomPlayers.begin() ;
	for ( ; iter != m_vInRoomPlayers.end() ; ++iter )
	{
		sendMsgToPlayer(jsContent,nMsgType,iter->second->nUserSessionID,ePort) ;
	}
}

void IRoom::sendMsgToPlayer( Json::Value& jsContent , unsigned short nMsgType , uint32_t nSessionID, eMsgPort ePort )
{
	m_pRoomMgr->sendMsg(jsContent,nMsgType,nSessionID,ePort);
}

bool IRoom::onMessage( stMsg* prealMsg , eMsgPort eSenderPort , uint32_t nPlayerSessionID )
{
	if ( m_pCurRoomState && m_pCurRoomState->onMessage(prealMsg,eSenderPort,nPlayerSessionID) )
	{
		return true ;
	}

	switch ( prealMsg->usMsgType )
	{
	case MSG_MODIFY_ROOM_RANK:
		{
			stMsgRobotModifyRoomRank* pRet = (stMsgRobotModifyRoomRank*)prealMsg ;
			if ( getDelegate() && getPlayerByUserUID(pRet->nTargetUID) )
			{
				getDelegate()->onUpdatePlayerGameResult(this,pRet->nTargetUID,pRet->nOffset);
				CLogMgr::SharedLogMgr()->SystemLog("modify uid = %u offset = %d",pRet->nTargetUID,pRet->nOffset) ;
			}
			else
			{
				CLogMgr::SharedLogMgr()->ErrorLog("modify room rank uid = %u not in room ", pRet->nTargetUID);
			}
		}
		break;
	default:
		return false ;
	}

	return true ;
}

bool IRoom::onMsg(Json::Value& prealMsg ,uint16_t nMsgType, eMsgPort eSenderPort , uint32_t nSessionID)
{
	if ( m_pCurRoomState && m_pCurRoomState->onMsg(prealMsg,nMsgType,eSenderPort,nSessionID) )
	{
		return true ;
	}
	return false ;
}


void IRoom::onGameDidEnd()
{
	STAND_PLAYER_ITER iter = m_vInRoomPlayers.begin();
	for ( ; iter != m_vInRoomPlayers.end();  )
	{
		if ( iter->second->isWillLeave )
		{
			playerDoLeaveRoom(iter->second) ;
			iter = m_vInRoomPlayers.begin() ;
		}
		else
		{
			++iter ;
		}
	}
}

void IRoom::onTimeSave( )
{
	//CLogMgr::SharedLogMgr()->PrintLog("time save room info room id = %u",getRoomID());

	//if ( m_bRoomInfoDiry )
	//{
	//	serializationToDB();
	//	m_bRoomInfoDiry = false ;
	//}
}

void IRoom::goToState(IRoomState* pTargetState )
{
	//assert(pTargetState != m_pCurRoomState && "go to the same state ? " );
	if ( pTargetState == m_pCurRoomState)
	{
		CLogMgr::SharedLogMgr()->SystemLog("go to the same state %d , room id = %d ? ",pTargetState->getStateID(), getRoomID() );
	}
	
	m_pCurRoomState->leaveState() ;
	m_pCurRoomState = pTargetState ;
	m_pCurRoomState->enterState(this) ;

	stMsgRoomEnterNewState msgNewState ;
	msgNewState.m_fStateDuring = m_pCurRoomState->getStateDuring();
	msgNewState.nNewState = m_pCurRoomState->getStateID();
	//sendRoomMsg(&msgNewState,sizeof(msgNewState)) ;
}

void IRoom::goToState( uint16_t nStateID )
{
	goToState(getRoomStateByID(nStateID)) ;
}

void IRoom::setInitState(IRoomState* pDefaultState )
{
	m_pCurRoomState = pDefaultState ;
	m_pCurRoomState->enterState(this);
}

IRoomState* IRoom::getCurRoomState()
{
	return m_pCurRoomState ;
}

IRoomState* IRoom::getRoomStateByID(uint16_t nStateID )
{
	MAP_ID_ROOM_STATE::iterator iter = m_vRoomStates.find(nStateID) ;
	if ( iter != m_vRoomStates.end() )
	{
		return iter->second ;
	}
	return nullptr ;
}

bool IRoom::addRoomState(IRoomState* pRoomState )
{
	assert(getRoomStateByID(pRoomState->getStateID()) == nullptr && "already added this state" ) ;
	m_vRoomStates[pRoomState->getStateID()] = pRoomState ;
	return true ;
}

void IRoom::setChatRoomID(uint32_t nChatRoomID )
{
	m_nChatRoomID = nChatRoomID ;
	m_bRoomInfoDiry = true ;
}

bool IRoom::isDeleteRoom()
{
	return false;
}
