#pragma once
#include "ISitableRoom.h"
#include "MJCard.h"
#include "TwoBirdGodFanxing.h"
struct stMJRoomConfig ;
class CNewMJRoomPlayer ;
class CNewMJRoom
	:public ISitableRoom
{
public:
	CNewMJRoom();
	bool onFirstBeCreated(IRoomManager* pRoomMgr,stBaseRoomConfig* pConfig, uint32_t nRoomID , Json::Value& vJsValue) override;
	void serializationFromDB(IRoomManager* pRoomMgr,stBaseRoomConfig* pConfig,uint32_t nRoomID , Json::Value& vJsValue )override;
	uint32_t getConfigID()override;
	void prepareState()override ;
	void roomInfoVisitor(Json::Value& vOutJsValue)override;
	void sendRoomInfo(uint32_t nSessionID )override;
	void sendRoomPlayersCardInfo(uint32_t nSessionID)override ;
	bool canStartGame()override ;
	uint32_t coinNeededToSitDown()override ;
	ISitableRoomPlayer* doCreateSitableRoomPlayer()override;
	bool canPlayerDirectLeave( uint32_t nUID )override;

	uint32_t getBaseBet(); // ji chu di zhu ;
	void doStartGame()override ;
	void onGameOver()override ;
	void onGameWillBegin()override ;
	void onGameDidEnd()override ;
	bool onMsg(Json::Value& prealMsg ,uint16_t nMsgType, eMsgPort eSenderPort , uint32_t nSessionID)override;
	uint8_t getRoomType()override{ return eRoom_MJ_Two_Bird_God ;}

	uint8_t getBankerIdx();
	bool isAnyOneNeedTheCard( uint8_t nCardOwner , uint8_t nCard, eMJActType eCardFrom ,Json::Value& jsArrayNeedIdx );
	uint8_t getAutoChuCardWhenWaitTimeOut(uint8_t nPlayerIdx );
	uint8_t getNextActIdx( uint8_t nCurActIdx );
	bool isGameOver();
	void sendActListToPlayerAboutCard(uint8_t nPlayerIdx , std::list<eMJActType>& vList , uint8_t nCard );
	void sendPlayerActListOnRecievedCard( uint8_t nCurPlayerIdx );

	// mj room function 
	bool onPlayerEat( uint8_t nActPlayerIdx  , uint8_t nInvokePlayerIdx ,uint8_t nTargetCard , uint8_t nWithCardA , uint8_t nWithCardB );
	bool onPlayerPeng( uint8_t nActPlayerIdx , uint8_t nInvokePlayrIdx , uint8_t nTargetCard );
	bool onPlayerHu( uint8_t nActPlayerIdx, uint8_t nInvokerPlayerIdx , uint8_t nTargetCard );
	bool onPlayerMingGang( uint8_t nActPlayerIdx , uint8_t nInvokerIdx , uint8_t nTargetCard );
	bool onPlayerDeclareBuGang(uint8_t nActPlayerIdx , uint8_t nTargetCard);
	bool onPlayerBuGang(uint8_t nActPlayerIdx , uint8_t nTargetCard );
	bool onPlayerAnGang(uint8_t nActPlayerIdx , uint8_t nTargetCard );
	bool onPlayerMoPai( uint8_t nActPlayerIdx );
	bool onPlayerChu(uint8_t nActPlayerIdx , uint8_t nCard );
	void onPlayerDeclareGangBeRobted( uint8_t nPlayerIdx , uint8_t nCard );

	// mj check function 
	bool canPlayerPeng(uint8_t nActPlayerIdx , uint8_t nCard );
	bool canPlayerMingGang(uint8_t nActlayerIdx ,uint8_t nCard );
	bool canPlayerHu(uint8_t nActPlayerIdx , uint8_t nCard );
	bool canPlayerEat(uint8_t nActPlayerIdx , uint8_t nCard );
	bool canPlayerEatWith(uint8_t nActPlayerIdx , uint8_t ACard, uint8_t nWithB );
protected:
	bool getHuFanxing(CNewMJRoomPlayer* pActor, CNewMJRoomPlayer* pInvoker, uint8_t nTargetCard, uint8_t& nFanxing, uint8_t& nFanshu );
protected:
	uint32_t getHuWinCoin(uint8_t nFanXing,uint16_t nFanshu ,bool isSelfHu );
	void prepareCards()override;
protected:
	CMJCard m_tPoker ;
	stMJRoomConfig* m_pRoomConfig ;
	uint8_t m_nBankerIdx ;
	uint8_t m_nCurIdx ;
private:
	static CTwoBirdFanxingChecker m_tTowBirdFanxingChecker ;
};