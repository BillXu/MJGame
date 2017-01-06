#include "tinyxml/tinyxml.h"
#include "Md5.h"
#include "HttpModule.h"
#include "log4z.h"
#include <boost/algorithm/string.hpp>  
#include "VerifyApp.h"
#include "ConfigDefine.h"
#include "json/json.h"
#include "ISeverApp.h"
#include "AsyncRequestQuene.h"
#include "alipaySDK\openapi\openapi_client.h"
void CHttpModule::init(IServerApp* svrApp)
{
	IGlobalModule::init(svrApp);

	std::string strNotifyUrl = Wechat_notifyUrl;
	// parse port ;
	std::size_t nPosDot = strNotifyUrl.find_last_of(':');
	std::size_t nPosSlash = strNotifyUrl.find_last_of('/');
	uint16_t nPort = 80;
	if (nPosDot != std::string::npos && std::string::npos != nPosSlash)
	{
		auto strPort = strNotifyUrl.substr(nPosDot + 1 , nPosSlash - nPosDot - 1 );
		nPort = atoi(strPort.c_str());
		if (0 == nPort)
		{
			nPort = 80;
		}
	}
	//
	mHttpServer = boost::make_shared<http::server::server>(nPort);
	mHttpServer->run();

	// parse uri 
	std::size_t nPos = strNotifyUrl.find_last_of('/');
	std::string strUri = strNotifyUrl.substr(nPos,strNotifyUrl.size() - nPos );
	registerHttpHandle( strUri, boost::bind(&CHttpModule::onHandleVXPayResult, this, boost::placeholders::_1));

	registerHttpHandle("/alipay.7z", boost::bind(&CHttpModule::onHandleAliPayResult, this, boost::placeholders::_1));

	registerHttpHandle("/playerInfo.yh", boost::bind(&CHttpModule::handleGetPlayerInfo, this, boost::placeholders::_1));
	registerHttpHandle("/addRoomCard.yh", boost::bind(&CHttpModule::handleAddRoomCard, this, boost::placeholders::_1)); 
	registerHttpHandle("/lepay.php", boost::bind(&CHttpModule::handleLePayResult, this, boost::placeholders::_1));
}

void CHttpModule::update(float fDeta)
{
	IGlobalModule::update(fDeta);
	std::set<http::server::connection_ptr> vOut;
	if (!mHttpServer->getRequestConnects(vOut))
	{
		return;
	}

	for (auto& ref : vOut)
	{
		auto req = ref->getReqPtr();
		auto pReply = ref->getReplyPtr();
		auto iter = vHttphandles.find( req->uri );
		if (iter == vHttphandles.end())
		{
			LOGFMTE("no handle for uri = %s",req->uri.c_str());
			*pReply = http::server::reply::stock_reply(http::server::reply::bad_request);
			ref->doReply();
			continue;
		}
		
		auto pfunc = iter->second;
		if ( !pfunc(ref) )
		{
			*pReply = http::server::reply::stock_reply(http::server::reply::bad_request);
			ref->doReply();
		}
	}
	vOut.clear();
}

bool CHttpModule::registerHttpHandle(std::string strURI, httpHandle pHandle)
{
	auto iter = vHttphandles.find(strURI);
	if (iter != vHttphandles.end())
	{
		LOGFMTE("already register handle for uri = %s",strURI.c_str());
		return false;
	}
	vHttphandles[strURI] = pHandle;
	return true;
}


 std::string getXmlNodeValue(const char* pnodeName, TiXmlNode* pRoot)
{
	std::string str = "";
	TiXmlElement* pValueParent = (TiXmlElement*)pRoot->FirstChild(pnodeName);
	if (pValueParent)
	{
		TiXmlNode* pValue = pValueParent->FirstChild();
		if (pValue)
		{
			str = pValue->Value();
			return str;
		}
	}
	LOGFMTE("xml node = %s value is null", pnodeName);
	return str;
}

// process vx pay result 
bool CHttpModule::onHandleVXPayResult(http::server::connection_ptr ptr)
{
	auto req = ptr->getReqPtr();
	auto res = ptr->getReplyPtr();

	if (req->contentSize == 0)
	{
		return false;
	}

	// parse xml 
	TiXmlDocument t;
	t.Parse(req->reqContent.c_str(), 0, TIXML_ENCODING_UTF8);
	if (t.Error())
	{
		LOGFMTE("vx pay parse xml error : %s",t.ErrorDesc());
		return false;
	}
#ifndef Node_Value_F  
#define node_Value( x ) getXmlNodeValue((x),pRoot)
#define Node_Value_F ;
#endif 
	TiXmlNode* pRoot = t.RootElement();
	uint8_t nRet = 0;
	do
	{
		if (!pRoot)
		{
			nRet = 1;
			break;
		}

		auto retCode = node_Value("return_code");
		if (retCode != "SUCCESS")
		{
			nRet = 3;
			auto strEmsg = node_Value("return_msg");
			LOGFMTE("ret msg : %s", strEmsg.c_str());
			break;
		}
 
		// do parse 
		auto fee = node_Value("total_fee");
		auto strTradeNo = node_Value("out_trade_no");
		auto payResult = node_Value("result_code");
		auto payTime = node_Value("time_end");
		if ("FAIL" == payResult)
		{
			return true ;
		}
		
		std::vector<std::string> vOut;
		boost::split(vOut,strTradeNo,boost::is_any_of("E"));
		if (vOut.size() < 2)
		{
			LOGFMTE("trade out error = %s",strTradeNo.c_str() );
			nRet = 4;
			break;
		}
		auto shopItem = vOut[0];
		auto userUID = vOut[1];
		LOGFMTD( "GO TO DB Verify trade = %s , fee = %s payTime = %s",strTradeNo.c_str(),fee.c_str(),payTime.c_str() );

		// do DB verify ;
		auto pVeirfyModule = ((CVerifyApp*)getSvrApp())->getTaskPoolModule();
		pVeirfyModule->doDBVerify(atoi(userUID.c_str()), atoi(shopItem.c_str()), ePay_WeChat,strTradeNo);
	} while (0);

	std::string str = "";
	if (nRet != 0)
	{
		str = "<xml><return_code><![CDATA[FAIL]]></return_code> <return_msg><![CDATA[unknown]]></return_msg> </xml> ";
	}
	else
	{
		str = "<xml><return_code><![CDATA[SUCCESS]]></return_code> <return_msg><![CDATA[OK]]></return_msg> </xml> ";
	}

	res->setContent(str,"text/xml");
	ptr->doReply();
	t.SaveFile("reT.xml");
	return true;
}

// process alipay result;
bool CHttpModule::onHandleAliPayResult(http::server::connection_ptr ptr)
{
	auto req = ptr->getReqPtr();
	auto res = ptr->getReplyPtr();
	LOGFMTD("recv alipay resp : %s ",req->reqContent.c_str());
	//req->reqContent = "total_amount=0.10&buyer_id=2088412132415960&trade_no=2016112121001004960273347388&notify_time=2016-11-21+19%3A35%3A37&subject=%E9%92%BB%E7%9F%B3&sign_type=RSA&buyer_logon_id=182****4594&auth_app_id=2016111602881028&charset=utf-8&notify_type=trade_status_sync&invoice_amount=0.10&out_trade_no=80E183E147972812841&trade_status=TRADE_SUCCESS&gmt_payment=2016-11-21+19%3A35%3A37&version=1.0&point_amount=0.00&sign=SpUGImVPXhvNUNQ4cSwIpQfbQFu4vgOlJrzYjq8Ifm2ArgNlvvtRvpFxzK%2BHmcCu72T8bDU0%2Fw%2FKbazMY4YV6B7QNCQoGVcYm8K9iG6vwr92bIvogcYoqNIJrKgwhtQaLAoeKu9ymrRjDoxz0%2FsmdKLxhI7eDrHe99M24rHoFNM%3D&gmt_create=2016-11-21+19%3A35%3A37&buyer_pay_amount=0.10&receipt_amount=0.10&fund_bill_list=%5B%7B%22amount%22%3A%220.10%22%2C%22fundChannel%22%3A%22ALIPAYACCOUNT%22%7D%5D&app_id=2016111602881028&seller_id=2088521286957872&notify_id=cece49855c34a839a401c3ecc0cd1e4neq&seller_email=hyzfb%407zplay.com ";
	
	std::map<std::string, std::string> strMap;
	for (uint16_t nIdx = 0; nIdx < req->reqContent.size();)
	{
		auto keyPos = req->reqContent.find_first_of("=",nIdx);
		auto strKey = req->reqContent.substr(nIdx,keyPos - nIdx );

		auto valueStart = keyPos + 1;
		auto valuePos = req->reqContent.find_first_of("&", valueStart );
		if (std::string::npos == valuePos)
		{
			valuePos = req->reqContent.size();
		}
		auto strValue = req->reqContent.substr(valueStart, valuePos - valueStart);
		strMap[strKey] = strValue;

		nIdx = valuePos + 1;
	}
	
	
	//OpenapiClient openapiClient(OpenapiClient::default_appID,
	//	OpenapiClient::KEY_PRIVATE,
	//	OpenapiClient::default_url,
	//	OpenapiClient::default_charset,
	//	OpenapiClient::KEY_PUBLIC);
	//auto ret = openapiClient.analyzeResponse(req->reqContent);
	//if (ret.empty())
	//{
	//	LOGFMTE("recve alipay respone but verify sign failed ?");
	//	std::string str = "failed";
	//	res->setContent(str, "text/xml");
	//	ptr->doReply();
	//	return true;
	//}

	//JsonType jsonObj = JsonUtil::stringToObject(ret);
	auto resultMap = strMap;
	auto statusIter = resultMap.find("trade_status");
	if (statusIter == resultMap.end())
	{
		LOGFMTE("why do not have trade status key ? ");
		std::string str = "failed";
		res->setContent(str, "text/xml");
		ptr->doReply();
		return true;
	}

	std::string strStatus = statusIter->second;
	LOGFMTD("respone status: %s",strStatus.c_str());
	if (strcmp(strStatus.c_str(), "TRADE_SUCCESS")) // not success 
	{
		// but we do processed ,do not tell me again ;
		std::string str = "success";
		res->setContent(str, "text/xml");
		ptr->doReply();
		return true;
	}

	auto iterAlipayTradeNo = resultMap.find("trade_no");
	if (iterAlipayTradeNo == resultMap.end())
	{
		LOGFMTE("alipay out trade_no is nullptr");
		std::string str = "success";
		res->setContent(str, "text/xml");
		ptr->doReply();
		return true;
	}

	auto iterTradeNo = resultMap.find("out_trade_no");
	if (iterTradeNo == resultMap.end())
	{
		LOGFMTE("out trade_no is nullptr");
		std::string str = "success";
		res->setContent(str, "text/xml");
		ptr->doReply();
		return true;
	}
	// go on do db verfiy ;
	// do DB verify ;
	std::vector<std::string> vOut;
	boost::split(vOut, iterTradeNo->second, boost::is_any_of("E"));
	if (vOut.size() < 2)
	{
		LOGFMTE("trade out error = %s", iterTradeNo->second.c_str());
		LOGFMTE("trade_no is nullptr");
		std::string str = "success";
		res->setContent(str, "text/xml");
		ptr->doReply();
		return true;
	}
	LOGFMTD("all right go on db verify Zhi fu bao ");
	auto shopItem = vOut[0];
	auto userUID = vOut[1];
	auto pVeirfyModule = ((CVerifyApp*)getSvrApp())->getTaskPoolModule();
	pVeirfyModule->doDBVerify(atoi(userUID.c_str()), atoi(shopItem.c_str()), ePay_ZhiFuBao, iterAlipayTradeNo->second);
	std::string str = "success";
	res->setContent(str, "text/xml");
	ptr->doReply();
	return true;
}

bool CHttpModule::handleGetPlayerInfo(http::server::connection_ptr ptr)
{
	auto req = ptr->getReqPtr();
	auto res = ptr->getReplyPtr();
	LOGFMTD("reciveget player info req = %s",req->reqContent.c_str());

	Json::Reader jsReader;
	Json::Value jsRoot;
	auto bRet = jsReader.parse(req->reqContent, jsRoot);
	if (!bRet)
	{
		LOGFMTE("parse agent get player info argument error");
		return false;
	}

	uint32_t nUID = 0;
	if (jsRoot["playerUID"].isNull() || jsRoot["playerUID"].isUInt() == false )
	{
		LOGFMTD("cant not finn uid argument");
		return false;
	}
	nUID = jsRoot["playerUID"].asUInt();

	// do async request 
	Json::Value jsReq;
	jsReq["targetUID"] = nUID;
	auto async = getSvrApp()->getAsynReqQueue();
	async->pushAsyncRequest(ID_MSG_PORT_DATA, eAsync_AgentGetPlayerInfo, jsReq, [this, ptr, nUID, async](uint16_t nReqType, const Json::Value& retContent, Json::Value& jsUserData)
	{
		// define help function , fetch not process mail card , and do respone
		auto lpfCheckDBMail = [](CAsyncRequestQuene* async, http::server::connection_ptr ptr, uint32_t nUID, Json::Value& jsAgentBack)
		{
			// take not process add card mail in to account 
			Json::Value jsSql;
			char pBuffer[512] = { 0 };
			sprintf(pBuffer, "SELECT * FROM mail WHERE userUID = '%u' and mailType = %u and state = '0' order by postTime desc limit 5 ;", nUID, eMailType::eMail_AddRoomCard);
			jsSql["sql"] = pBuffer;
			async->pushAsyncRequest(ID_MSG_PORT_DB, eAsync_DB_Select, jsSql, [ptr, nUID](uint16_t nReqType, const Json::Value& retContent, Json::Value& jsUserData)
			{
				int32_t nTotalCnt = 0;
				int32_t nAddCoin = 0;
				int32_t nAddDiamond = 0;
				uint8_t nRow = retContent["afctRow"].asUInt();
				Json::Value jsData = retContent["data"];
				for (uint8_t nIdx = 0; nIdx < jsData.size(); ++nIdx)
				{
					Json::Value jsRow = jsData[nIdx];

					Json::Reader jsReader;
					Json::Value jsC;
					auto bRt = jsReader.parse(jsRow["mailContent"].asString(), jsC);
					if (!bRt || jsC["addCard"].isNull() || jsC["addCoin"].isNull() || jsC["addDiamond"].isNull() )
					{
						LOGFMTE("pasre add card mail error id = %u", nUID);
						continue;
					}
					nTotalCnt += jsC["addCard"].asInt();
					nAddCoin += jsC["addCoin"].asInt();
					nAddDiamond += jsC["addDiamond"].asInt();
				}
				LOGFMTD("uid = %u mail card cnt = %d",nUID,nTotalCnt);
				jsUserData["cardCnt"] = jsUserData["cardCnt"].asInt() + nTotalCnt;
				jsUserData["coin"] = jsUserData["coin"].asInt() + nAddCoin;
				jsUserData["diamond"] = jsUserData["diamond"].asInt() + nAddDiamond;

				// build msg to send ;
				auto res = ptr->getReplyPtr();
				Json::StyledWriter jswrite;
				auto str = jswrite.write(jsUserData);
				res->setContent(str, "text/json");
				ptr->doReply();
				LOGFMTD("do get player info cards uid = %u", nUID);
			}, jsAgentBack);
		};

		bool isOnline = retContent["isOnline"].asUInt() == 1;
		if (isOnline)
		{
			Json::Value jsAgentBack;
			jsAgentBack["ret"] = 1;
			jsAgentBack["name"] = retContent["name"];
			jsAgentBack["playerUID"] = nUID;
			jsAgentBack["cardCnt"] = retContent["leftCardCnt"];
			jsAgentBack["coin"] = retContent["coin"];
			jsAgentBack["diamond"] = retContent["diamond"];

			lpfCheckDBMail(async,ptr,nUID,jsAgentBack);

			return;
		}
		
		LOGFMTD("uid = %u not online get info from db ",nUID);
		// not online , must get name first ;
		Json::Value jsSql;
		char pBuffer[512] = { 0 };
		sprintf(pBuffer, "SELECT playerName, diamond,coin,vipRoomCardCnt FROM playerbasedata WHERE userUID = '%u' ;", nUID );
		jsSql["sql"] = pBuffer;
		async->pushAsyncRequest(ID_MSG_PORT_DB, eAsync_DB_Select, jsSql, [lpfCheckDBMail,async, ptr, nUID](uint16_t nReqType, const Json::Value& retContent, Json::Value& jsUserData)
		{
			uint8_t nRow = retContent["afctRow"].asUInt();

			Json::Value jsAgentBack;
			jsAgentBack["ret"] = 1;
			jsAgentBack["name"] = "";
			jsAgentBack["playerUID"] = nUID;
			jsAgentBack["cardCnt"] = 0;

			if (nRow == 0)
			{
				jsAgentBack["ret"] = 0;
				// build msg to send ;
				auto res = ptr->getReplyPtr();
				Json::StyledWriter jswrite;
				auto str = jswrite.write(jsAgentBack);
				res->setContent(str, "text/json");
				ptr->doReply();
				LOGFMTE("get info can not find uid = %u info from db",nUID);
				return;
			}
			else
			{
				Json::Value jsData = retContent["data"];
				Json::Value jsRow = jsData[0u];
				jsAgentBack["name"] = jsRow["playerName"];
				jsAgentBack["cardCnt"] = jsRow["vipRoomCardCnt"];
				jsAgentBack["coin"] = jsRow["coin"];
				jsAgentBack["diamond"] = jsRow["diamond"];
				LOGFMTD("uid = %u base data card cnt = %d", nUID, jsRow["diamond"].asInt());
				lpfCheckDBMail(async, ptr, nUID, jsAgentBack);

				return;
			}
		});
	});
	LOGFMTD("do async agent get player info uid = %u", nUID);
	return true;
}

bool CHttpModule::handleAddRoomCard(http::server::connection_ptr ptr)
{
	auto req = ptr->getReqPtr();
	auto res = ptr->getReplyPtr();
	LOGFMTD("reciveget add room card info req = %s", req->reqContent.c_str());

	Json::Reader jsReader;
	Json::Value jsRoot;
	auto bRet = jsReader.parse(req->reqContent, jsRoot);
	if (!bRet)
	{
		LOGFMTE("parse add room card argument error");
		return false;
	}

	uint32_t nUID = 0;
	int32_t nAddCard = 0;
	int32_t nAddCoin = 0;
	int32_t nAddDiamond = 0;
	uint32_t nAddCardNo = 0 ;
	if (jsRoot["playerUID"].isNull() || jsRoot["playerUID"].isUInt() == false )
	{
		LOGFMTD("cant not finn uid argument");
		return false;
	}

	if (jsRoot["addCard"].isNull() || jsRoot["addCard"].isInt() == false)
	{
		LOGFMTD("cant not finn addCard argument");
		return false;
	}

	if (jsRoot["addCoin"].isNull() || jsRoot["addCoin"].isInt() == false)
	{
		LOGFMTD("cant not finn nAddCoin argument");
		return false;
	}

	if (jsRoot["addDiamond"].isNull() || jsRoot["addDiamond"].isInt() == false)
	{
		LOGFMTD("cant not finn nAddDiamond argument");
		return false;
	}

	if (jsRoot["addCardNo"].isNull() || jsRoot["addCardNo"].isUInt() == false)
	{
		LOGFMTD("cant not finn addCardNo argument");
		return false;
	}

	nAddCard = jsRoot["addCard"].asInt();
	nAddCoin = jsRoot["addCoin"].asInt();
	nAddDiamond = jsRoot["addDiamond"].asInt();
	nUID = jsRoot["playerUID"].asUInt();
	nAddCardNo = jsRoot["addCardNo"].asUInt();

	// do async request 
	Json::Value jsReq;
	jsReq["targetUID"] = nUID;
	jsReq["addCard"] = nAddCard;
	jsReq["addCoin"] = nAddCoin;
	jsReq["addDiamond"] = nAddDiamond;
	jsReq["addCardNo"] = nAddCardNo;
	auto async =  getSvrApp()->getAsynReqQueue();
	async->pushAsyncRequest(ID_MSG_PORT_DATA, eAsync_AgentAddRoomCard, jsReq, [this, ptr, nUID, nAddCardNo](uint16_t nReqType, const Json::Value& retContent, Json::Value& jsUserData)
	{
		auto res = ptr->getReplyPtr();
		// do check 
		jsUserData["ret"] = 1;
		Json::StyledWriter jswrite;
		auto str = jswrite.write(jsUserData);
		res->setContent(str, "text/json");
		ptr->doReply();
		LOGFMTD("do agent add room cards uid = %u, addCardNo = %u ", nUID, nAddCardNo);
	}, jsRoot);
	LOGFMTD("do async agent add room cards uid = %u cnt = %d,addCardNo = %u", nUID, nAddCard, nAddCardNo);
	return true;
}

bool CHttpModule::handleLePayResult(http::server::connection_ptr ptr)
{
	auto req = ptr->getReqPtr();
	auto res = ptr->getReplyPtr();
	LOGFMTD("recv lepay resp : %s ", req->reqContent.c_str());
	std::map<std::string, std::string> strMap;
	for (uint16_t nIdx = 0; nIdx < req->reqContent.size();)
	{
		auto keyPos = req->reqContent.find_first_of("=", nIdx);
		auto strKey = req->reqContent.substr(nIdx, keyPos - nIdx);

		auto valueStart = keyPos + 1;
		auto valuePos = req->reqContent.find_first_of("&", valueStart);
		if (std::string::npos == valuePos)
		{
			valuePos = req->reqContent.size();
		}
		auto strValue = req->reqContent.substr(valueStart, valuePos - valueStart);
		strMap[strKey] = strValue;

		nIdx = valuePos + 1;
	}

	auto& resultMap = strMap;

	auto iterAlipayTradeNo = resultMap.find("pxNumber");
	if (iterAlipayTradeNo == resultMap.end())
	{
		LOGFMTE("lePaypxNumber is nullptr");
		std::string str = "failed";
		res->setContent(str, "text/xml");
		ptr->doReply();
		return true;
	}

	auto iterTradeNo = resultMap.find("params");
	if (iterTradeNo == resultMap.end())
	{
		LOGFMTE("lepay params is nullptr");
		std::string str = "failed";
		res->setContent(str, "text/xml");
		ptr->doReply();
		return true;
	}
	// go on do db verfiy ;
	std::string strNum = iterTradeNo->second;
	// do url decode 
	auto isValid = url_decode(iterTradeNo->second, strNum);
	if (isValid == false)
	{
		LOGFMTE("le pay params is not valued urlencode : %s", iterTradeNo->second.c_str());
		std::string str = "failed";
		res->setContent(str, "text/xml");
		ptr->doReply();
		return true;
	}
	LOGFMTD("after urldecode tradeno : %s",strNum.c_str() );
	// do DB verify ;
	std::vector<std::string> vOut;
	boost::split(vOut, strNum, boost::is_any_of("E"));
	if (vOut.size() < 2)
	{
		LOGFMTE("trade out error = %s", strNum.c_str());
		LOGFMTE("trade_no is nullptr");
		std::string str = "success";
		res->setContent(str, "text/xml");
		ptr->doReply();
		return true;
	}
	LOGFMTD("all right go on db verify Le Pay ");
	auto shopItem = vOut[0];
	auto userUID = vOut[1];
	auto pVeirfyModule = ((CVerifyApp*)getSvrApp())->getTaskPoolModule();
	pVeirfyModule->doDBVerify(atoi(userUID.c_str()), atoi(shopItem.c_str()), ePay_LePay, iterAlipayTradeNo->second);
	std::string str = "SUCCESS";
	res->setContent(str, "text/xml");
	ptr->doReply();
}

bool CHttpModule::url_decode(const std::string& in, std::string& out)
{
	out.clear();
	out.reserve(in.size());
	for (std::size_t i = 0; i < in.size(); ++i)
	{
		if (in[i] == '%')
		{
			if (i + 3 <= in.size())
			{
				int value = 0;
				std::istringstream is(in.substr(i + 1, 2));
				if (is >> std::hex >> value)
				{
					out += static_cast<char>(value);
					i += 2;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else if (in[i] == '+')
		{
			out += ' ';
		}
		else
		{
			out += in[i];
		}
	}
	return true;
}

