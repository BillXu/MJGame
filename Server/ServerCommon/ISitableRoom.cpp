#include "ISitableRoom.h"
#include "RoomConfig.h"
#include "ISitableRoomPlayer.h"
#include <cassert>
#include "MessageDefine.h"
#include "AutoBuffer.h"
#include "ServerMessageDefine.h"
#include "log4z.h"
#include <json/json.h>
#include "ServerStringTable.h"
#include "SeverUtility.h"
#include <time.h>
#include "IRoomDelegate.h"
#include <algorithm>
#include "IPeerCard.h"
#include "RobotDispatchStrategy.h"
#include "log4z.h"
#include "IRoomState.h"
ISitableRoom::~ISitableRoom()
{
	for ( uint8_t nIdx = 0 ; nIdx < m_nSeatCnt ; ++nIdx )
	{
		if ( m_vSitdownPlayers[nIdx])
		{
			delete m_vSitdownPlayers[nIdx];
			m_vSitdownPlayers[nIdx] = nullptr ;
		}
	}
	delete [] m_vSitdownPlayers;

	for ( ISitableRoomPlayer* pPlayer : m_vReserveSitDownObject )
	{
		delete pPlayer ;
		pPlayer = nullptr ;
	}
	m_vReserveSitDownObject.clear() ;

	delete m_pRobotDispatchStrage ;
	m_pRobotDispatchStrage = nullptr ;
}

bool ISitableRoom::onFirstBeCreated(IRoomManager* pRoomMgr,stBaseRoomConfig* pConfig, uint32_t nRoomID, Json::Value& vJsValue ) 
{
	IRoom::onFirstBeCreated(pRoomMgr,pConfig,nRoomID,vJsValue) ;
	stBaseRoomConfig* pC = pConfig;
	m_nSeatCnt = pC->nMaxSeat ;
	m_vSitdownPlayers = new ISitableRoomPlayer*[m_nSeatCnt] ;
	for ( uint8_t nIdx = 0 ; nIdx < m_nSeatCnt ; ++nIdx )
	{
		m_vSitdownPlayers[nIdx] = nullptr ;
	}

	m_pRobotDispatchStrage = new CRobotDispatchStrategy() ;

	m_pRobotDispatchStrage->init(this,0,getRoomID(),nRoomID);
	return true ;
}

void ISitableRoom::serializationFromDB(IRoomManager* pRoomMgr,stBaseRoomConfig* pConfig,uint32_t nRoomID , Json::Value& vJsValue )
{
	IRoom::serializationFromDB(pRoomMgr,pConfig,nRoomID,vJsValue);
	auto pC = pConfig;
	m_nSeatCnt = pC->nMaxSeat ;
	m_vSitdownPlayers = new ISitableRoomPlayer*[m_nSeatCnt] ;
	for ( uint8_t nIdx = 0 ; nIdx < m_nSeatCnt ; ++nIdx )
	{
		m_vSitdownPlayers[nIdx] = nullptr ;
	}

	m_pRobotDispatchStrage = new CRobotDispatchStrategy() ;
	m_pRobotDispatchStrage->init(this,0,getRoomID(),nRoomID);
}

void ISitableRoom::willSerializtionToDB(Json::Value& vOutJsValue)
{
	IRoom::willSerializtionToDB(vOutJsValue);
}

void ISitableRoom::roomItemDetailVisitor(Json::Value& vOutJsValue)
{
	IRoom::roomItemDetailVisitor(vOutJsValue);
	vOutJsValue["playerCnt"] = getSitDownPlayerCount();
}

uint8_t ISitableRoom::canPlayerEnterRoom( stEnterRoomData* pEnterRoomPlayer )  // return 0 means ok ;
{
	auto pPlayer = getSitdownPlayerByUID(pEnterRoomPlayer->nUserUID);
	if ( pPlayer )
	{
		if ((pPlayer->isHaveState(eRoomPeer_DecideLose) == false))
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}



	uint8_t nRet = IRoom::canPlayerEnterRoom(pEnterRoomPlayer) ;
	if ( nRet )
	{
		return nRet ;
	}

	if ( pEnterRoomPlayer->nCoin < coinNeededToSitDown() )
	{
		return 3;
	}

	if ( getCurRoomState()->getStateID() != eRoomSate_WaitReady) // already full ;
	{
		return 1;
	}

	for ( uint8_t nidx = 0 ; nidx < getSeatCount() ; ++nidx )
	{
		auto pp = getPlayerByIdx(nidx) ;
		if ( pp == nullptr || pp->getUserUID() == pEnterRoomPlayer->nUserUID )
		{
			return 0 ;
		}
	} 

	return 8 ;
}

void ISitableRoom::onPlayerEnterRoom(stEnterRoomData* pEnterRoomPlayer,int8_t& nSubIdx )
{
	bool bNewEnter = false ;
	nSubIdx = 0 ;
	IRoom::stStandPlayer* pInPlayer = getPlayerByUserUID(pEnterRoomPlayer->nUserUID) ;
	if ( pInPlayer )
	{
		pInPlayer->nUserSessionID = pEnterRoomPlayer->nUserSessionID ;
		pInPlayer->isWillLeave = false ;
		LOGFMTD("player uid = %u , come back room ",pEnterRoomPlayer->nUserSessionID) ;
	}
	else
	{
		IRoom::onPlayerEnterRoom(pEnterRoomPlayer,nSubIdx) ;
		bNewEnter = true ;
	}

	sendRoomInfo(pEnterRoomPlayer->nUserSessionID);
	sendRoomPlayersCardInfo(pEnterRoomPlayer->nUserSessionID);
	LOGFMTD("uid = %u , enter room id = %u , subIdx = %u",pEnterRoomPlayer->nUserUID, getRoomID(),0) ;

	auto pSitPlayer = getSitdownPlayerByUID(pEnterRoomPlayer->nUserUID) ;
	if ( pSitPlayer )
	{
		pSitPlayer->setSessionID(pEnterRoomPlayer->nUserSessionID) ;
		LOGFMTD("player uid = %u , come back room , rebind session id ",pEnterRoomPlayer->nUserUID) ;
	}
	else
	{
		LOGFMTD("player uid = %u not sit , system auto sit",pEnterRoomPlayer->nUserUID) ;
		stMsgPlayerSitDown msgSitDown ;
		msgSitDown.nIdx = 0 ;
		msgSitDown.nSubRoomIdx = 0 ;
		msgSitDown.nTakeInCoin = 0 ;
		onMessage(&msgSitDown,ID_MSG_PORT_CLIENT,pEnterRoomPlayer->nUserSessionID) ;
	}

	if ( bNewEnter )
	{
		getRobotDispatchStrage()->onPlayerJoin(pEnterRoomPlayer->nUserSessionID,pEnterRoomPlayer->nPlayerType == ePlayer_Robot );
	}
}

bool ISitableRoom::onPlayerApplyLeaveRoom(uint32_t nUserUID )
{
	auto pp = getPlayerByUserUID(nUserUID) ;

	if ( pp == nullptr )
	{
		return true ;
	}

	uint32_t nSessionID = pp->nUserSessionID ;
	bool isRobot = pp->nPlayerType == ePlayer_Robot ;

	pp->isWillLeave = true ;

	if ( canPlayerDirectLeave(nUserUID) )
	{
		LOGFMTD("game is not running , so direct leave room") ;
		auto pS = getSitdownPlayerByUID(nUserUID) ;
		if ( pS == nullptr )
		{
			LOGFMTE("why you not sit down uid = %u",nUserUID) ;
		}
		else
		{
			playerDoStandUp(pS);
		}
		playerDoLeaveRoom(pp) ;

		getRobotDispatchStrage()->onPlayerLeave(nSessionID,isRobot);
	}
	else
	{
		auto pS = getSitdownPlayerByUID(nUserUID) ;
		if ( pS )
		{
			pS->delayStandUp() ;
		}
		else
		{
			LOGFMTE("why you are not sit down ? uid = %u",nUserUID) ;
		}
		LOGFMTD("game is running , so delay leave room , uid = %u",nUserUID) ;
	}
	return true ;
}

bool ISitableRoom::canStartGame()
{
	if ( IRoom::canStartGame() == false )
	{
		return false ;
	}

	return getPlayerCntWithState(eRoomPeer_WaitNextGame) >= 2 ;
}

void ISitableRoom::update(float fDelta)
{
	IRoom::update(fDelta);
	//m_pRobotDispatchStrage->update(fDelta) ;
}
//bool ISitableRoom::onPlayerSitDown(ISitableRoomPlayer* pPlayer , uint8_t nIdx )
//{
//	if ( isSeatIdxEmpty(nIdx) )
//	{
//		m_vSitdownPlayers[nIdx] = pPlayer ;
//		pPlayer->doSitdown(nIdx) ;
//		pPlayer->setIdx(nIdx);
//
//		// save standup log ;
//		stMsgSaveLog msgLog ;
//		msgLog.nJsonExtnerLen = 0 ;
//		msgLog.nLogType = eLog_PlayerSitDown ;
//		msgLog.nTargetID = pPlayer->getUserUID() ;
//		memset(msgLog.vArg,0,sizeof(msgLog.vArg)) ;
//		msgLog.vArg[0] = getRoomType() ;
//		msgLog.vArg[1] = getRoomID() ;
//		msgLog.vArg[2] = pPlayer->getCoin() ;
//		sendMsgToPlayer(&msgLog,sizeof(msgLog),getRoomID()) ;
//		return true ;
//	}
//	return false ;
//}

void ISitableRoom::playerDoStandUp( ISitableRoomPlayer* pPlayer )
{
	//m_pRobotDispatchStrage->onRobotLeave(pPlayer->getSessionID()) ;
	// remove from m_vSortByPeerCardsAsc ;

	Json::Value jsValue ;
	jsValue["idx"] = pPlayer->getIdx() ;
	sendRoomMsg(jsValue,MSG_ROOM_PLAYER_LEAVE) ;
	LOGFMTD("player uid = %u do leave room",pPlayer->getUserUID()) ;
	// remove other player data ;
	assert(isSeatIdxEmpty(pPlayer->getIdx()) == false && "player not sit down" );
	pPlayer->willStandUp();
	m_vSitdownPlayers[pPlayer->getIdx()] = nullptr ;
	auto standPlayer = getPlayerByUserUID(pPlayer->getUserUID()) ;
	
	if ( pPlayer->isHaveState(eRoomPeer_StayThisRound) )
	{
		LOGFMTD("uid = %d invoke game end , before stand up",pPlayer->getUserUID()) ;
		pPlayer->onGameEnd();
	}

	if ( standPlayer == nullptr )
	{
		if ( pPlayer->getCoin() > 0 )
		{
			stMsgSvrDelayedLeaveRoom msgdoLeave ;
			msgdoLeave.nCoin = pPlayer->getCoin() ;
			msgdoLeave.nGameType = getRoomType() ;
			msgdoLeave.nRoomID = getRoomID() ;
			msgdoLeave.nUserUID = pPlayer->getUserUID() ;
			msgdoLeave.nMaxFanShu = pPlayer->getMaxWinTimes()  ;
			msgdoLeave.nRoundsPlayed = pPlayer->getPlayTimes() ;
			msgdoLeave.nMaxFangXingType = pPlayer->getMaxWinCardType() ;
			msgdoLeave.nUserUID = pPlayer->getUserUID() ;
			msgdoLeave.nGameOffset = pPlayer->getTotalGameOffset() ;
			sendMsgToPlayer(&msgdoLeave,sizeof(msgdoLeave),pPlayer->getSessionID()) ;
			LOGFMTD("player uid = %d game end stand up sys coin = %d to data svr ",pPlayer->getUserUID(),pPlayer->getCoin()) ;
		}
		else
		{
			LOGFMTD("player uid = %d just stand up dely leave , but no coin",pPlayer->getUserUID() ) ;
		}
	}
	else
	{
		standPlayer->nCoin += pPlayer->getCoin() ;
		standPlayer->nNewPlayerHaloWeight = pPlayer->getHaloWeight() ;
		standPlayer->nRoundsPlayed += pPlayer->getPlayTimes();
		standPlayer->nGameOffset += pPlayer->getTotalGameOffset() ;
		if ( pPlayer->getMaxWinCardType() > standPlayer->nMaxFangXingType )
		{
			standPlayer->nMaxFangXingType = pPlayer->getMaxWinCardType() ;
		}

		if ( pPlayer->getMaxWinTimes() > standPlayer->nMaxFanShu )
		{
			standPlayer->nMaxFanShu = pPlayer->getMaxWinTimes() ;
		}

		LOGFMTD("player uid = %d just normal stand up ",pPlayer->getUserUID() ) ;
	}
	m_vReserveSitDownObject.push_back(pPlayer) ;
}

uint16_t ISitableRoom::getEmptySeatCount()
{
	uint16_t nCount = 0 ;
	for ( uint16_t nIdx = 0 ; nIdx < m_nSeatCnt ; ++nIdx )
	{
		if ( m_vSitdownPlayers[nIdx] == nullptr )
		{
			++nCount ;
		}
	}
	return nCount ;
}

bool ISitableRoom::onMsg(Json::Value& prealMsg, uint16_t nMsgType, eMsgPort eSenderPort, uint32_t nSessionID)
{
	if (IRoom::onMsg(prealMsg, nMsgType, eSenderPort, nSessionID))
	{
		return true;
	}

	if ( MSG_INTERAL_ROOM_SEND_UPDATE_COIN == nMsgType)
	{
		uint32_t nSessionIDThis = prealMsg["sessionID"].asUInt();
		uint32_t nUID = prealMsg["uid"].asUInt();
		uint32_t nCoin = prealMsg["coin"].asUInt();
		uint32_t nDiamond = prealMsg["diamond"].asUInt();
		uint32_t nRoomID = prealMsg["roomID"].asUInt();
		auto pSitp = getSitdownPlayerByUID(nUID);
		if (pSitp)
		{
			LOGFMTD("uid = %u data svr coin = %u , room sit down coin = %u",nCoin,pSitp->getCoin());
			nCoin = pSitp->getCoin();
		}
		else
		{
			auto pStand = getPlayerByUserUID(nUID);
			if (pStand)
			{
				LOGFMTD("uid = %u data svr coin = %u , room stand coin = %u", nCoin, pStand->nCoin );
				nCoin = pStand->nCoin;
			}
			else
			{
				LOGFMTE("update coin error ,you don't int ther room id = %u  , uid = %u",nRoomID,nUID);
			}
		}

		Json::Value jsmsgBack;
		jsmsgBack["coin"] = nCoin;
		jsmsgBack["diamond"] = nDiamond;
		sendMsgToPlayer(jsmsgBack, MSG_REQ_UPDATE_COIN, nSessionIDThis);
		return true;
	}

	if ( MSG_PLAYER_CHAT_MSG == nMsgType )
	{
		auto pRet = getSitdownPlayerBySessionID(nSessionID);
		if (pRet)
		{
			prealMsg["idx"] = pRet->getIdx();
			sendRoomMsg(prealMsg, MSG_ROOM_CHAT_MSG);
		}
		else
		{
			LOGFMTE("you do not sit down , can not say anything");
		}
		Json::Value jsRet;
		jsRet["ret"] = pRet == nullptr ? 1 : 0;
		sendMsgToPlayer(jsRet, nMsgType, nSessionID);
		return true;
	}
	return false;
}

ISitableRoomPlayer* ISitableRoom::getPlayerByIdx(uint16_t nIdx )
{
	assert(nIdx < getSeatCount() && "invalid player idx");
	if ( nIdx >= getSeatCount() )
	{
		return nullptr ;
	}
	return m_vSitdownPlayers[nIdx] ;
}

bool ISitableRoom::isSeatIdxEmpty( uint8_t nIdx )
{
	assert(nIdx < getSeatCount() && "invalid player idx");
	if ( nIdx >= getSeatCount() )
	{
		return false ;
	}
	return m_vSitdownPlayers[nIdx] == nullptr ;
}

uint16_t ISitableRoom::getSitDownPlayerCount()
{
	return getSeatCount() - getEmptySeatCount() ;
}

uint16_t ISitableRoom::getSeatCount()
{
	return m_nSeatCnt ;
}

ISitableRoomPlayer* ISitableRoom::getReuseSitableRoomPlayerObject()
{
	LIST_SITDOWN_PLAYERS::iterator iter = m_vReserveSitDownObject.begin() ;
	if ( iter != m_vReserveSitDownObject.end() )
	{
		ISitableRoomPlayer* p = *iter ;
		m_vReserveSitDownObject.erase(iter) ;
		return p ;
	}
	return doCreateSitableRoomPlayer();
}

uint16_t ISitableRoom::getPlayerCntWithState( uint32_t nState )
{
	uint8_t nCount = 0 ;
	for ( uint8_t nIdx = 0 ; nIdx < m_nSeatCnt ; ++nIdx )
	{
		if ( m_vSitdownPlayers[nIdx] && m_vSitdownPlayers[nIdx]->isHaveState(nState) )
		{
			++nCount ;
		}
	}
	return nCount ;
}

ISitableRoomPlayer* ISitableRoom::getSitdownPlayerBySessionID(uint32_t nSessionID)
{
	for ( uint8_t nIdx = 0 ; nIdx < m_nSeatCnt ; ++nIdx )
	{
		if ( m_vSitdownPlayers[nIdx] && m_vSitdownPlayers[nIdx]->getSessionID() == nSessionID )
		{
			return m_vSitdownPlayers[nIdx] ;
		}
	}
	return nullptr ;
}

ISitableRoomPlayer* ISitableRoom::getSitdownPlayerByUID(uint32_t nUserUID )
{
	for ( uint8_t nIdx = 0 ; nIdx < m_nSeatCnt ; ++nIdx )
	{
		if ( m_vSitdownPlayers[nIdx] && m_vSitdownPlayers[nIdx]->getUserUID() == nUserUID )
		{
			return m_vSitdownPlayers[nIdx] ;
		}
	}
	return nullptr ;
}

bool ISitableRoom::onMessage( stMsg* prealMsg , eMsgPort eSenderPort , uint32_t nPlayerSessionID )
{
	if ( IRoom::onMessage(prealMsg,eSenderPort,nPlayerSessionID) )
	{
		return true ;
	}

	switch ( prealMsg->usMsgType )
	{
	case MSG_REQ_CUR_GAME_OFFSET:
		{
			auto pPlayer = getSitdownPlayerBySessionID(nPlayerSessionID) ;
			if ( pPlayer )
			{
				stMsgReqRobotCurGameOffsetRet msgback ;
				msgback.nCurGameOffset = pPlayer->getTotalGameOffset();
				sendMsgToPlayer(&msgback,sizeof(msgback),nPlayerSessionID) ;
				LOGFMTD("robot req cur offset = %d , uid = %u",msgback.nCurGameOffset,pPlayer->getUserUID());
			}
		}
		break;
	case MSG_ADD_TEMP_HALO:
		{
			auto pPlayer = getPlayerBySessionID(nPlayerSessionID) ;
			if ( pPlayer == nullptr )
			{
				LOGFMTE("not in room player add temp halo session id = %u",nPlayerSessionID);
				break;
			}

			//if ( pPlayer->nPlayerType == ePlayer_Normal )
			//{
			//	LOGFMTE("normal can not add temp halo");
			//	break;
			//}

			stMsgAddTempHalo* pRet = (stMsgAddTempHalo*)prealMsg ;
			if ( 0 == pRet->nTargetUID )
			{
				pRet->nTargetUID = pPlayer->nUserUID ;
			}

			auto psitpp = getSitdownPlayerByUID(pRet->nTargetUID) ;
			if ( psitpp )
			{
				psitpp->setTempHaloWeight(pRet->nTempHalo);
				LOGFMTD("uid = %u add temp halo = %u",pRet->nTargetUID,pRet->nTempHalo) ;
			}
			else
			{
				LOGFMTE("uid = %u not sit down why add temp halo",pPlayer->nUserUID);
			}
		}
		break;
	case MSG_PLAYER_SITDOWN:
		{
			stMsgPlayerSitDown* pRet = (stMsgPlayerSitDown*)prealMsg ;

			stMsgPlayerSitDownRet msgBack ;
			msgBack.nRet = 0 ;

			stStandPlayer* pPlayer = getPlayerBySessionID(nPlayerSessionID) ;
			if ( !pPlayer )
			{
				LOGFMTE("palyer session id = %d ,not in this room so , can not sit down",nPlayerSessionID) ;
				msgBack.nRet = 3 ;
				sendMsgToPlayer(&msgBack,sizeof(msgBack),nPlayerSessionID) ;
				break; 
			}

			auto pp = getSitdownPlayerByUID(pPlayer->nUserUID);
			if ( pp )
			{
				assert(0 && "should not come here" );
				LOGFMTD("session id = %d , already sit down , just do resit down",nPlayerSessionID ) ;
				pp->reset(pPlayer) ;

				if ( pRet->nTakeInCoin == 0 || pRet->nTakeInCoin > pPlayer->nCoin)
				{
					pRet->nTakeInCoin = pPlayer->nCoin ;
				}

				pPlayer->nCoin -= pRet->nTakeInCoin ;

				Json::Value jsMsg ;
				jsMsg["idx"] = pp->getIdx() ;
				jsMsg["uid"] = pp->getUserUID() ;
				jsMsg["coin"] = pp->getCoin() ;
				sendRoomMsg(jsMsg,MSG_ROOM_PLAYER_ENTER);
				break;
			}

			
			if ( pRet->nTakeInCoin == 0 || pRet->nTakeInCoin > pPlayer->nCoin)
			{
				pRet->nTakeInCoin = pPlayer->nCoin ;
			}

			if ( pRet->nTakeInCoin < coinNeededToSitDown() )
			{
				msgBack.nRet = 1 ;
				sendMsgToPlayer(&msgBack,sizeof(msgBack),nPlayerSessionID) ;
				LOGFMTD("player coin is not enought so can not sit down session id = %u",nPlayerSessionID) ;
				break; 
			}

			//if ( isSeatIdxEmpty(pRet->nIdx) == false )
			//{
			//	msgBack.nRet = 2 ;
			//	sendMsgToPlayer(&msgBack,sizeof(msgBack),nPlayerSessionID) ;
			//	break; 
			//}

			for ( uint8_t nIdx = 0 ; nIdx < getSeatCount() ; ++nIdx )
			{
				if ( isSeatIdxEmpty(nIdx) )
				{
					pRet->nIdx = nIdx ;
					break; 
				}
			}

			if ( isSeatIdxEmpty(pRet->nIdx) == false )
			{
				msgBack.nRet = 2 ;
				sendMsgToPlayer(&msgBack,sizeof(msgBack),nPlayerSessionID) ;
				LOGFMTD("seat is not empty , session id = %u can not sit down",nPlayerSessionID) ;
				break; 
			}

			auto sitDownPlayer = getReuseSitableRoomPlayerObject() ;
			sitDownPlayer->reset(pPlayer) ;
			pPlayer->nCoin -= pRet->nTakeInCoin ;
			sitDownPlayer->setCoin(pRet->nTakeInCoin) ;
			sitDownPlayer->doSitdown(pRet->nIdx) ;
			sitDownPlayer->setIdx(pRet->nIdx);
			sitDownPlayer->setState(eRoomPeer_WaitNextGame );
			m_vSitdownPlayers[pRet->nIdx] = sitDownPlayer ;

			// tell others ;
			//stMsgRoomSitDown msgSitDown ;
			//msgSitDown.nIdx = sitDownPlayer->getIdx() ;
			//msgSitDown.nSitDownPlayerUserUID = sitDownPlayer->getUserUID() ;
			//msgSitDown.nTakeInCoin = sitDownPlayer->getCoin() ;
			//sendRoomMsg(&msgSitDown,sizeof(msgSitDown));

			Json::Value jsMsg ;
			jsMsg["idx"] = sitDownPlayer->getIdx() ;
			jsMsg["uid"] = sitDownPlayer->getUserUID() ;
			jsMsg["coin"] = sitDownPlayer->getCoin() ;
			sendRoomMsg(jsMsg,MSG_ROOM_PLAYER_ENTER);

			if ( pPlayer->nPlayerType == ePlayer_Robot )
			{
				LOGFMTD("robot uid = %d enter room",sitDownPlayer->getUserUID()) ;
				//m_pRobotDispatchStrage->onRobotJoin(sitDownPlayer->getSessionID());
			}
			LOGFMTD("a player sit down uid = %u",sitDownPlayer->getUserUID()) ;
		}
		break;
	case MSG_SYNC_IN_GAME_ADD_COIN:
		{
			stMsgSyncInGameCoin* pRet = (stMsgSyncInGameCoin*)prealMsg ;
			stMsgSyncInGameCoinRet msgback ;
			msgback.nRet = 0 ;
			msgback.nAddCoin = pRet->nAddCoin ;
			msgback.nRoomID = pRet->nRoomID ;
			msgback.nUserUID = pRet->nUserUID ;
			auto pSitPlayer = getSitdownPlayerByUID(pRet->nUserUID) ;
			do 
			{
				if ( pSitPlayer )
				{
					pSitPlayer->setCoin(pSitPlayer->getCoin() + pRet->nAddCoin) ;
					// send update coin msg to client ;
					Json::Value jsmsg ;
					jsmsg["idx"] = pSitPlayer->getIdx() ;
					jsmsg["coin"] = pSitPlayer->getCoin() ;
					sendRoomMsg(jsmsg,MSG_ROOM_PLAYER_COIN_UPDATE);
					break;
				}

				auto pStand = getPlayerByUserUID(pRet->nUserUID) ;
				if ( pStand )
				{
					pStand->nCoin += pRet->nAddCoin ;
					break;
				}
				msgback.nRet = 1 ;
			} while (0);
			sendMsgToPlayer(&msgback,sizeof(msgback),nPlayerSessionID) ;
			LOGFMTD("update in room player uid = %u, offset coin = %d",pRet->nUserUID,pRet->nAddCoin);
		}
		break ;
	default:
		return false;
	}
	return true ;
}

void ISitableRoom::onGameDidEnd()
{
	for ( uint8_t nIdx = 0 ; nIdx < m_nSeatCnt ; ++nIdx )
	{
		auto pPlayer = m_vSitdownPlayers[nIdx] ;
		if ( pPlayer == nullptr )
		{
			continue;
		}

		if ( pPlayer->isDelayStandUp() )
		{
			playerDoStandUp(pPlayer);	
			pPlayer = nullptr ;
			m_vSitdownPlayers[nIdx] = nullptr ;
			continue;
		}

		if ( pPlayer )
		{
			pPlayer->onGameEnd() ;
		}
	}
	IRoom::onGameDidEnd() ;
}

void ISitableRoom::onGameWillBegin()
{
	IRoom::onGameWillBegin() ;
	uint16_t nSeatCnt = getSeatCount() ;
	for ( uint8_t nIdx = 0; nIdx < nSeatCnt; ++nIdx )
	{
		ISitableRoomPlayer* pp = getPlayerByIdx(nIdx) ;
		if ( pp )
		{
			pp->setCoin(pp->getCoin() - getDeskFee() ) ;
			addTotoalProfit(getDeskFee());
			pp->onGameBegin();
		}
	}
}

bool sortPlayerByCard(ISitableRoomPlayer* pLeft , ISitableRoomPlayer* pRight )
{
	if ( pLeft->getPeerCard()->pk(pRight->getPeerCard()) == IPeerCard::PK_RESULT_FAILED )
	{
		return true ;
	}
	return false ;
}

uint8_t ISitableRoom::GetFirstInvalidIdxWithState( uint8_t nIdxFromInclude , eRoomPeerState estate )
{
	auto seatCnt = getSeatCount() ;
	for ( uint8_t nIdx = nIdxFromInclude ; nIdx < seatCnt * 2 ; ++nIdx )
	{
		uint8_t nRealIdx = nIdx % seatCnt ;
		if ( getPlayerByIdx(nRealIdx) == nullptr )
		{
			continue;
		}

		if ( getPlayerByIdx(nRealIdx)->isHaveState(estate) )
		{
			return nRealIdx ;
		}
	}
	LOGFMTE("why don't have peer with state = %d",estate ) ;
	return 0 ;
}

bool ISitableRoom::checkHavePlayerLoseOver(std::vector<uint8_t>& vLoseOverPlayerIdxs)
{
	for ( uint8_t nIdx = 0 ; nIdx < getSeatCount() ; ++nIdx )
	{
		auto pp = getPlayerByIdx(nIdx) ;
		if ( pp->getCoin() <= 0 )
		{
			vLoseOverPlayerIdxs.push_back(nIdx) ;
		}
	}

	return vLoseOverPlayerIdxs.empty() == false ;
}

uint8_t ISitableRoom::getIdxBySessionID(uint32_t nSessionID )
{
	auto p = getSitdownPlayerBySessionID(nSessionID) ;
	if ( !p )
	{
		return -1 ;
	}

	return p->getIdx() ;
}

void ISitableRoom::debugCoinInfo()
{
	LOGFMTI("输出玩家此刻金币详情：");
	for (uint8_t nIdx = 0; nIdx < m_nSeatCnt; ++nIdx)
	{
		auto pPlayer = m_vSitdownPlayers[nIdx];
		if (pPlayer == nullptr)
		{
			continue;
		}
		LOGFMTD("uid = %u , coin = %u , offset = %d",pPlayer->getUserUID(),pPlayer->getCoin(),pPlayer->getGameOffset());
	}
	LOGFMTI("金币详情结束！");
}