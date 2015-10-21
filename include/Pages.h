#pragma once

#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/JSON/Object.h"

void AddMessage(Poco::JSON::Object::Ptr params, std::string type, std::string content);
void TemplateRender(Poco::Net::HTTPServerResponse& response, Poco::JSON::Object::Ptr params, std::string tpl_path);

void AddUserInfo(Poco::Net::HTTPServerRequest& request, Poco::JSON::Object::Ptr params);