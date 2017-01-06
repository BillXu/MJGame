#pragma once 
#include "IGlobalModule.h"
#include "./HttpServer/server.hpp"
class CHttpModule
	:public IGlobalModule
{
public:
	typedef boost::function<bool (http::server::connection_ptr)> httpHandle;
public:
	void init(IServerApp* svrApp)override;
	void update(float fDeta)override;
	bool registerHttpHandle(std::string strURI, httpHandle pHandle );
protected:
	bool onHandleVXPayResult(http::server::connection_ptr ptr );
	bool onHandleAliPayResult(http::server::connection_ptr ptr);
	bool handleGetPlayerInfo(http::server::connection_ptr ptr );
	bool handleAddRoomCard(http::server::connection_ptr ptr);
	bool handleLePayResult(http::server::connection_ptr ptr);
	static bool url_decode(const std::string& in, std::string& out);
protected:
	boost::shared_ptr<http::server::server> mHttpServer;
	std::map<std::string, httpHandle> vHttphandles;
};