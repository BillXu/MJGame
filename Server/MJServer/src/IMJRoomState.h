#pragma once
#include "json/json.h"
#include "NativeTypes.h"
#include "messageIdentifer.h"
class IMJRoom;
class IMJRoomState
{
public:
	IMJRoomState(){ m_fStateDuring = 99999; m_pRoom = nullptr; };
	virtual ~IMJRoomState(){}
	virtual void enterRoom(IMJRoom* pmjRoom, Json::Value& jsTranData)
	{
		m_pRoom = pmjRoom;
	}
	virtual uint32_t getStateID() = 0;
	virtual bool onMsg(Json::Value& prealMsg, uint16_t nMsgType, eMsgPort eSenderPort, uint32_t nSessionID){ return false; }
	virtual void onStateTimeUp(){}
	virtual void leaveState(){}
	virtual void update(float fDeta)
	{
		if (m_fStateDuring >= 0.0f)
		{
			m_fStateDuring -= fDeta;
			if (m_fStateDuring <= 0.0f)
			{
				onStateTimeUp();
			}
		}
	}
	void setStateDuringTime(float fTime){ m_fStateDuring = fTime; }
	float getStateDuring(){ return m_fStateDuring; }
	IMJRoom* getRoom(){ return m_pRoom; }
private:
	float m_fStateDuring;
	IMJRoom* m_pRoom;
};