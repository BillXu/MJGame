#pragma once 
#include "IMJPlayerCard.h"
#include "IMJPlayerCardCheckPaixingHelper.h"
#include "MJDefine.h"
#include <algorithm>  
#include <set>
class MJPlayerCard
	:public IMJPlayerCard
	,public IMJPlayerCardCheckPaixingHelper
{
public:
	struct stNotShunCard
	{
		VEC_CARD vCards;
	public:
		stNotShunCard();
		bool operator != (const stNotShunCard& v);
		stNotShunCard& operator = (const stNotShunCard& v);
		bool operator == (const stNotShunCard& v);
		uint8_t getLackCardCntForShun();
		uint8_t getSize()const{ return vCards.size(); }
		bool operator < (const stNotShunCard& v)const;
	};
	typedef std::set<stNotShunCard> SET_NOT_SHUN;
public:
	void reset() override;
	void addDistributeCard(uint8_t nCardNum) final;
	bool onGangCardBeRobot(uint8_t nCard) final;
	bool onCardBeGangPengEat(uint8_t nCard) final;

	bool isHaveCard(uint8_t nCard) final;  // holdCard ;
	bool canMingGangWithCard(uint8_t nCard) final;
	bool canPengWithCard(uint8_t nCard) final;
	bool canEatCard(uint8_t nCard, uint8_t& nWithA, uint8_t& withB) override;
	bool canHuWitCard(uint8_t nCard) override;
	bool isTingPai() override;
	bool getHoldCardThatCanAnGang(VEC_CARD& vGangCards)final;
	bool getHoldCardThatCanBuGang(VEC_CARD& vGangCards)final;
	bool isHoldCardCanHu() override;

	void onMoCard(uint8_t nMoCard) final;
	bool onPeng(uint8_t nCard) final;
	bool onMingGang(uint8_t nCard, uint8_t nGangGetCard) final;
	bool onAnGang(uint8_t nCard, uint8_t nGangGetCard) final;
	bool onBuGang(uint8_t nCard, uint8_t nGangGetCard) final;
	bool onEat(uint8_t nCard, uint8_t nWithA, uint8_t withB) final;
	bool onChuCard(uint8_t nChuCard)final;

	bool getHoldCard(VEC_CARD& vHoldCard) final;
	bool getChuedCard(VEC_CARD& vChuedCard) final;
	bool getGangedCard(VEC_CARD& vGangCard) final;
	bool getPengedCard(VEC_CARD& vPengedCard) final;
	bool getEatedCard(VEC_CARD& vEatedCard) final;
	uint32_t getNewestFetchedCard()final;
protected:
	void addCardToVecAsc(VEC_CARD& vec, uint8_t nCard );
	bool getNotShuns(VEC_CARD vCard, SET_NOT_SHUN& vNotShun, bool bMustKeZiShun );
	bool pickKeZiOut(VEC_CARD vCard, VEC_CARD& vKeZi , VEC_CARD& vLeftCard );
	bool pickNotShunZiOutIgnoreKeZi(VEC_CARD vCardIgnorKeZi, SET_NOT_SHUN& vNotShun);
	bool is7PairTing();
	bool canHoldCard7PairHu();
protected:
	VEC_CARD m_vCards[eCT_Max];
	VEC_CARD m_vChuedCard;
	VEC_CARD m_vPenged;
	VEC_CARD m_vGanged;
	VEC_CARD m_vEated;
	uint8_t m_nNesetFetchedCard;
};