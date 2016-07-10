#pragma once
#include "IPlayerComponent.h"
#include <set>
#include <list>
class CPlayerGameData
	:public IPlayerComponent
{
public:
	enum ePlayerGameState 
	{
		ePlayerGameState_Entering,
		ePlayerGameState_StayIn,
		ePlayerGameState_NotIn,
		ePlayerGameState_Max,
	};

	struct stGameData
		: public stMJPlayerGameData
	{
		bool bDirty ;
	};
public:
	typedef std::set<uint32_t> SET_ROOM_ID ;
	typedef std::map<uint32_t,stMyOwnRoom> MAP_ID_MYROOW ;
public:
	CPlayerGameData(CPlayer* pPlayer):IPlayerComponent(pPlayer){ m_eType = ePlayerComponent_PlayerGameData ; }
	void Reset()override;
	void Init()override;
	bool OnMessage( stMsg* pMessage , eMsgPort eSenderPort)override ;
	bool OnMessage( Json::Value& recvValue , uint16_t nmsgType, eMsgPort eSenderPort) override;
	bool onCrossServerRequest(stMsgCrossServerRequest* pRequest , eMsgPort eSenderPort,Json::Value* vJsValue = nullptr)override;
	bool onCrossServerRequestRet(stMsgCrossServerRequestRet* pResult,Json::Value* vJsValue = nullptr )override;
	void OnPlayerDisconnect()override;
	void OnOtherWillLogined()override;
	void TimerSave();
	void OnReactive(uint32_t nSessionID )override{ sendGameDataToClient(); }
	void OnOtherDoLogined() override{sendGameDataToClient();}
	uint32_t getCurRoomID(){ return m_nStateInRoomID ;}
	ePlayerGameState getCurGameState(){ return m_ePlayerGameState ; }
	void addOwnRoom(eRoomType eType , uint32_t nRoomID , uint16_t nConfigID );
	bool isCreateRoomCntReachLimit(eRoomType eType);
	bool deleteOwnRoom(eRoomType eType , uint32_t nRoomID );
	/*uint16_t getMyOwnRoomConfig(eRoomType eType ,  uint32_t nRoomID ) ;*/
	bool isRoomIDMyOwn(eRoomType eType , uint32_t nRoomID);
	bool isNotInAnyRoom(){ return m_ePlayerGameState == ePlayerGameState_NotIn ; }
	void addNewBillIDs(uint32_t nBillID );
protected:
	void sendGameDataToClient();
protected:
	uint32_t m_nStateInRoomID ;
	ePlayerGameState m_ePlayerGameState ;
	bool m_bIsCreating ;

	std::list<uint32_t> m_vVipBillIDs ;
	stGameData m_vData[eRoom_MJ_MAX] ;
	MAP_ID_MYROOW m_vMyOwnRooms[eRoom_MJ_MAX];
};