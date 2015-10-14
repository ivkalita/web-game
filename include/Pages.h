#pragma once

#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"

namespace Pages {

	void Index(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
	void Database(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);

}