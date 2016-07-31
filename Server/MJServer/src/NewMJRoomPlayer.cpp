#include "NewMJRoomPlayer.h"
void CNewMJRoomPlayer::reset(IRoom::stStandPlayer* pPlayer)
{
	ISitableRoomPlayer::reset(pPlayer) ;
	m_tPeerCard.reset();
	m_nGameOffset = 0 ;
	m_eNewRecivedCardFrom = eMJAct_None ;
	m_nNewRecivedCard = 0 ;
}

void CNewMJRoomPlayer::onGameEnd()
{
	ISitableRoomPlayer::onGameEnd();
	m_nGameOffset = 0 ;
	m_tPeerCard.reset();
	m_eNewRecivedCardFrom = eMJAct_None ;
	m_nNewRecivedCard = 0 ;
}

void CNewMJRoomPlayer::onGameBegin()
{
	ISitableRoomPlayer::onGameBegin() ;
	m_nGameOffset = 0 ;
	m_tPeerCard.reset();
	m_eNewRecivedCardFrom = eMJAct_None ;
	m_nNewRecivedCard = 0 ;
}

void CNewMJRoomPlayer::onRecievedCard(eMJActType eCardFrom , uint8_t nCard )
{
	m_eNewRecivedCardFrom = eCardFrom ;
	m_nNewRecivedCard = nCard ;
}

uint8_t CNewMJRoomPlayer::getNewRecievedCard()
{
	return m_nNewRecivedCard ;
}

uint8_t CNewMJRoomPlayer::getNewRecievedCardFrom()
{
	return m_eNewRecivedCardFrom ;
}