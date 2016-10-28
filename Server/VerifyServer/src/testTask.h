#include "ITask.h"
#include "config-win.h"
class CTestTask
	:public ITask
{
public:
	CTestTask(uint32_t nID , uint32_t fTime  )
		:ITask(nID)
	{
		nTimes = fTime ;
	}

	uint8_t performTask()override
	{
		while ( nTimes-- )
		{
			sleep(1);
		}
		return 0 ;
	}

	void reset()
	{ 
		//setCallBack(nullptr);
	}
public:
	uint32_t nTimes ;
	uint32_t nreal;
};