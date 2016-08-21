#pragma once
#include "MJDefine.h"
#include "NativeTypes.h"
#include <list>
#include <map>
#include "json/json.h"
#include <vector>
#include "MJHuPaiInfo.h"
class CMJPeerCardNew
{
public:
	struct stEatPair
	{
		uint8_t nCard[2] ;
	};
	typedef std::vector<stEatPair> VEC_EAT_PAIR ;
	typedef std::shared_ptr<CMJHuPaiInfo> hupaiInfo_ptr ;
public:
	CMJPeerCardNew():ptrHupaiInfo(new CMJHuPaiInfo()){ reset(); m_isHuPaiInfoDirty = true ;}
	void reset();
	bool init();
	void addHoldCard(uint8_t nCard );
	
	// check function 
	bool isCardCanPeng( uint8_t nCard );
	bool isCardCanMingGang( uint8_t nCard );
	bool isCardCanBuGang( uint8_t nCard );
	bool isCardCanAnGang(uint8_t nCard );
	bool isHaveAnGangCards( std::vector<uint8_t>& vAnGangCards );
	bool isHaveBuGang( std::vector<uint8_t>& vAnGangCards );
	bool isCardCanHu( uint8_t nCard );  // ncard = 0, means self ;
	bool isCardCanEat( uint8_t nCard, VEC_EAT_PAIR& vEatPairs );

	// act function 
	void onMoCard( uint8_t nCard );
	bool onChuCard(uint8_t nCard );
	bool onPeng( uint8_t nCard );
	bool onMingGang( uint8_t nCard , uint8_t nGangNewCard );
	bool onBuGang( uint8_t nCard , uint8_t nGangNewCard );
	bool onAnGang( uint8_t nCard , uint8_t nGangNewCard );
	bool onEat(uint8_t nCard , stEatPair& refWithPair );
	bool onCardBeRobted( uint8_t nCard );
	bool onHu(uint8_t nCard,bool isSelf );

	//  get function 
	void debugPeerCardInfo();
	uint8_t getFirstHoldCard(){ return vecHoldCard.front() ; }
	bool isHoldCardExist(uint8_t nCard );
	hupaiInfo_ptr getHuPaiInfoPtr(){ return ptrHupaiInfo ;}

	// get func 
	void getHoldCard(std::vector<uint8_t>& vCards );
	void getShowedCard(std::vector<uint8_t>& vCards );  // not sort ; for client use ; 
	void getChuedCard(std::vector<uint8_t>& vCards );
	void getPengedCard(std::vector<uint8_t>& vCards );
	void getGangCard( std::vector<uint8_t>& vCards );
	void getMingGang( std::vector<uint8_t>& vCards );
	void getAnGang(std::vector<uint8_t>& vCards);
	void getEatCard(std::vector<uint8_t>& vCards );
	bool isBeRobotEmpty(){ return vecBeRobot.empty() ;}
protected:
	void debugVecCardInfo(std::vector<uint8_t>&vecCards );
	bool addNumberToVecWithAsc(std::vector<uint8_t>& vecCards, uint8_t& nAddCard );
	bool removeNumberFromVec(std::vector<uint8_t>& vecCards , uint8_t& nCard );
protected:
	friend CMJHuPaiInfo ;
protected:
	bool m_isHuPaiInfoDirty ;
	hupaiInfo_ptr ptrHupaiInfo ;
	std::vector<uint8_t> vecHoldCard ;
	std::vector<uint8_t> vecPengedCard ;
	std::vector<uint8_t> vecMingGangCard ;
	std::vector<uint8_t> vecAnGangCard ;
	std::vector<uint8_t> vecEatedCard ;
	std::vector<uint8_t> vecChuCard ;
	std::vector<uint8_t> vecBeRobot ;
};