#pragma once
#include "IRoom.h"
#include <vector>
class ISitableRoomPlayer ;
class CRobotDispatchStrategy ;
class ISitableRoom
	:public IRoom
{
public:
	typedef std::list<ISitableRoomPlayer*> LIST_SITDOWN_PLAYERS ;
	typedef std::vector<ISitableRoomPlayer*> VEC_SITDOWN_PLAYERS;
public:
	~ISitableRoom();
	bool onFirstBeCreated(IRoomManager* pRoomMgr,stBaseRoomConfig* pConfig, uint32_t nRoomID , Json::Value& vJsValue) override;
	void serializationFromDB(IRoomManager* pRoomMgr,stBaseRoomConfig* pConfig,uint32_t nRoomID , Json::Value& vJsValue )override;
	void willSerializtionToDB(Json::Value& vOutJsValue)override;
	void roomItemDetailVisitor(Json::Value& vOutJsValue)override;
	bool canStartGame()override ;
	void update(float fDelta)override;
	uint8_t canPlayerEnterRoom( stEnterRoomData* pEnterRoomPlayer ) override;
	// event function 
	void playerDoStandUp( ISitableRoomPlayer* pPlayer );

	uint16_t getEmptySeatCount();
	ISitableRoomPlayer* getPlayerByIdx(uint16_t nIdx );
	bool isSeatIdxEmpty( uint8_t nIdx );
	uint16_t getSitDownPlayerCount();
	uint16_t getSeatCount();
	ISitableRoomPlayer* getReuseSitableRoomPlayerObject();
	virtual ISitableRoomPlayer* doCreateSitableRoomPlayer() = 0 ;
	uint16_t getPlayerCntWithState( uint32_t nState );
	ISitableRoomPlayer* getSitdownPlayerBySessionID(uint32_t nSessionID);
	ISitableRoomPlayer* getSitdownPlayerByUID(uint32_t nUserUID );
	bool onMessage( stMsg* prealMsg , eMsgPort eSenderPort , uint32_t nPlayerSessionID )override;
	virtual  uint32_t coinNeededToSitDown() = 0;
	void onGameDidEnd()override ;
	void onGameWillBegin()override ;
	CRobotDispatchStrategy* getRobotDispatchStrage(){ return m_pRobotDispatchStrage ;}
	bool checkHavePlayerLoseOver(std::vector<uint8_t>& vLoseOverPlayerIdxs);
	uint8_t getIdxBySessionID(uint32_t nSessionID );
protected:
	uint8_t GetFirstInvalidIdxWithState( uint8_t nIdxFromInclude , eRoomPeerState estate );
private:
	time_t m_tTimeCheckRank ;
	uint16_t m_nSeatCnt ;
	ISitableRoomPlayer** m_vSitdownPlayers ;
	CRobotDispatchStrategy* m_pRobotDispatchStrage ;
private:
	LIST_SITDOWN_PLAYERS m_vReserveSitDownObject ;
};