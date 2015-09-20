#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/NetException.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Format.h"
#include <iostream>

class PageRequestHandler: public Poco::Net::HTTPRequestHandler {
public:
	void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
};

//Handle a WebSocket connection.
class WebSocketRequestHandler: public Poco::Net::HTTPRequestHandler { 
public:
	void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
};


class RequestHandlerFactory: public Poco::Net::HTTPRequestHandlerFactory {
public:
	Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);
};


/// The main application class.
///
/// This class handles command-line arguments and
/// configuration files.
/// Start the WebSocketServer executable with the help
/// option (/help on Windows, --help on Unix) for
/// the available command line options.
///
/// To use the sample configuration file (WebSocketServer.properties),
/// copy the file to the directory where the WebSocketServer executable
/// resides. If you start the debug version of the WebSocketServer
/// (WebSocketServerd[.exe]), you must also create a copy of the configuration
/// file named WebSocketServerd.properties. In the configuration file, you
/// can specify the port on which the server is listening (default
/// 9980) and the format of the date/time string sent back to the client.
///
/// To test the WebSocketServer you can use any web browser (http://localhost:9980/).

class WebSocketServer: public Poco::Util::ServerApplication {
private:
	bool _helpRequested;

protected:
	void initialize(Application& self);
		
	void uninitialize();

	void defineOptions(Poco::Util::OptionSet& options);

	void handleOption(const std::string& name, const std::string& value);

	void displayHelp();

	int main(const std::vector<std::string>& args);

public:
	WebSocketServer();
	
	~WebSocketServer();
};