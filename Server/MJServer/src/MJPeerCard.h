#pragma once
#include "MJDefine.h"
#include "NativeTypes.h"
#include <list>
#include <map>
#include "json/json.h"
enum eSinglePeerCardState
{
	eSinglePeerCard_An,
	eSinglePeerCard_Peng,
	eSinglePeerCard_MingGang,
	eSinglePeerCard_AnGang,
	eSinglePeerCard_BuGang,
	eSinglePeerCard_Chi,
	eSinglePeerCard_Max,
};

struct stSinglePeerCard
{
	uint8_t nCardNumber ;
	eSinglePeerCardState eState ;
	stSinglePeerCard(){ eState = eSinglePeerCard_An; nCardNumber = 0 ;}
};

struct stWantedCard
{
	uint8_t nNumber ;
	ePosType eWanteddCardFrom;
	eMJActType eCanInvokeAct ;
	uint8_t nFanRate ; // used when act type == hu ;
	eFanxingType eFanxing ; //  used when act type == hu ;
	stWantedCard(){ nFanRate = 0 ; eFanxing = eFanxing_PingHu ; }
};

typedef std::list<stWantedCard> LIST_WANTED_CARD;


class CPeerCardSubCollect
{
public:
	typedef std::list<stSinglePeerCard> LIST_PEER_CARDS;
public:
	bool removeCardNumber( uint8_t nNumber );
	void doAction(eMJActType eType, uint8_t nNumber );
	void getWantedCardList(LIST_WANTED_CARD& vList,bool bOmitChi );
	void getSelfOperateCardList(LIST_WANTED_CARD& vList);
	void clear();
	uint8_t getCardCount();
	uint8_t getAnPaiCount() { return m_vAnCards.size() ; }
	uint8_t getGenCount();
public:
	LIST_PEER_CARDS m_vAnCards ;
	LIST_PEER_CARDS m_vMingCards ;
};

class CMJPeerCard
{
public:
	typedef std::map<eMJCardType,CPeerCardSubCollect> MAP_CT_COLLECT;
public:
	void setMustQueType(eMJCardType eQueType ){ m_eMustQueType = eQueType ;}
	bool removeCardNumber( uint8_t nNumber );
	void doAction(eMJActType eType, uint8_t nNumber );
	virtual void reset();
	eMJCardType getMustQueType(){ return m_eMustQueType ;}
	void updateWantedCard( LIST_WANTED_CARD& vWantList );
	void updateSelfOperateCard( LIST_WANTED_CARD& vOperateList, uint8_t nNewCard );
	bool isContainMustQue();
	void addCard( uint8_t nCardNuber );
	uint8_t doHuPaiFanshu( uint8_t nCardNumber , uint8_t& nGenShu ); // nCardNumber = 0 , means self mo ; return value not include gen ;
	uint8_t getMaxHuPaiFanShu( uint8_t& nGenShu );
	uint8_t getCardByIdx(uint8_t nCardIdx, bool isForExchange = true );
	bool isHaveAnCard(uint8_t nCardNumber);
	void getAnPai(Json::Value& vAnPia );
	void getMingPai( Json::Value& vMingPia );
	void debugAnpaiCount();
protected:
	uint8_t getGenShu();
protected:
	//friend class CBloodQingYiSe ;
	//friend class CBloodFanxingPingHu ;
	//friend class CBloodFanxingQiDui ;
	//friend class CBloodFanxingLongQiDui ;
	//friend class CBloodFanxingDuiDuiHu ;
public:
	MAP_CT_COLLECT m_vSubCollectionCards ;
	eMJCardType m_eMustQueType ;
};