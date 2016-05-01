#pragma once
#include "CommonDefine.h"
#include "ISitableRoomPlayer.h"
#include "MJPeerCard.h"
#include "CommonData.h"
class CMJRoomPlayer
	:public ISitableRoomPlayer
{
public:
	void reset(IRoom::stStandPlayer* pPlayer) override ;
	void onGameEnd()override ;
	void onGameBegin()override ;
	void doSitdown(uint8_t nIdx ) override;
	void willStandUp() override;
	void onGetCard( uint8_t nIdx , uint8_t nCard );
	int32_t getGameOffset()override { return m_nGameOffset ; } ;
	IPeerCard* getPeerCard()override{ return nullptr ;};
	CMJPeerCard* getMJPeerCard(){ return &m_tPeerCard ;}

	bool removeCard(uint8_t nCardNumber) ;
	uint8_t getCardByIdx(uint8_t nCardIdx, bool isForExchange = true );
	void addCard(uint8_t nCardNumber );
	void setMustQueType(uint8_t nType );
	bool canHuPai(uint8_t nCard);
	bool canGangWithCard(uint8_t nCard);
	uint8_t getNewFetchCard();
	void fetchCard(uint8_t nCardNumber , eMJActType eCardFrom );
	eMJActType getNewFetchedFrom();
protected:
	uint8_t m_nTryBankerTimes ;
	uint8_t m_nBetTimes ;
	int32_t m_nGameOffset ;
	CMJPeerCard m_tPeerCard ;
};