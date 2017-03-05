#include "MJRoomStateDoPlayerAct.h"
class DCMJRoomStateDoPlayerAct
	:public MJRoomStateDoPlayerAct
{
public:
	void onStateTimeUp()override
	{
		if (eMJAct_Chi == m_eActType || eMJAct_Peng == m_eActType)
		{
			Json::Value jsValue;
			jsValue["idx"] = m_nActIdx;
			getRoom()->goToState(eRoomState_WaitPlayerAct, &jsValue);
			return;
		}

		MJRoomStateDoPlayerAct::onStateTimeUp();
	}
};