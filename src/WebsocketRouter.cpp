#include "WebsocketRouter.hpp"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/NetException.h"
#include "Poco/JSON/Parser.h"
#include "Poco/URI.h"
#include <sstream>

#include "Router.hpp"

Poco::JSON::Object WebsocketRouter::handle(Poco::JSON::Object message, int & user) const {
    std::string action = message.get("action").extract<std::string>();
    auto route = static_cast<WebsocketRoute*>(match("", Poco::URI(action), std::map<std::string, std::string>()));
    if (!route) {
        Poco::JSON::Object r;
        r.set("result", "notFound");
        return r;
    }
    if (route->isNeedAuth() && user == -1) {
        Poco::JSON::Object r;
        r.set("result", "needAuth");
        return r;
    }
    message = message.get("params").extract<Poco::JSON::Object>();
    return route->handle(message, user);
}

namespace {
    Poco::JSON::Object process(char buffer[1024], int & user) {
        Poco::JSON::Object req, res;
        try {
            req = Poco::JSON::Parser().parse(buffer).extract<Poco::JSON::Object>();
        }
        catch (...) {
            res.set("result", "badRequest");
            return res;
        }
        try {
            res = WebsocketRouter::instance().handle(req, user);
        }
        catch (...) {
            res.set("result", "internalError");
            return res;
        }
    }

    void handle(const RouteMatch& m) {
        try {
            Poco::Net::WebSocket ws(m.request(), m.response());
            char buffer[1024];
            int flags, n, user = -1;

            do {
                n = ws.receiveFrame(buffer, sizeof(buffer), flags);
                Poco::JSON::Object res = process(buffer, user);
                if (res.size() > 0) {
                    std::ostringstream st;
                    res.stringify(st);
                    std::string s = st.str();
                    ws.sendFrame(s.c_str(), s.length(), flags);
                }
            } while (n > 0 || (flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) != Poco::Net::WebSocket::FRAME_OP_CLOSE);
        }
        catch (Poco::Net::WebSocketException& exc) {
            switch (exc.code()) {
            case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_UNSUPPORTED_VERSION:
                m.response().set("Sec-WebSocket-Version", Poco::Net::WebSocket::WEBSOCKET_VERSION);
            case Poco::Net::WebSocket::WS_ERR_NO_HANDSHAKE:
            case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_NO_VERSION:
            case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_NO_KEY:
                m.response().setStatusAndReason(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
                m.response().setContentLength(0);
                m.response().send();
                break;
            }
        }
    }

    struct Pages {
        Pages() {
            Router::instance().registerRoute("/api/ws", handle);
        }
    };

    Pages pages;
}
