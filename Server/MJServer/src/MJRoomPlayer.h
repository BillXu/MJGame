#pragma once
#include "CommonDefine.h"
#include "ISitableRoomPlayer.h"
#include "MJPeerCard.h"
#include "CommonData.h"
struct stBill
{
	enum eBillType
	{
		eBill_None,
		eBill_Lose = 1 ,
		eBill_Win = 1 << 1 ,
		eBill_GangWin = (1 << 2 ) | eBill_Win ,
		eBill_HuWin = (1 << 3 ) | eBill_Win ,
		eBill_GangLose = (1 << 4 ) | eBill_Win ,
		eBill_HuLose = (1 << 5 ) | eBill_Win ,
		eBill_WinRollBackGang = (1 << 6 ) | eBill_Win ,
		eBill_LoseRollBackGang = (1 << 6 ) | eBill_Win ,
	};
	
	int32_t nOffset ;
	eBillType eType ;
	
	stBill(){ eType = eBill_None ; }

};

struct stBillLose
	:public stBill
{
	stBillLose(){ eType = eBill_GangLose; }
	uint8_t nWinnerIdx ;
};

struct stBillWin
	:public stBill
{
	stBillWin(){ eType = eBill_GangWin ;}
	std::map<uint8_t,uint32_t> vLoseIdxAndCoin ;
};

class CMJRoomPlayer
	:public ISitableRoomPlayer
{
public:
	typedef std::vector<uint8_t> VEC_UINT_8 ;
public:
	void reset(IRoom::stStandPlayer* pPlayer) override ;
	void onGameEnd()override ;
	void onGameBegin()override ;
	void doSitdown(uint8_t nIdx ) override;
	//void willStandUp() override;
	void onGetCard( uint8_t nIdx , uint8_t nCard );
	int32_t getGameOffset()override { return m_nGameOffset ; } ;
	IPeerCard* getPeerCard()override{ return nullptr ;};
	CMJPeerCard* getMJPeerCard(){ return &m_tPeerCard ;}

	void declareBuGang(uint8_t nCardNumber );
	void beRobotGang( uint8_t nCardNumber );

	bool removeCard(uint8_t nCardNumber) ;
	bool isHaveAnCard(uint8_t nCardNumber);
	uint8_t getCardByIdx(uint8_t nCardIdx, bool isForExchange = true );
	void addDistributeCard(uint8_t nCardNumber );
	void setMustQueType(uint8_t nType );
	bool canHuPai(uint8_t nCard = 0 ); // 0 means , self hu ; 
	bool canGangWithCard(uint8_t nCard,bool bCardFromSelf );
	bool canPengWithCard(uint8_t nCard);
	uint8_t getNewFetchCard();
	void fetchCard(uint8_t nCardNumber);
	void gangPai(uint8_t nGangPai, eMJActType eGangType,uint8_t nNewCard );
	eMJActType getNewFetchedFrom();
	uint8_t doHuPaiFanshu( uint8_t nCardNumber, uint8_t& nGenShu );  // nCardNumber = 0 , means self mo ; return value not include gen ;
	bool isCardBeWanted(uint8_t nCardNumber, bool bFromSelf );
	void onPengCard(uint8_t nCard );
	void onChuCard(uint8_t nCard );
	bool isHuaZhu();
	bool isTingPai();
	uint8_t getMaxCanHuFanShu( uint8_t& nGenShu );
	void addBill( stBill* pBill );
	void getGangWinBill( std::vector<stBill*>& vecGangWin );

	void onChuedPaiBePengOrGang(uint8_t nCardNum );
	void addChuPai(uint8_t nCardNum );
	void addHuPai(uint8_t nCardNum );
	void getCardInfo( Json::Value& vCardInFoValue );
	void debugWantedCard();
protected:
	void updateWantedCardList();
protected:
	int32_t m_nGameOffset ;
	CMJPeerCard m_tPeerCard ;
	std::vector<stBill*> m_vecBill ;

	VEC_UINT_8 m_vHuedCards ;
	VEC_UINT_8 m_vChuedCards ;

	uint8_t m_nNewFetchCard ;
	eMJActType m_eNewFetchCardFrom ;
	uint8_t m_nBuGaneCard ;
	LIST_WANTED_CARD m_listWantedCard ;
	bool m_isWantedCarListDirty ;
};