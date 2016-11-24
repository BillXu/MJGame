#include "IDVerifyTask.h"
#include <json/json.h>
#include "LogManager.h"
IDVerifyTask::IDVerifyTask(uint32_t nTaskID)
	:ITask(nTaskID)
	, m_isVerifyOk(false)
{
	m_tHttpRequest.init("http://v.apix.cn/apixcredit/idcheck/idcard?");
	m_tHttpRequest.setDelegate(this);
}

uint8_t IDVerifyTask::performTask()
{
	m_isVerifyOk = false;
	std::string strApix = "apix-key: ";
	strApix += "81f3bdc1af8643d652d34366b1adf5e5";
	m_tHttpRequest.setHead(strApix.c_str());
	std::string strCombin = "cardno=";
	strCombin += m_strCardID;
	strCombin += "&name=";
	strCombin += m_strVeifyName;
	strCombin += "&type=idcard";
	m_tHttpRequest.performGetRequest(strCombin.c_str());
	return 0;
}

void IDVerifyTask::onHttpCallBack(char* pResultData, size_t nDatalen, void* pUserData, size_t nUserTypeArg)
{
	Json::Reader reader;
	Json::Value rootValue;
	char pTempBuffer[1024] = { 0 };
	memset(pTempBuffer, 0, sizeof(pTempBuffer));
	memcpy(pTempBuffer, pResultData, nDatalen);
	LOGFMTI("verifyRet: %s",pTempBuffer);
	if (reader.parse(pTempBuffer, rootValue))
	{
		this->m_isVerifyOk = (rootValue["code"].isNull() == false && rootValue["code"].asInt() == 0);
	}
	else
	{
		this->m_isVerifyOk = false;
	}
	return;
}

void IDVerifyTask::setVerifyInfo(const char* pName, const char* pID)
{
	m_strVeifyName = pName;
	m_strCardID = pID;
}