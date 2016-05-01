#include "MJRoomPlayer.h"
#include <string>
#include "LogManager.h"
#include "ServerMessageDefine.h"
void CMJRoomPlayer::reset(IRoom::stStandPlayer* pPlayer)
{
	ISitableRoomPlayer::reset(pPlayer) ;
	m_nTryBankerTimes = 0 ;
	m_nBetTimes = 0 ;
	m_tPeerCard.reset();
	m_nGameOffset = 0 ;
}

void CMJRoomPlayer::onGameEnd()
{
	ISitableRoomPlayer::onGameEnd();
	m_nTryBankerTimes = 0 ;
	m_nBetTimes = 0 ;
	m_nGameOffset = 0 ;
	m_tPeerCard.reset();
}

void CMJRoomPlayer::onGameBegin()
{
	ISitableRoomPlayer::onGameBegin() ;
	m_nTryBankerTimes = 0 ;
	m_nBetTimes = 0 ;
	m_nGameOffset = 0 ;
	m_tPeerCard.reset();
	setState(eRoomPeer_CanAct) ;
}

void CMJRoomPlayer::doSitdown(uint8_t nIdx )
{
	setIdx(nIdx) ;
}

void CMJRoomPlayer::willStandUp()
{
	ISitableRoomPlayer::willStandUp() ;
}

