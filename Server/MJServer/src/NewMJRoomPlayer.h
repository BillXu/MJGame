#pragma once
#include "ISitableRoomPlayer.h"
#include "MJPeerCardNew.h"
class CNewMJRoomPlayer
	:public ISitableRoomPlayer
{
public:
	void reset(IRoom::stStandPlayer* pPlayer)override ;
	void onGameEnd()override ;
	void onGameBegin()override ;

	//void willStandUp() override;
	int32_t getGameOffset()override { return m_nGameOffset ; } ;
	void addGameOffset( int32_t nOffset ){ m_nGameOffset += nOffset ;}
	IPeerCard* getPeerCard()override{ return nullptr ;};
	CMJPeerCardNew* getMJPeerCard(){ return &m_tPeerCard ;}
	void onRecievedCard(eMJActType eCardFrom , uint8_t nCard );
	uint8_t getNewRecievedCard();
	uint8_t getNewRecievedCardFrom();
protected:
	CMJPeerCardNew m_tPeerCard ;
	int32_t m_nGameOffset ;
	uint8_t m_nNewRecivedCard ;
	eMJActType m_eNewRecivedCardFrom ;
};