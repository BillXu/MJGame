#include "IRoomState.h"
#include "IRoom.h"
#include <time.h>
#include "ServerCommon.h"
#include <cassert>
#include "ISitableRoom.h"
#include "ISitableRoomPlayer.h"
#include "json/json.h"
void IRoomState::update(float fDeta)
{ 
	if ( m_fStateDuring >= 0.0f )
	{ 
		m_fStateDuring -= fDeta ; 
		if ( m_fStateDuring <= 0.0f )
		{ 
			onStateDuringTimeUp();
		} 
	}
}

// wait state 

void IWaitingState::enterState(IRoom* pRoom)
{
	IRoomState::enterState(pRoom) ;
	assert(getStateDuring() > 0.00001 && "must set wait time before enter state" );
	assert( m_vWaitIdxs.empty() == false && "must set execute wait list before enter state" ) ;
}

void IWaitingState::addWaitingTarget( uint8_t nIdx , uint8_t nPrio )
{
	assert(isIdxInWaitList(nIdx) == false && "can not add twice" );
	stWaitIdx st ;
	st.nIdx = nIdx ;
	st.nMaxActExePrio = nPrio ;
	m_vWaitIdxs.push_back(st) ;
}

bool IWaitingState::isIdxInWaitList( uint8_t nIdx )
{
	auto iter = m_vWaitIdxs.begin() ;
	for ( ; iter != m_vWaitIdxs.end(); ++iter )
	{
		if ( (*iter).nIdx == nIdx )
		{
			return true ;
		}
	}

	return false ;
}

bool IWaitingState::responeWaitAct(uint8_t nIdx ,stActionItem* pAct)
{
	auto iter = m_vWaitIdxs.begin() ;
	for ( ; iter != m_vWaitIdxs.end(); ++iter )
	{
		if ( (*iter).nIdx == nIdx )
		{
			if ( (*iter).nMaxActExePrio > pAct->nExePrio )
			{
				printf("you can not do so high act \n");
				return false;
			}

			m_vWaitIdxs.erase(iter) ;

			if ( pAct )
			{
				// remove lower priority act 
				auto iterAct = m_vActList.begin() ;
				while (iterAct != m_vActList.end() )
				{
					if ( (*iterAct)->nExePrio < pAct->nExePrio )
					{
						delete (*iterAct) ;
						m_vActList.erase(iterAct) ;
						iterAct = m_vActList.begin() ;
					}
				}

				m_vActList.push_back(pAct) ; 

				// remove lower wait idx ;
				auto iterWidx = m_vWaitIdxs.begin() ;
				while ( iterWidx != m_vWaitIdxs.end() )
				{
					if ( (*iterWidx).nMaxActExePrio < pAct->nExePrio )
					{
						m_vWaitIdxs.erase(iterWidx);
						iterWidx = m_vWaitIdxs.begin() ;
					}
				}
			}

			if ( m_vWaitIdxs.empty() )
			{
				onWaitEnd(false) ;
			}
			return true ;
		}
	}

	printf("act player idx = %d , not in wait list \n",pAct->nActIdx ) ;
	return false ;
}

void IWaitingState::onStateDuringTimeUp()
{
	onWaitEnd(true) ;
}

void IWaitingState::setWaitTime( float fSeconds )
{
	setStateDuringTime(fSeconds) ;
}

// execute state 
void IExecuingState::setExecuteActs(VEC_ACTITEM& vActList )
{
	m_vActList.assign(vActList.begin(),vActList.end()) ;
	assert(m_vActList.empty() == false && "why act list is null ?" );
}

void IExecuingState::setExecuteTime(float fseconds )
{
	setStateDuringTime(fseconds) ;
}

void IExecuingState::enterState(IRoom* pRoom)
{
	IRoomState::enterState(pRoom) ;
	assert(getStateDuring() < 0.00001 && "must set exeute time before enter state" );
	assert(m_vActList.empty() == false && "must set execute act list before enter state" ) ;
	
	for ( auto act : m_vActList )
	{
		doExecuteAct(act) ;
		delete act ;
		act = nullptr ;
	}

	m_vActList.clear() ;
}

// wait  player ready 
void IRoomStateWaitPlayerReady::update(float)
{
	auto pSitRoom = (ISitableRoom*)(m_pRoom);
	if ( pSitRoom->getPlayerCntWithState(eRoomPeer_Ready) == m_pRoom->getSeatCount() )
	{
		// go to start game 
		pSitRoom->goToState(eRoomState_StartGame) ;
	}
}

bool IRoomStateWaitPlayerReady::onMsg(Json::Value& prealMsg ,uint16_t nMsgType, eMsgPort eSenderPort , uint32_t nSessionID)
{
	if ( nMsgType == MSG_PLAYER_SET_READY )
	{
		auto pSitRoom = (ISitableRoom*)(m_pRoom); 
		auto pp = pSitRoom->getSitdownPlayerBySessionID(nSessionID) ;
		pp->setState(eRoomPeer_Ready) ;

		Json::Value jsMsg ;
		jsMsg["idx"] = pp->getIdx() ;
		pSitRoom->sendRoomMsg(jsMsg,MSG_ROOM_PLAYER_READY) ;
		return true ;
	}
	return false ;
}