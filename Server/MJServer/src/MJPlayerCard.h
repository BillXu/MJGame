#pragma once 
#include "IMJPlayerCard.h"
#include "IMJPlayerCardCheckPaixingHelper.h"
class MJPlayerCard
	:public IMJPlayerCard
	,public IMJPlayerCardCheckPaixingHelper
{
public:
	void reset() override;
	void addDistributeCard(uint8_t nCardNum) final;
	bool onGangCardBeRobot(uint8_t nCard) final;
	bool onCardBeGangPengEat(uint8_t nCard) final;

	bool isHaveCard(uint8_t nCard) final;
	bool canMingGangWithCard(uint8_t nCard) final;
	bool canPengWithCard(uint8_t nCard) final;
	bool canEatCard(uint8_t nCard, uint8_t& nWithA, uint8_t& withB) override;
	bool canHuWitCard(uint8_t nCard) override;
	bool isTingPai() override;
	bool getHoldCardThatCanAnGang(VEC_CARD& vGangCards)final;
	bool getHoldCardThatCanBuGang(VEC_CARD& vGangCards)final;
	bool isHoldCardCanHu() override;

	void onMoCard(uint8_t nMoCard) final;
	virtual bool onPeng(uint8_t nCard) final;
	virtual bool onMingGang(uint8_t nCard, uint8_t nGangGetCard) final;
	virtual bool onAnGang(uint8_t nCard, uint8_t nGangGetCard) final;
	virtual bool onBuGang(uint8_t nCard, uint8_t nGangGetCard) final;
	virtual bool onEat(uint8_t nCard, uint8_t nWithA, uint8_t withB) final;
	virtual bool onChuCard(uint8_t nChuCard)final;

	bool getHoldCard(VEC_CARD& vHoldCard) final;
	bool getChuedCard(VEC_CARD& vChuedCard) final;
	bool getGangedCard(VEC_CARD& vGangCard) final;
	bool getPengedCard(VEC_CARD& vPengedCard) final;
	bool getEatedCard(VEC_CARD& vEatedCard) final;
	uint32_t getNewestFetchedCard()final;
};