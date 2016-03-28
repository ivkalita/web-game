#include "Router.hpp"
#include "User.hpp"
#include "Matchmaking.hpp"

#include "Poco/JSON/Template.h"
#include "Poco/JSON/Object.h"

namespace {
    static void lobby(const RouteMatch& m) {
        auto params = m.captures();
        std::string accessToken = params.at("accessToken");
        User user(accessToken);
        Matchmaking::CreateConnection(user, Poco::Net::WebSocket(m.request(), m.response()));
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
