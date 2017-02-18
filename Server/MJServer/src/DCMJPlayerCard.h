#pragma once 
#include "MJPlayerCard.h"
class DCMJPlayerCard
	:public MJPlayerCard
{
public:
	struct stHuCheckInfo
	{
		eFanxingType eHuType;
		uint8_t nContinueGangTime;
		bool isUsedBao;

		bool isCanHu()
		{
			return eHuType != eFanxing_Max;
		}

		void reset()
		{
			eHuType = eFanxing_Max;
			nContinueGangTime = 0;
			isUsedBao = false;
		}
	};
public:
	DCMJPlayerCard();
	void reset() override;
	void setEnableYao( bool isEanbleYao );
	void setIsHaveTianHuBao( bool isHaveTianHuBao );
	bool isTingPai() override;
	bool isHoldCardCanHu() override;
	bool onChuCard(uint8_t nChuCard)override;
	bool onMingGang(uint8_t nCard, uint8_t nGangGetCard) override;
	bool onAnGang(uint8_t nCard, uint8_t nGangGetCard) override;
	bool onBuGang(uint8_t nCard, uint8_t nGangGetCard) override;
	bool canEatCard(uint8_t nCard, uint8_t& nWithA, uint8_t& withB) override;
	bool canHuWitCard(uint8_t nCard) override;
	bool onDoHu( eFanxingType& nType, bool& isUsedBao ,uint8_t& nBeishu , uint8_t& nGangCnt );
protected:
	bool isCommonHoldCardCanHu();
	bool isDuiDuiHu( bool& isUsedBao, bool& isDanDiao);
	bool isNanHu( bool isHaveBao, bool& isUsedBao, bool& isQiZiQuan );
	bool isYaoHu( bool isHaveBao , bool& bIsUsedBao );
	bool isHaveGangBao();
	bool isHaveTianHuBao();
protected:
	stHuCheckInfo m_tCheckHuInfo;
	bool m_isEnableYao;
	bool m_isHaveTianHuBao;
};