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

void CMJRoomPlayer::onGetCard( uint8_t nIdx , uint8_t nCard )
{
	if ( nIdx >= NIUNIU_HOLD_CARD_COUNT )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("uid = %d ,on get card idx invalid idx = %d",getUserUID(),nIdx);
		return ;
	}
	getPeerCard()->addCompositCardNum(nCard);
}

uint8_t CMJRoomPlayer::getCardByIdx(uint8_t nIdx )
{
	if ( nIdx >= NIUNIU_HOLD_CARD_COUNT )
	{
		CLogMgr::SharedLogMgr()->ErrorLog("uid = %d , get card idx invalid idx = %d",getUserUID(),nIdx);
		return 0 ;
	}
	return m_tPeerCard.getCardByIdx(nIdx) ;
}

uint8_t CMJRoomPlayer::getTryBankerTimes()
{
	return m_nTryBankerTimes ;
}

void CMJRoomPlayer::setTryBankerTimes(uint8_t nTimes )
{
	m_nTryBankerTimes = nTimes ;
}

uint8_t CMJRoomPlayer::getBetTimes()
{
	return m_nBetTimes ;
}

void CMJRoomPlayer::setBetTimes(uint8_t nTimes)
{
	m_nBetTimes = nTimes ;
}