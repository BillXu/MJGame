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

void CNewMJRoomPlayer::getCardInfo( Json::Value& vCardInFoValue )
{
	Json::Value vAnPai ;
	Json::Value vMingPai ;
	Json::Value vHuPai ;
	Json::Value vChuPai ;

	std::vector<uint8_t> vCards ;

	// hold card
	m_tPeerCard.getHoldCard(vCards);
	for (auto& ref : vCards )
	{
		vAnPai[vAnPai.size()] = ref ;
	}

	// showed card ;
	vCards.clear() ;
	m_tPeerCard.getShowedCard(vCards);
	for (auto& ref : vCards )
	{
		vMingPai[vMingPai.size()] = ref ;
	}

	// chu card ;
	vCards.clear() ;
	m_tPeerCard.getChuedCard(vCards);
	for (auto& ref : vCards )
	{
		vChuPai[vChuPai.size()] = ref ;
	}

	vCardInFoValue["idx"] = getIdx() ;
	vCardInFoValue["anPai"] = vAnPai ;
	vCardInFoValue["mingPai"] = vMingPai ;
	vCardInFoValue["huPai"] = vHuPai ;
	vCardInFoValue["chuPai"] = vChuPai ;
	vCardInFoValue["queType"] = eCT_None;
}