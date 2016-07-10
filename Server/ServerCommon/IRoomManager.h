#pragma once
#include "ServerMessageDefine.h"
#include <map>
#include <json/json.h>
#include "httpRequest.h"
#include "IGlobalModule.h"
#include <list>
#include <memory>
#include <set>
class CRoomConfigMgr ;
class IRoomInterface ;
class IRoom ;
class IRoomManager
	:public CHttpRequestDelegate
	,public IGlobalModule
{
public:
	typedef std::list<IRoomInterface*> LIST_ROOM ;
	typedef std::map<uint32_t, IRoomInterface*> MAP_ID_ROOM;
	typedef std::vector<uint32_t>  VEC_INT ;
	struct stRoomCreatorInfo
	{
		uint32_t nPlayerUID ;
		VEC_INT vRoomIDs ;
	};

	struct stSystemRoomInfo
	{
		uint32_t nConfigID ;
		VEC_INT vRoomIDs ; 
	};

	struct stVipRoomBill
	{
		uint32_t nBillID ;
		uint32_t nRoomID ;
		uint32_t nCreateUID ;
		uint32_t nRoomType ;
		uint32_t nRoomInitCoin ;
		uint16_t nCircleCnt ; 
		uint32_t nBillTime ;
		Json::Value jsDetail ;
	};

	struct stReqVipRoomBillPlayers 
	{
		uint32_t nReqBillID ;
		std::set<uint32_t> vReqPlayers ;
	};

	typedef std::shared_ptr<stVipRoomBill> VIP_ROOM_BILL_SHARED_PTR ;

	typedef std::map<uint32_t,stRoomCreatorInfo> MAP_UID_CR;
	typedef std::map<uint32_t, stSystemRoomInfo>  MAP_CONFIG_ID_SYS_ROOM ;
	typedef std::map<uint32_t , std::shared_ptr<stVipRoomBill>> MAP_VIP_BILL ;
public:
	IRoomManager(CRoomConfigMgr* pConfigMgr);
	~IRoomManager();
	void init( IServerApp* svrApp )override; 
	uint16_t getModuleType(){ return IGlobalModule::eMod_RoomMgr ;}
	bool onMsg(stMsg* prealMsg , eMsgPort eSenderPort , uint32_t nSessionID)override;
	bool onMsg(Json::Value& prealMsg ,uint16_t nMsgType, eMsgPort eSenderPort , uint32_t nSessionID)override;
	virtual bool onPublicMsg(stMsg* prealMsg , eMsgPort eSenderPort , uint32_t nSessionID);
	IRoomInterface* GetRoomByID(uint32_t nRoomID );
	void sendMsg(stMsg* pmsg, uint32_t nLen , uint32_t nSessionID ) ;
	void sendMsg( Json::Value& jsContent , unsigned short nMsgType , uint32_t nSessionID, eMsgPort ePort = ID_MSG_PORT_CLIENT ) ;
	void onHttpCallBack(char* pResultData, size_t nDatalen , void* pUserData , size_t nUserTypeArg);
	void update(float fDeta )override;
	void onTimeSave()override;
	void onConnectedSvr()override;
	bool reqeustChatRoomID(IRoom* pRoom);
	void deleteRoomChatID(uint32_t nChatID );
protected:
	virtual bool onCrossServerRequest(stMsgCrossServerRequest* pRequest , eMsgPort eSenderPort,Json::Value* vJsValue = nullptr);
	virtual bool onCrossServerRequestRet(stMsgCrossServerRequestRet* pResult,Json::Value* vJsValue = nullptr );
	virtual IRoomInterface* doCreateRoomObject( eRoomType cRoomType,bool isPrivateRoom ) = 0 ;
	virtual IRoomInterface* doCreateInitedRoomObject(uint32_t nRoomID , bool isPrivateRoom, uint16_t nRoomConfigID ,eRoomType reqSubRoomType, Json::Value& vJsValue) = 0 ;
	void addRoomToPrivate(uint32_t nOwnerUID ,IRoomInterface* pRoom);
	void removePrivateRoom( IRoomInterface* pRoom );

	void addRoomToSystem(IRoomInterface* pRoom);
	bool getPrivateRooms(uint32_t nCreatorUID,VEC_INT& vRoomIDsInfo );
	bool getSystemRooms(uint32_t nCreatorUID,VEC_INT& vRoomIDsInfo );
	virtual eRoomType getMgrRoomType() = 0 ;
	void sendVipRoomBillToPlayer( uint32_t nBillID , uint32_t nTargetSessionD );
public:
	void addVipRoomBill(std::shared_ptr<stVipRoomBill>& pBill, bool isAddtoDB );
	bool isHaveVipRoomBill(uint32_t nVipBillID );
	std::shared_ptr<stVipRoomBill> createVipRoomBill();
protected:
	MAP_ID_ROOM m_vRooms ;

	CHttpRequest m_pGoTyeAPI;
	uint32_t m_nMaxRoomID ;

	MAP_UID_CR m_mapPrivateRooms ;

	MAP_CONFIG_ID_SYS_ROOM m_vSystemRooms ;

	CRoomConfigMgr* m_pConfigMgr ;
	std::map<uint32_t,std::shared_ptr<stReqVipRoomBillPlayers>> m_vReqingBillInfoPlayers ;
protected:
	MAP_VIP_BILL m_vVipRoomBills ;
public:
	static uint32_t s_MaxBillID ;
};