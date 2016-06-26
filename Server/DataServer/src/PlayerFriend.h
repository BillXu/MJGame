#pragma once 
#include "IPlayerComponent.h"
#include "CommonDefine.h"
#include <set>
#include "MessageDefine.h"
struct stEventArg ;
class CPlayerFriend
	:public IPlayerComponent
{
public:
	typedef std::set<uint32_t> FRIENDS_UID ;
public:
	CPlayerFriend(CPlayer* pPlayer) ;
	~CPlayerFriend();
	virtual bool OnMessage( stMsg* pMessage , eMsgPort eSenderPort);
	bool OnMessage( Json::Value& recvValue , uint16_t nmsgType, eMsgPort eSenderPort)override ;
	virtual void OnPlayerDisconnect();
	virtual void Reset();
	virtual void Init();
	bool IsFriendListFull(){ return m_vAllFriends.size() > 30 ;}
	void OnPlayerWantAddMe(CPlayerFriend* pPlayerWantAddMe, std::string& strNotice );
	void OnOtherReplayMeAboutAddItbeFriend(bool bAgree,CPlayerFriend* pWhoReplyMe);
	virtual void TimerSave();
	void RemoveFriendByUID(uint32_t nPlayerUID );
	void AddFriend( uint32_t nFriendUserUID);
	static bool EventFunc(void* pUserData,stEventArg* pArg);
	bool isPlayerUIDFriend(uint32_t nPlayerUID);
protected:
	void SendListToClient(uint32_t nStartIdx , uint32_t nreqCnt );
	void OnProcessEvent(stEventArg* pArg);
	void readFriendList();
protected:
	FRIENDS_UID m_vAllFriends ;
	bool m_bDirty ;
};