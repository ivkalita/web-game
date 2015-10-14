#include "Users.h"

#include "Poco/JSON/Template.h"
#include "Poco/JSON/Object.h"
#include "Poco/Dynamic/Var.h"

using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

namespace Pages {

	void Login(HTTPServerRequest& request, HTTPServerResponse& response) {
		std::ostream& st = response.send();
		try {
			Poco::JSON::Template tmp("views/login.html");
			tmp.parse();
			Poco::JSON::Object params;
			params.set("title", "Вход в систему");
			Poco::Dynamic::Var var(params);
			tmp.render(var, st);
		}
		catch (const std::exception& e) {
			st << "Error: " << e.what();
		}
		st.flush();
	}

	void Register(HTTPServerRequest& request, HTTPServerResponse& response) {
		std::ostream& st = response.send();
		try {
			Poco::JSON::Template tmp("views/register.html");
			tmp.parse();
			Poco::JSON::Object params;
			params.set("title", "Регистраци¤");
			Poco::Dynamic::Var var(params);
			tmp.render(var, st);
		}
		catch (const std::exception& e) {
			st << "Error: " << e.what();
		}
		st.flush();
	}

}