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
	void addMoCard(uint8_t nMoCard) final;
	void addGangCard(uint8_t nGangCard) final;
	bool chuCard(uint8_t nChuCard) final;
	bool eatCard(uint8_t nTarget, uint8_t nWithCardA, uint8_t nWithCardB) final;
	bool gangCardBeRobot(uint8_t nCard) final;
	bool onCardBeGangPeng(uint8_t nCard) final;

	bool isHaveCard(uint8_t nCard) final;
	bool canMingGangWithCard(uint8_t nCard) final;
	bool canPengWithCard(uint8_t nCard) final;
	bool canEatCard(uint8_t nCard, uint8_t& nWithA, uint8_t& withB) override;
	bool canHuWitCard(uint8_t nCard) override;
	bool isTingPai() override;
	bool getHoldCardThatCanGang(VEC_CARD& vGangCards)final;
	bool isHoldCardCanHu() override;

	bool getHoldCard(VEC_CARD& vHoldCard) final;
	bool getChuedCard(VEC_CARD& vChuedCard) final;
	bool getGangedCard(VEC_CARD& vGangCard) final;
	bool getPengedCard(VEC_CARD& vPengedCard) final;
	bool getEatedCard(VEC_CARD& vEatedCard) final;
	uint32_t getNewestFetchedCard()final;
};