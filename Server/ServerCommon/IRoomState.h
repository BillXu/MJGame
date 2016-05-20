#pragma once
#include "NativeTypes.h"
#include "MessageIdentifer.h"
#include "CommonDefine.h"
#include <ctime>
#include "IRoom.h"
#include "IRoomDelegate.h"
#include <cassert>
class IRoom ;
class ISitableRoom ;
struct stMsg ;
class IRoomState
{
public:
	IRoomState(){ m_fStateDuring = 0 ; }
	virtual ~IRoomState(){}
	virtual void enterState(IRoom* pRoom){ m_pRoom = (ISitableRoom*)pRoom ;} 
	virtual void leaveState(){}
	virtual void update(float fDeta);
	virtual bool onMsg(Json::Value& prealMsg ,uint16_t nMsgType, eMsgPort eSenderPort , uint32_t nSessionID){ return false ;}
	virtual bool onMessage( stMsg* prealMsg , eMsgPort eSenderPort , uint32_t nPlayerSessionID ){ return false ;}
	virtual uint16_t getStateID() = 0  ;
	virtual void onStateDuringTimeUp(){}
	void setStateDuringTime( float fTime ){ m_fStateDuring = fTime ;} 
	float getStateDuring(){ return m_fStateDuring ;}
private:
	float m_fStateDuring ;
protected:
	uint16_t m_nState ;
	ISitableRoom* m_pRoom ;
};

struct stActionItem
{
	uint8_t nActIdx;
	uint32_t nActType ;
	uint8_t nExePrio ;
	stActionItem(){ nActType = 0 ; nExePrio = 0 ;}
};

struct stWaitIdx
{
	uint8_t nIdx ;
	uint8_t nMaxActExePrio ;
	stWaitIdx(){ nMaxActExePrio = 0 ;}
};

typedef std::vector<stActionItem*> VEC_ACTITEM ;

// wait state 
class IWaitingState
	:public IRoomState
{
public:
	typedef std::vector<stWaitIdx> VEC_WAIT_IDX ;
public:
	void enterState(IRoom* pRoom)override;
	void addWaitingTarget( uint8_t nIdx , uint8_t nPrio = 0 );
	bool responeWaitAct(uint8_t nIdx ,stActionItem* pAct);
	virtual void onWaitEnd( bool bTimeOut ) = 0 ;
	void onStateDuringTimeUp()override final ;
	void setWaitTime( float fSeconds );
	bool isIdxInWaitList( uint8_t nIdx );
	void leaveState()override{ m_vWaitIdxs.clear();}
protected:
	VEC_ACTITEM m_vActList ;  // free object in execute state 
	VEC_WAIT_IDX m_vWaitIdxs ;
};

// running state 
class IExecuingState
	:public IRoomState
{
public:
	void setExecuteActs(VEC_ACTITEM& vActList );
	void setExecuteTime(float fseconds );
	void enterState(IRoom* pRoom)override;
	virtual void onExecuteOver() = 0;
	virtual void doExecuteAct( stActionItem* pAct) = 0 ;
	void onStateDuringTimeUp()override final 
	{
		onExecuteOver();
	}
	//void leaveState(){ assert( m_vActList.empty() && "not delete this object ? " ); }
protected:
	VEC_ACTITEM m_vActList ;
};

// wait player join state 
class IRoomStateWaitPlayerReady
	: public IRoomState
{
public:
	void update(float)override ;
	bool onMsg(Json::Value& prealMsg ,uint16_t nMsgType, eMsgPort eSenderPort , uint32_t nSessionID);
	uint16_t getStateID(){ return eRoomSate_WaitReady ; }
};

// game end 
class IRoomStateGameEnd
	: public IRoomState
{
public:
	void enterState(IRoom* pRoom)override
	{
		IRoomState::enterState(pRoom) ;
		pRoom->onGameDidEnd();
	}
	uint16_t getStateID(){ return eRoomState_GameEnd ; }
};

//// close state 
//class IRoomStateClosed
//	:public IRoomState
//{
//public:
//	IRoomStateClosed(){ m_nState = eRoomState_Close;}
//	void enterState(IRoom* pRoom )override
//	{ 
//		m_pRoom = pRoom ;
//	}
//
//	void update(float)override
//	{
//		if ( m_pRoom->getDelegate() == nullptr || m_pRoom->getDelegate()->isRoomShouldClose(m_pRoom) == false )
//		{
//			m_pRoom->goToState(eRoomState_WaitJoin) ;
//		}
//	}
//protected:
//	IRoom* m_pRoom ;
//};
//
//// did game over 
//class IRoomStateDidGameOver
//	:public IRoomState
//{
//public:
//	enum { eStateID = eRoomState_Dead };
//public:
//	IRoomStateDidGameOver(){ m_nState = eRoomState_DidGameOver; }
//	virtual ~IRoomStateDidGameOver(){}
//	virtual void enterState(IRoom* pRoom)
//	{
//		m_pRoom = pRoom ;
//		pRoom->onGameDidEnd();
//	}
//
//	void update(float)override
//	{
//		// check close 
//		if ( m_pRoom->getDelegate() && m_pRoom->getDelegate()->isRoomShouldClose(m_pRoom) )
//		{
//			m_pRoom->goToState(eRoomState_Close) ;
//			return ;
//		}
//		m_pRoom->goToState(eRoomState_WaitJoin) ;
//	}
//protected:
//	IRoom* m_pRoom ;
//};
