#pragma once 
#include "IPlayerComponent.h"
#include "CommonDefine.h"
#include <list>
#include "MessageDefine.h"
struct stEventArg ;
class CPlayerBag
	:public IPlayerComponent
{
public:
	struct stPlayerItem
	{
		uint32_t nItemID ;
		uint32_t nBuyTime ;
		uint32_t nCnt ;
		uint32_t nDeadTime ;
	};
	typedef std::list<stPlayerItem*> PLAYER_ITEMS ;
public:
	CPlayerBag(CPlayer* pPlayer) ;
	~CPlayerBag();
	bool OnMessage( Json::Value& recvValue , uint16_t nmsgType, eMsgPort eSenderPort)override;
	void Reset()override;
	void Init()override;
	void addPlayerItem(uint32_t nItemID , bool isStack , uint32_t nCnt ,bool isNewAdd );
	stPlayerItem* getPlayerItem(uint32_t nItemID );
	uint32_t getPlayerItemCnt(uint32_t nItemID );
	bool removePlayerItem(uint32_t nItemID , uint32_t nCnt );
	void clearItems();
protected:
	void SendListToClient();
	void readItems();
protected:
	PLAYER_ITEMS m_vAllItems ;
	bool m_isReading;
};