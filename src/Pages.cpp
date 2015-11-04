#include "WebgameServer.hpp"
#include "Router.hpp"
#include "DBConnector.hpp"

#include "Poco/JSON/Template.h"
#include "Poco/JSON/Object.h"

static void root(const RouteMatch& m) {
    Poco::JSON::Template tpl("views/index.html");
    tpl.parse();
    std::ostream& st = m.response().send();
    tpl.render(Poco::JSON::Object(), st);
    st.flush();
}

static void http_example(const RouteMatch& m) {
    auto & s = m.response().send();
    s << "Hello world!";
    auto & db = DBConnection::instance();
    auto res = db.ExecParams("SELECT * FROM users", {});
    for (auto it = res.begin(); it != res.end(); ++it) {
        s << std::endl << (*it).field_by_name("name") << std::endl;
    }
}

#include "Poco/Net/WebSocket.h"
#include "Poco/Net/NetException.h"

static void websocket_example(const RouteMatch& m) {
    auto & app = WebgameServer::instance();

    try {
        Poco::Net::WebSocket ws(m.request(), m.response());
        app.logger().information("WebSocket connection established.");
        char buffer[1024];
        int flags;
        int n;

        do {
            n = ws.receiveFrame(buffer, sizeof(buffer), flags);
            app.logger().information(Poco::format("Frame received (length=%d, flags=0x%x).", n, unsigned(flags)));
            ws.sendFrame(buffer, n, flags);
        } while (n > 0 || (flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) != Poco::Net::WebSocket::FRAME_OP_CLOSE);

        app.logger().information("WebSocket connection closed.");
    }

    catch (Poco::Net::WebSocketException& exc) {
        app.logger().log(exc);

        switch (exc.code()) {
        case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_UNSUPPORTED_VERSION:
            m.response().set("Sec-WebSocket-Version", Poco::Net::WebSocket::WEBSOCKET_VERSION);
            // fallthrough
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

class Pages {
public:
    Pages() {
        auto & router = Router::instance();
        router.registerRoute("/", root);
        router.registerRoute("/hw", http_example);
        router.registerRoute("/ws", websocket_example);
    }
};

static Pages pages;
