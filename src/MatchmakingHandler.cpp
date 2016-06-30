#include "Router.hpp"
#include "User.hpp"
#include "Matchmaking.hpp"

#include "Poco/JSON/Template.h"
#include "Poco/JSON/Object.h"
#include "Poco/Net/HTTPResponse.h"

namespace {
    static void lobby(const RouteMatch& m) {
        auto params = m.captures();
        std::string accessToken = params.at("accessToken");
        User user(accessToken);
        auto ws = Poco::Net::WebSocket(m.request(), m.response());
        try {
            Matchmaking::CreateConnection(user, ws);
        }catch(WSConnectionException &e) {
            ws.close();
        }
    }

    static void getLobby(const RouteMatch& m) {
        Poco::JSON::Template tpl("views/matchmaking.html");
        tpl.parse();
        std::ostream& st = m.response().send();
        tpl.render(Poco::JSON::Object(), st);
        st.flush();
    }

    class Pages {
    public:
        Pages() {
            auto & router = Router::instance();
            router.registerRoute("/getLobby", getLobby);
            router.registerRoute("/api/lobby/{accessToken}", lobby);
        }
    };

    static Pages pages;
}
