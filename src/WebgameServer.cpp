#include "WebgameServer.hpp"
#include "DBConnector.hpp"

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

pair<string, string> PageRequestHandler::getFile(HTTPServerRequest& request) {
    string path = "../web/";
    string type;

    string URI = request.getURI();

    if (URI == "/") {
        path += "html/index.html";
        type = "text/html";
    } else {
        string extension = URI.substr(URI.find(".") + 1, URI.length());
        path += URI;
        type = "text/" + extension;  
    }

    return *new pair<string, string>(path, type);
}

void PageRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    response.setChunkedTransferEncoding(true);
    pair<string, string> file = getFile(request);
    response.sendFile(file.first, file.second);

    /* Database interaction example
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
    }
    else {
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
    PQfinish(conn); */
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

WebgameServer::WebgameServer() : _helpRequested(false) {}

WebgameServer::~WebgameServer() {}

void WebgameServer::initialize(Application& self) {
    loadConfiguration();


    DBConnection::instance().Connect(
        config().getString("database.hostaddr", "localhost"),
        config().getString("database.port", "5432"),
        config().getString("database.dbname", "web-game"),
        config().getString("database.user", "web-game"),
        config().getString("database.password", "web-game")
    );

    ServerApplication::initialize(self);
}

void WebgameServer::uninitialize() {
    ServerApplication::uninitialize();
}

void WebgameServer::defineOptions(OptionSet& options) {
    ServerApplication::defineOptions(options);

    options.addOption(
        Option("help", "h", "display help information on command line arguments")
        .required(false)
        .repeatable(false));
}

void WebgameServer::handleOption(const std::string& name, const std::string& value) {
    ServerApplication::handleOption(name, value);

    if (name == "help")
        _helpRequested = true;
}

void WebgameServer::displayHelp() {
    HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader("A sample HTTP server supporting the WebSocket protocol.");
    helpFormatter.format(std::cout);
}

int WebgameServer::main(const std::vector<std::string>& args) {
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
