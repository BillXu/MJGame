#include "MJRoom.h"
#include "MJRoomPlayer.h"
#include "MJServer.h"
#include "MessageIdentifer.h"
#include "ServerMessageDefine.h"
#include "LogManager.h"
#include "AutoBuffer.h"
#include "ServerStringTable.h"
#include <algorithm>
#include <json/json.h>
CMJRoom::CMJRoom()
{
	
}

bool CMJRoom::onFirstBeCreated(IRoomManager* pRoomMgr,stBaseRoomConfig* pConfig, uint32_t nRoomID, Json::Value& vJsValue )
{
	ISitableRoom::onFirstBeCreated(pRoomMgr,pConfig,nRoomID,vJsValue) ;
	m_nBaseBet = ((stNiuNiuRoomConfig*)pConfig)->nBaseBet;

	return true ;
}

void CMJRoom::prepareState()
{
	ISitableRoom::prepareState();
	// create room state ;

}

void CMJRoom::serializationFromDB(IRoomManager* pRoomMgr,stBaseRoomConfig* pConfig,uint32_t nRoomID , Json::Value& vJsValue )
{
	ISitableRoom::serializationFromDB(pRoomMgr,pConfig,nRoomID,vJsValue);
	m_nBaseBet = ((stNiuNiuRoomConfig*)pConfig)->nBaseBet;
}

void CMJRoom::willSerializtionToDB(Json::Value& vOutJsValue)
{
	ISitableRoom::willSerializtionToDB(vOutJsValue);
}

void CMJRoom::roomItemDetailVisitor(Json::Value& vOutJsValue)
{
	ISitableRoom::roomItemDetailVisitor(vOutJsValue) ;
	//vOutJsValue["baseBet"] = getBaseBet() ;
}

ISitableRoomPlayer* CMJRoom::doCreateSitableRoomPlayer()
{
	return new CMJRoomPlayer();
}

void CMJRoom::onPlayerWillStandUp( ISitableRoomPlayer* pPlayer )
{
	if ( pPlayer->isHaveState(eRoomPeer_CanAct) )
	{
		 ISitableRoom::onPlayerWillStandUp(pPlayer) ;
	}
	else
	{
		playerDoStandUp(pPlayer);
	}
}

uint32_t CMJRoom::getLeastCoinNeedForCurrentGameRound(ISitableRoomPlayer* pp)
{
	CMJRoomPlayer* pPlayer = (CMJRoomPlayer*)pp ;
	return pPlayer->getCoin() ;
}

void CMJRoom::roomInfoVisitor(Json::Value& vOutJsValue)
{
	vOutJsValue["bankIdx"] = m_nBankerIdx;
	vOutJsValue["baseBet"] = getBaseBet();
}

void CMJRoom::sendRoomPlayersInfo(uint32_t nSessionID)
{
	CLogMgr::SharedLogMgr()->PrintLog("send room info to session id = %d, not used msg ", nSessionID ) ;
}


uint32_t CMJRoom::getBaseBet()
{
	return m_nBaseBet ;
}

void CMJRoom::onGameWillBegin()
{
	ISitableRoom::onGameWillBegin();
	m_nBankerIdx = 0 ;
	CLogMgr::SharedLogMgr()->PrintLog("room game begin");
}

void CMJRoom::onGameDidEnd()
{

	ISitableRoom::onGameDidEnd();
	CLogMgr::SharedLogMgr()->PrintLog("room game End");
}

void CMJRoom::prepareCards()
{

}

uint32_t CMJRoom::coinNeededToSitDown()
{
	return getBaseBet()* 100 ;
}

void CMJRoom::caculateGameResult()
{
	// caculate result ;
	
}
