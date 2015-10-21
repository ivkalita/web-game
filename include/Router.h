#pragma once

#include "Poco/SingletonHolder.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/JSON/Object.h"

class Router {
private:
	typedef void RouteHandler(Poco::Net::HTTPServerRequest&, Poco::Net::HTTPServerResponse&, Poco::JSON::Object::Ptr);
	std::map<std::string, RouteHandler *> Routes;
public:
	Router() {}
	~Router() {}
	static Router& instance();
	void Register(std::string URI, RouteHandler * handler);
	void Process(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
};
