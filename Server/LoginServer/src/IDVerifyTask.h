#include "ITask.h"
#include "httpRequest.h"
class IDVerifyTask
	:public ITask
	,public CHttpRequestDelegate
{
public:
	IDVerifyTask(uint32_t nTaskID);
	uint8_t performTask()override;
	void onHttpCallBack(char* pResultData, size_t nDatalen, void* pUserData, size_t nUserTypeArg)override;
	void setVerifyInfo(const char* pName, const char* pID );
	bool isVerifyOk(){ return m_isVerifyOk; }
protected:
	CHttpRequest m_tHttpRequest;
	std::string m_strVeifyName;
	std::string m_strCardID;
	bool m_isVerifyOk;
};