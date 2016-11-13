#pragma once 
#include "MJPlayerCard.h"
#include "XLFanXingChecker.h"
class XLMJPlayerCard
	:public MJPlayerCard
	, public XLFanXingHelper
{
public:
	void reset()override;
	bool canEatCard(uint8_t nCard, uint8_t& nWithA, uint8_t& withB) override;
	bool onDoHu( bool isZiMo , uint8_t nCard , uint32_t& nHuType , uint8_t& nBeiShu, uint8_t& genCnt );
	bool isHuaZhu();
	uint32_t getMaxPossibleBeiShu();
	void setQueType(uint8_t nType );
	uint8_t getAutoQueType();
	uint8_t getQueType();
	bool canHuWitCard(uint8_t nCard) override;
	bool isHoldCardCanHu() override;
	bool isTingPai() override;
	bool canMingGangWithCard(uint8_t nCard) override;
	bool canPengWithCard(uint8_t nCard) override;
	bool canAnGangWithCard(uint8_t nCard)override;
	bool getHoldCardThatCanAnGang(VEC_CARD& vGangCards)override;
	XLFanXingChecker* getFanxingChecker(){ return &m_tFanXingChecker; };
	bool getHuedCard(VEC_CARD& vhuedCard);
protected:
	uint8_t getGenShu();
protected:
	uint8_t m_nQueType;
	VEC_CARD m_vecAlreadyHu;
public:
	static XLFanXingChecker m_tFanXingChecker;
};