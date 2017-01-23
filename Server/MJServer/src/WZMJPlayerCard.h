#pragma once 
#include "MJPlayerCard.h"
class WZMJPlayerCard
	:public MJPlayerCard
{
public:
	bool canEatCard(uint8_t nCard, uint8_t& nWithA, uint8_t& withB) override;
	bool onEat(uint8_t nCard, uint8_t nWithA, uint8_t withB)override;
	bool canHuWitCard(uint8_t nCard) override;
	uint8_t getMustChuFisrtCards(VEC_CARD& vCardMustChu );
	void setCaiShenCard( uint8_t nCaiShenCard );
	bool isHoldCardCanHu() override;
	bool onDoHu(bool isZiMo, uint8_t nCard, uint8_t& nHuType, uint8_t& nBeiShu);
	uint8_t getCaiShenCnt();
protected:
	bool isHardHuWithCard(uint8_t ncard = 0);  // zero means self mo
	uint8_t getBlankCnt();
	bool getFanxing(uint8_t& eFanxing, uint8_t& beiShu);
	uint8_t getMiniQueCnt(VEC_CARD vCards[eCT_Max])override;
	uint8_t get7PairQueCnt(VEC_CARD vCards[eCT_Max])override;
protected:
	uint8_t m_nCaiShenCard;
};