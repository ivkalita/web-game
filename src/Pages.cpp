#include "Pages.h"

#include "Poco/JSON/Template.h"
#include "Poco/JSON/Object.h"
#include "Poco/Dynamic/Var.h"

//#include "libpq-fe.h"

using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

namespace Pages {

	void Index(HTTPServerRequest& request, HTTPServerResponse& response) {
		std::ostream& st = response.send();
		try {
			Poco::JSON::Template tmp("views/index.html");
			tmp.parse();
			Poco::JSON::Object params;
			params.set("title", "Main Page");
			params.set("number", 42);
			Poco::Dynamic::Var var(params);
			tmp.render(var, st);
		}
		catch (const std::exception& e) {
			st << "Error: " << e.what();
		}
		st.flush();
	}

	void Database(HTTPServerRequest& request, HTTPServerResponse& response) {
		/* Database interaction example */
		/*response.setContentType("text/html");
		std::stringstream ostr;
		Application &app = Application::instance();
		string connection_string = app.config().getString("database.connection_string");
		PGconn* conn = PQconnectdb(connection_string.c_str());
		if (PQstatus(conn) != CONNECTION_OK) {
		ostr << "Connection failed" << endl;
		cerr << "Database connection error:" << endl << PQerrorMessage(conn) << endl;
		PQfinish(conn);
		return;
		}
		PGresult* res;
		res = PQexec(conn, "SELECT * FROM PLAYERS");
		if (PQresultStatus(res) == PGRES_TUPLES_OK) {
		ostr << "All players in database:";
		ostr << "<br><br>";
		} else {
		cerr << "SELECT failed: " << endl << PQerrorMessage(conn) << endl;
		ostr << "Statement execution failed" << endl;
		PQfinish(conn);
		return;
		}
		for (int i = 0; i < PQntuples(res); i++) { // PQntuples - count of rows
		ostr << i + 1 << " - " << PQgetvalue(res, i, PQfnumber(res, "login"));
		ostr << "<br>";
		}
		PQclear(res);
		PQfinish(conn);*/
		//ostr.flush();

		std::ostream& st = response.send();
		try {
			Poco::JSON::Template tmp("views/base.html");
			tmp.parse();
			Poco::JSON::Object params;
			params.set("title", "Database test");
			params.set("content", "Database disabled");
			//params.set("content", ostr.str());
			Poco::Dynamic::Var var(params);
			tmp.render(var, st);
		}
		catch (const std::exception& e) {
			st << "Error: " << e.what();
		}
		st.flush();
	}

}