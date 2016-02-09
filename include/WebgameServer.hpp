#ifndef WEBGAME_SERVER_HPP_INCLUDED
#define WEBGAME_SERVER_HPP_INCLUDED

#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Util/ServerApplication.h"

class RequestHandler: public Poco::Net::HTTPRequestHandler {
public:
	void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
};

class RequestHandlerFactory: public Poco::Net::HTTPRequestHandlerFactory {
public:
	Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);
};

class WebgameServer: public Poco::Util::ServerApplication {
private:
	bool _helpRequested;
protected:
	void initialize(Poco::Util::Application& self);
	void uninitialize();
	void defineOptions(Poco::Util::OptionSet& options);
	void handleOption(const std::string& name, const std::string& value);
	void displayHelp();
	int main(const std::vector<std::string>& args);
public:
	static WebgameServer& instance() {
		return dynamic_cast<WebgameServer &>(ServerApplication::instance());
	}
	WebgameServer();
	~WebgameServer();
};

#endif
