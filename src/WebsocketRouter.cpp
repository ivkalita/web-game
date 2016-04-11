#include "WebsocketRouter.hpp"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/NetException.h"
#include "Poco/JSON/Parser.h"
#include "Poco/URI.h"
#include <sstream>

#include "Router.hpp"
#include "DBConnector.hpp"

Poco::JSON::Object jsonDataResult(std::string result) {
    Poco::JSON::Object r, data;
    r.set("result", "ok");
    data.set("result", result);
    r.set("data", data);
    return r;
}

static Poco::JSON::Object jsonResult(std::string result) {
    Poco::JSON::Object r;
    r.set("result", result);
    return r;
}

Poco::JSON::Object WebsocketRouter::handle(Poco::JSON::Object message, int & user) const {
    std::string action = message.get("action").extract<std::string>();
    auto route = static_cast<WebsocketRoute*>(match("", Poco::URI(action), std::map<std::string, std::string>()));
    if (!route)
        return jsonResult("notFound");
    if (route->isNeedAuth() && user == -1)
        return jsonResult("needAuth");
    message = *message.get("data").extract<Poco::JSON::Object::Ptr>();
    return route->handle(message, user);
}

static Poco::JSON::Object process(std::string & buffer, int & user) {
    Poco::JSON::Object req, res;
    try {
        req = *Poco::JSON::Parser().parse(buffer).extract<Poco::JSON::Object::Ptr>();
    }
    catch (...) {
        res.set("result", "badRequest");
        return res;
    }
    try {
        res.set("result", "ok");
        res.set("action", req.get("action").extract<std::string>());
        res.set("data", WebsocketRouter::instance().handle(req, user));
    }
    catch (...) {
        res.set("result", "internalError");
    }
    return res;
}

static void setOffline(int userId) {
    if (userId != -1)
        DBConnection::instance().ExecParams("UPDATE users SET online = false WHERE id = $1", { std::to_string(userId) });
}

void WebSocketRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
    int user = -1;
    try {
        Poco::Net::WebSocket ws(request, response);
        char buffer[1024];
        int flags, n;

        do {
            n = ws.receiveFrame(buffer, sizeof(buffer), flags);
            Poco::JSON::Object res = process(std::string(buffer, n), user);
            if (res.size() > 0) {
                std::ostringstream st;
                res.stringify(st);
                std::string s = st.str();
                ws.sendFrame(s.c_str(), s.size(), flags);
            }
        } while (n > 0 || (flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) != Poco::Net::WebSocket::FRAME_OP_CLOSE);
        setOffline(user);
    }
    catch (Poco::Net::WebSocketException& exc) {
        switch (exc.code()) {
        case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_UNSUPPORTED_VERSION:
            response.set("Sec-WebSocket-Version", Poco::Net::WebSocket::WEBSOCKET_VERSION);
        case Poco::Net::WebSocket::WS_ERR_NO_HANDSHAKE:
        case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_NO_VERSION:
        case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_NO_KEY:
            response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
            response.setContentLength(0);
            response.send();
            break;
        }
    }
    catch (Poco::Net::ConnectionAbortedException&) {
        setOffline(user);
    }
    catch (Poco::TimeoutException&) {
        setOffline(user);
    }
}
