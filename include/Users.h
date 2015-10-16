#pragma once

#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/JSON/Object.h"

namespace Pages {

	void Login(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, Poco::JSON::Object::Ptr params);
	void Logout(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, Poco::JSON::Object::Ptr params);
	void Register(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, Poco::JSON::Object::Ptr params);
	void Profile(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, Poco::JSON::Object::Ptr params);
	
}

void AddUserInfo(Poco::Net::HTTPServerRequest& request, Poco::JSON::Object::Ptr params);