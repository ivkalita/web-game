#pragma once

#include "Poco/SingletonHolder.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"

class Router {
private:
	typedef void RoutineHandler(Poco::Net::HTTPServerRequest&, Poco::Net::HTTPServerResponse&);
	struct Routine {
		std::string URI;
		RoutineHandler * handler;
		Routine(std::string _URI, RoutineHandler * _handler) : URI(_URI), handler(_handler) {}
	};
	std::vector<Routine> routines;
public:
	Router();
	~Router() {}
	static Router& instance();
	void Process(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
};
