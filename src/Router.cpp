#include "Router.h"

#include "Pages.h"
#include "Users.h"

Router::Router() {
	routines.push_back(Routine("/", Pages::Index));
	routines.push_back(Routine("/login", Pages::Login));
	routines.push_back(Routine("/register", Pages::Register));
	routines.push_back(Routine("/database", Pages::Database));
}

Router& Router::instance() {
	static Poco::SingletonHolder<Router> sh;
	return *sh.get();
}

void Router::Process(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
	std::string URI = request.getURI();
	for (auto it = routines.begin(); it != routines.end(); ++it)
		if (it->URI == URI) {
			it->handler(request, response);
			return;
		}

	try {
		response.sendFile("web/" + URI,
			"text/" + URI.substr(URI.find_last_of(".") + 1, URI.length()));
	}
	catch (const Poco::FileException& e) {
		std::ostream& st = response.send();
		st << "404" << std::endl << e.className();
		st.flush();
	}
	/*catch (OpenFileException & e) {
	std::ostream& st = response.send();
	st << "403";
	st.flush();
	}*/
}