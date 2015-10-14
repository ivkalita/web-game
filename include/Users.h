#pragma once

#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"

namespace Pages {

	void Login(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
	void Register(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);

}