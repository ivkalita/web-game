#pragma once

#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/JSON/Object.h"

namespace Pages {

	void Index(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, Poco::JSON::Object::Ptr params);
	void Database(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, Poco::JSON::Object::Ptr params);

}

void AddMessage(Poco::JSON::Object::Ptr params, std::string type, std::string content);
void TemplateRender(Poco::Net::HTTPServerResponse& response, Poco::JSON::Object::Ptr params, std::string tpl_path);