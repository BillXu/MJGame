#include "WaitPlayerActState.h"
#include "LogManager.h"
#include "NewMJRoom.h"
void CWaitPlayerAct::enterState(IRoom* pRoom,Json::Value& jsTransferData)
{
	IRoomState::enterState(pRoom,jsTransferData) ;
	m_isWaitingChoseAct = jsTransferData["isWaitChoseAct"].asBool();
	m_nCurPlayerIdx = jsTransferData["idx"].asUInt();

	if ( this->m_isWaitingChoseAct )
	{
		bool isOnlyChu = !(jsTransferData["onlyChu"].isNull() || jsTransferData["onlyChu"].asUInt() == 0) ;
		startWaitChoseAct(m_nCurPlayerIdx,isOnlyChu) ;
		return ;
	}
	
	// just executing act 
	m_eExecutingAct = (eMJActType)jsTransferData["exeAct"].asUInt();
	m_nActCard = 0 ;
	if ( jsTransferData["actCard"].isNull() == false )
	{
		m_nActCard = jsTransferData["actCard"].asUInt();
	}
	
	auto ret = doExcutingAct(m_eExecutingAct,m_nActCard ) ;
	if ( ret == false )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("doing act = %u , error with card = %u",m_eExecutingAct,m_nActCard ) ;
	}
}

bool CWaitPlayerAct::doExcutingAct( eMJActType eAct, uint8_t nActCard )
{
	m_eExecutingAct = eAct ;
	m_nActCard = nActCard ;
	this->m_isWaitingChoseAct = false ;

	auto pNewRoom = (CNewMJRoom*)m_pRoom ;
	bool isExcuteOk = false ;
	float fDuringTime = 88888888888.0f ;
	switch ( m_eExecutingAct )
	{
	case eMJAct_Chu:
		isExcuteOk = pNewRoom->onPlayerChu(m_nCurPlayerIdx,nActCard) ;
		fDuringTime = eTime_DoPlayerActChuPai;
		break;
	case eMJAct_Mo:
		isExcuteOk = pNewRoom->onPlayerMoPai(m_nCurPlayerIdx) ;
		fDuringTime = eTime_DoPlayerMoPai;
		break;
	case eMJAct_BuGang_Done:
		isExcuteOk = pNewRoom->onPlayerBuGang(m_nCurPlayerIdx,m_nActCard) ;
		fDuringTime = eTime_DoPlayerAct_Gang ;
		break;
	case eMJAct_Hu:
		 isExcuteOk = pNewRoom->onPlayerHu(m_nCurPlayerIdx,m_nCurPlayerIdx,m_nActCard ) ;
		 fDuringTime = eTime_DoPlayerAct_Hu ;
		break;
	case eMJAct_BuGang:
	case eMJAct_BuGang_Declare:
		{
			m_eExecutingAct = eMJAct_BuGang_Declare ;
			isExcuteOk = pNewRoom->onPlayerDeclareBuGang(m_nCurPlayerIdx,m_nActCard) ; // must delcalre , send the declare msg to client ;
			if ( !isExcuteOk )
			{
				CLogMgr::SharedLogMgr()->PrintLog( "you can not bu gang idx = %u",m_nCurPlayerIdx) ;
				break;  // if you can not bu gang , you should do this ;
			}

			Json::Value jsArrayNeedIdx ;
			if ( !pNewRoom->isAnyOneNeedTheCard(m_nCurPlayerIdx,m_nActCard,eMJAct_BuGang_Declare,jsArrayNeedIdx) )
			{
				// if nobody need , just do gang ;
				m_eExecutingAct = eMJAct_BuGang_Done ;
				isExcuteOk = pNewRoom->onPlayerBuGang(m_nCurPlayerIdx,m_nActCard) ;
				fDuringTime = eTime_DoPlayerAct_Gang ;
				break;
			}
			isExcuteOk = pNewRoom->onPlayerDeclareBuGang(m_nCurPlayerIdx,m_nActCard) ;
			fDuringTime = eTime_DoPlayerActChuPai ;
		}
		break;
	case eMJAct_AnGang:
		isExcuteOk = pNewRoom->onPlayerAnGang(m_nCurPlayerIdx,m_nActCard) ;
		fDuringTime = eTime_DoPlayerAct_Gang ;
		break;
	default:
		CLogMgr::SharedLogMgr()->ErrorLog("unknown act type = %u ,wait player act enter state card = %u , you may need restart the server ",m_eExecutingAct,m_nActCard ) ;
		break;
	}
	setStateDuringTime(fDuringTime);
	return isExcuteOk ;
}

void CWaitPlayerAct::onStateDuringTimeUp()
{
	auto pNewRoom = (CNewMJRoom*)m_pRoom;
	if ( this->m_isWaitingChoseAct )  // wait time out 
	{
		// wait chu pai time out ;system auto chu ;
		m_nActCard = pNewRoom->getAutoChuCardWhenWaitTimeOut(m_nCurPlayerIdx) ;
		doExcutingAct(eMJAct_Chu,m_nActCard);
		return ;
	}

	// executing act finished 
	switch ( m_eExecutingAct )
	{
	case eMJAct_Chu:
		{
			// does any other need the card ?
			Json::Value jsArrayNeedIdx ;
			if ( pNewRoom->isAnyOneNeedTheCard(m_nCurPlayerIdx,m_nActCard,eMJAct_Chu,jsArrayNeedIdx) )
			{
				Json::Value jsTrans ;
				jsTrans["invokerIdx"] = m_nCurPlayerIdx;
				jsTrans["card"] = m_nActCard;
				jsTrans["cardFrom"] = eMJAct_Chu;
				jsTrans["arrNeedIdxs"] = jsArrayNeedIdx  ;
				pNewRoom->goToState(eRoomState_WaitOtherPlayerAct,&jsTrans);
				break;
			}
			else
			{
				// is game over 
				if ( pNewRoom->isGameOver() )
				{
					pNewRoom->goToState(eRoomState_GameEnd);
					break;
				}
				else // next player mo pai ;
				{
					m_nCurPlayerIdx = pNewRoom->getNextActIdx(m_nCurPlayerIdx) ;
					doExcutingAct(eMJAct_Mo,0);
				}
			}
		}
		break;
	case eMJAct_Hu:
		if ( pNewRoom->isGameOver() )
		{
			pNewRoom->goToState(eRoomState_GameEnd);
			break;
		}
		else
		{
			m_nCurPlayerIdx = pNewRoom->getNextActIdx(m_nCurPlayerIdx) ;
			doExcutingAct(eMJAct_Mo,0);
		}
		break;
	case eMJAct_BuGang_Declare:
		{
			Json::Value jsArrayNeedIdx ;
			if ( pNewRoom->isAnyOneNeedTheCard(m_nCurPlayerIdx,m_nActCard,eMJAct_BuGang_Declare,jsArrayNeedIdx) )
			{
				Json::Value jsTrans ;
				jsTrans["invokerIdx"] = m_nCurPlayerIdx;
				jsTrans["card"] = m_nActCard;
				jsTrans["cardFrom"] = eMJAct_BuGang_Declare;
				jsTrans["arrNeedIdxs"] = jsArrayNeedIdx  ;
				pNewRoom->goToState(eRoomState_WaitOtherPlayerAct,&jsTrans);
				break;
			}
			else
			{
				CLogMgr::SharedLogMgr()->ErrorLog("no body can hu bu gang card , why come to declare state ?");
				doExcutingAct(eMJAct_BuGang_Done,m_nActCard) ;
			}
		}
		break;
	case eMJAct_Mo:
	case eMJAct_BuGang_Done:
	case eMJAct_AnGang:
		startWaitChoseAct(m_nCurPlayerIdx) ;
		break;
	default:
		CLogMgr::SharedLogMgr()->ErrorLog("unknown act type = %u ,during time out ",m_eExecutingAct) ;
		break;
	}
}

bool CWaitPlayerAct::onMsg(Json::Value& prealMsg ,uint16_t nMsgType, eMsgPort eSenderPort , uint32_t nSessionID)
{
	if ( nMsgType != MSG_PLAYER_ACT )
	{
		return false ;
	}

	uint8_t nReqActType = prealMsg["actType"].asUInt();
	uint8_t nReqActCard = 0 ;
	if ( prealMsg["card"].isNumeric() )
	{
		nReqActCard = prealMsg["card"].asUInt();
	}

	uint8_t nRet = 0 ;
	do 
	{
		if ( this->m_isWaitingChoseAct == false )
		{
			CLogMgr::SharedLogMgr()->ErrorLog("cur state is not wait act, so you can not respone your act = %u, session id = %u",nReqActType,nReqActCard) ;
			nRet = 4 ;
			break;
		}

		// check the player is valid 
		auto pRoom = (CNewMJRoom*)m_pRoom ;
		if ( pRoom->getIdxBySessionID(nSessionID) != m_nCurPlayerIdx )
		{
			CLogMgr::SharedLogMgr()->ErrorLog("you are not in room or not the turn you act , curActIdx = %u,so can not respone your act = %u, session id = %u",m_nCurPlayerIdx,nReqActType,nReqActCard) ;
			nRet = 1 ;
			break;
		}

		if ( eMJAct_Pass == nReqActType )
		{
			CLogMgr::SharedLogMgr()->PrintLog("player chose pass ,so start waiting agian") ;
			startWaitChoseAct(m_nCurPlayerIdx,true) ;
			break;
		}

		auto fLeftDuring = getStateDuring();
		if ( !doExcutingAct((eMJActType)nReqActType,nReqActCard) )
		{
			CLogMgr::SharedLogMgr()->ErrorLog("this act error = %u , card = %u, go on waiting chose, but do not reset time " ,nReqActType,nReqActCard ) ;
			setStateDuringTime(fLeftDuring) ;
			this->m_isWaitingChoseAct = true ;
			nRet = 2 ;
			break;
		}
	} while (0);

	Json::Value jsmsback ;
	jsmsback["ret"] = nRet ;
	m_pRoom->sendMsgToPlayer(jsmsback,nMsgType,nSessionID) ;
	return true ;
}

void CWaitPlayerAct::startWaitChoseAct( uint8_t nPlayerIdx , bool isOnlyCanChu )
{
	m_nCurPlayerIdx = nPlayerIdx ;
	this->m_isWaitingChoseAct = true ;
	this->m_isOnlyCanChu = isOnlyCanChu ;
	setStateDuringTime(eTime_WaitPlayerChoseAct);
	if ( !isOnlyCanChu )
	{
		auto pRoom = (CNewMJRoom*)m_pRoom ;
		pRoom->sendPlayerActListOnRecievedCard(m_nCurPlayerIdx);
	}
}
