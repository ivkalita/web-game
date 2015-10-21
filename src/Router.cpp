#include "Router.h"
#include "DBConnector.hpp"

#include "Pages.h"

#include "Poco/JSON/Object.h"
#include "Poco/Exception.h"

void Router::Register(std::string URI, RouteHandler * handler) {
	Routes.insert(pair<std::string, RouteHandler *>(URI, handler));
}

Router& Router::instance() {
	static Poco::SingletonHolder<Router> sh;
	return *sh.get();
}

void Router::Process(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
	try {
		std::string URI = request.getURI();
		auto it = Routes.find(URI);
		if (it != Routes.end()) {
			Poco::JSON::Object::Ptr params(new Poco::JSON::Object);
			AddUserInfo(request, params);
			it->second(request, response, params);
			return;
		}
		response.sendFile("web/" + URI,
			"text/" + URI.substr(URI.find_last_of(".") + 1, URI.length()));
	}
	catch (const ConnectionException& e) {
		std::ostream& st = response.send();
		st << "DBConnectionException: " << e.what() << std::endl;
		st.flush();
	}
	catch (const Poco::OpenFileException& e) {
		std::ostream& st = response.send();
		st << "403" << std::endl << e.className();
		st.flush();
	}
	catch (const Poco::FileException& e) {
		std::ostream& st = response.send();
		st << "404" << std::endl << e.className();
		st.flush();
	}
	catch (const Poco::Exception& e) {
		std::ostream& st = response.send();
		st << "PocoException, " << e.className() << ":" << e.displayText() << std::endl;
		st << "Message: " << e.message() << std::endl << "Name: " << e.name() << std::endl;
		st.flush();
	}
	catch (const std::exception& e) {
		std::ostream& st = response.send();
		st << "Error: " << e.what();
		st.flush();
	}
}