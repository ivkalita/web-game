#include "Router.h"
#include "Pages.h"

#include "Poco/JSON/Template.h"
#include "Poco/JSON/Object.h"
#include "Poco/Dynamic/Var.h"

#include "libpq-fe.h"
#include "Poco/Util/ServerApplication.h"

using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

void AddMessage(Poco::JSON::Object::Ptr params, std::string type, std::string content) {
	Poco::JSON::Object::Ptr inner = new Poco::JSON::Object;
	inner->set("type", type); //"warning", "danger", "success", "info"
	inner->set("content", content);
	Poco::JSON::Array::Ptr messages = params->getArray("messages");
	if (!messages) {
		messages = new Poco::JSON::Array;
		params->set("messages", messages);
	}
	messages->add(inner);
}

void TemplateRender(HTTPServerResponse& response, Poco::JSON::Object::Ptr params, std::string tpl_path) {
	Poco::JSON::Template tpl("views/" + tpl_path);
	tpl.parse();
	std::ostream& st = response.send();
	tpl.render(params, st);
	st.flush();
}

using namespace std;

class Pages {
public:
	Pages() {
		Router::instance().Register("/", Index);
		Router::instance().Register("/database", Database);
	}
private:
	static void Index(HTTPServerRequest& request, HTTPServerResponse& response, Poco::JSON::Object::Ptr params) {
		params->set("title", "Main Page");
		params->set("number", 42);
		TemplateRender(response, params, "index.html");
	}

	static void Database(HTTPServerRequest& request, HTTPServerResponse& response, Poco::JSON::Object::Ptr params) {
		/* Database interaction example */
		response.setContentType("text/html");
		std::stringstream ostr;
		Poco::Util::Application &app = Poco::Util::Application::instance();

		map<string, string> database_configs;
		database_configs.insert(pair<string, string>("user", "postgres"));
		database_configs.insert(pair<string, string>("hostaddr", "127.0.0.1"));
		database_configs.insert(pair<string, string>("port", "5432"));
		database_configs.insert(pair<string, string>("dbname", "galcon"));
		database_configs.insert(pair<string, string>("password", ""));
		string connection_string = "";
		for (auto it = database_configs.begin(); it != database_configs.end(); ++it) {
			string key = "database." + it->first;
			string value = app.config().getString(key, it->second);
			if (value.length() > 0) {
				connection_string += it->first + "=" + value + " ";
			}
		}

		//app.config().setString("database.connection_string", connection_string);
		//std::string connection_string = app.config().getString("database.connection_string");

		PGconn* conn = PQconnectdb(connection_string.c_str());
		if (PQstatus(conn) != CONNECTION_OK) {
			ostr << "Connection failed" << std::endl;
			std::cerr << "Database connection error:" << std::endl << PQerrorMessage(conn) << std::endl;
			PQfinish(conn);
			return;
		}
		PGresult* res;
		res = PQexec(conn, "SELECT * FROM games");
		if (PQresultStatus(res) == PGRES_TUPLES_OK) {
		ostr << "All games in database:";
		ostr << "<br><br>";
		} else {
			std::cerr << "SELECT failed: " << std::endl << PQerrorMessage(conn) << std::endl;
			ostr << "Statement execution failed" << std::endl;
			PQfinish(conn);
			return;
		}
		for (int i = 0; i < PQntuples(res); i++) { // PQntuples - count of rows
		ostr << i + 1 << " - " << PQgetvalue(res, i, PQfnumber(res, "name"));
		ostr << "<br>";
		}
		PQclear(res);
		PQfinish(conn);
		ostr.flush();

		params->set("title", "Database test");
		params->set("content", ostr.str());
		TemplateRender(response, params, "base.html");
	}
};

static Pages pages;