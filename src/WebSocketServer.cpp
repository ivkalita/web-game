#include "WebSocketServer.hpp"

using Poco::Net::ServerSocket;
using Poco::Net::WebSocket;
using Poco::Net::WebSocketException;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Timestamp;
using Poco::ThreadPool;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;

using namespace std;

void PageRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    response.setChunkedTransferEncoding(true);
    response.setContentType("text/html");
    std::ostream& ostr = response.send();
    Application &app = Application::instance();
    string connection_string = app.config().getString("database.connection_string");

    PGconn* conn = PQconnectdb(connection_string.c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
        ostr << "Connection failed" << endl;
    }
  
    PGresult* res;
    res = PQexec(conn, "SELECT * FROM PLAYERS"); 

    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        ostr << "All players in database:";
        ostr << "<br><br>";
    }
    else {
        cerr << "SELECT failed " << PQerrorMessage(conn) << endl;
        PQclear(res);
    }

    for (int i = 0; i < PQntuples(res); i++) { // PQntuples - count of rows
        ostr << i + 1 << " - " << PQgetvalue(res, i, PQfnumber(res, "login"));
        ostr << "<br>";
    }

    PQclear(res);
    PQfinish(conn);
}


void WebSocketRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    Application& app = Application::instance();

    try {
        WebSocket ws(request, response);
        app.logger().information("WebSocket connection established.");
        char buffer[1024];
        int flags;
        int n;

        do {
            n = ws.receiveFrame(buffer, sizeof(buffer), flags);
            app.logger().information(Poco::format("Frame received (length=%d, flags=0x%x).", n, unsigned(flags)));
            ws.sendFrame(buffer, n, flags);
        } while (n > 0 || (flags & WebSocket::FRAME_OP_BITMASK) != WebSocket::FRAME_OP_CLOSE);

        app.logger().information("WebSocket connection closed.");
    }

    catch (WebSocketException& exc) {
        app.logger().log(exc);

        switch (exc.code()) {
        case WebSocket::WS_ERR_HANDSHAKE_UNSUPPORTED_VERSION:
            response.set("Sec-WebSocket-Version", WebSocket::WEBSOCKET_VERSION);
            // fallthrough
        case WebSocket::WS_ERR_NO_HANDSHAKE:
        case WebSocket::WS_ERR_HANDSHAKE_NO_VERSION:
        case WebSocket::WS_ERR_HANDSHAKE_NO_KEY:
            response.setStatusAndReason(HTTPResponse::HTTP_BAD_REQUEST);
            response.setContentLength(0);
            response.send();
            break;
        }
    }
}

HTTPRequestHandler* RequestHandlerFactory::createRequestHandler(const HTTPServerRequest& request) {
    Application& app = Application::instance();

    app.logger().information("Request from "
        + request.clientAddress().toString()
        + ": "
        + request.getMethod()
        + " "
        + request.getURI()
        + " "
        + request.getVersion());

    for (HTTPServerRequest::ConstIterator it = request.begin(); it != request.end(); ++it) {
        app.logger().information(it->first + ": " + it->second);
    }

    if (request.find("Upgrade") != request.end() && Poco::icompare(request["Upgrade"], "websocket") == 0)
        return new WebSocketRequestHandler;
    else
        return new PageRequestHandler;
}

WebSocketServer::WebSocketServer() : _helpRequested(false) {}

WebSocketServer::~WebSocketServer() {}

void WebSocketServer::initialize(Application& self) {
    loadConfiguration();

    //initialize connection_string for database and load it into config
    map<string, string> database_configs;
    database_configs.insert(pair<string, string>("user", "postgres"));
    database_configs.insert(pair<string, string>("hostaddr", "127.0.0.1"));
    database_configs.insert(pair<string, string>("port", "5432"));
    database_configs.insert(pair<string, string>("dbname", "galcon"));
    database_configs.insert(pair<string, string>("password", ""));
    string connection_string = "";
    for (auto it = database_configs.begin(); it != database_configs.end(); ++it) {
        string key = "database." + it->first;
        string value = config().getString(key, it->second);
        connection_string += it->first + "=" + value + " ";
    }
    config().setString("database.connection_string", connection_string);
    ServerApplication::initialize(self);
}

void WebSocketServer::uninitialize() {
    ServerApplication::uninitialize();
}

void WebSocketServer::defineOptions(OptionSet& options) {
    ServerApplication::defineOptions(options);

    options.addOption(
        Option("help", "h", "display help information on command line arguments")
        .required(false)
        .repeatable(false));
}

void WebSocketServer::handleOption(const std::string& name, const std::string& value) {
    ServerApplication::handleOption(name, value);

    if (name == "help")
        _helpRequested = true;
}

void WebSocketServer::displayHelp() {
    HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader("A sample HTTP server supporting the WebSocket protocol.");
    helpFormatter.format(std::cout);
}

int WebSocketServer::main(const std::vector<std::string>& args) {
    if (_helpRequested) {
        displayHelp();
    }

    else {
        unsigned short port = (unsigned short)config().getInt("application.port", 1336);
        ServerSocket svs(port);
        HTTPServer srv(new RequestHandlerFactory, svs, new HTTPServerParams);
        srv.start();
        waitForTerminationRequest();
        srv.stop();
    }

    return Application::EXIT_OK;
}