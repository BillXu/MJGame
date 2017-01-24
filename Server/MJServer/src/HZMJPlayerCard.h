#pragma once 
#include "MJPlayerCard.h"
class HZMJPlayerCard
	:public MJPlayerCard
{
public:
	bool canEatCard(uint8_t nCard, uint8_t& nWithA, uint8_t& withB) override;
	//bool is7PairTing() override;
	//bool canHoldCard7PairHu() override;
	//bool isTingPai() override;
	//bool isHoldCardCanHu() override;
	bool canHuWitCard(uint8_t nCard) override;
	uint8_t get7PairHuHaoHuaCnt();
	bool isBaoTou();
	void setIdxInfo( uint8_t nSelfIdx , uint8_t nBankeIdx );
	bool canAnGangWithCard(uint8_t nCard)override;
	bool getHoldCardThatCanAnGang(VEC_CARD& vGangCards)override;
protected:
	uint8_t getMiniQueCnt(VEC_CARD vCards[eCT_Max])override;
	uint8_t getCaiShenCnt();
	uint8_t get7PairQueCnt(VEC_CARD vCards[eCT_Max])override;
	bool is7PairBaoTou();
	bool isCommonBaoTou();
	//uint8_t getLestQue(SET_NOT_SHUN& vNotShun, bool bFindJiang, bool bFindDanDiao, uint8_t& nFiandJiang, uint8_t& nFindDanDiao );
protected:
	uint8_t nSelfIdx;
	uint8_t nBankeIdx;
};