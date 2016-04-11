#pragma once
#include <AbstractRouter.hpp>

#include "Poco/SingletonHolder.h"
#include "Poco/JSON/Object.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include <functional>

typedef std::function<Poco::JSON::Object(Poco::JSON::Object &, int & user)> WebsocketRouteHandler;

class WebsocketRoute : public AbstractRoute {
public:
    WebsocketRoute(const std::string& pattern, const WebsocketRouteHandler& handler, bool need_auth = true) :
        AbstractRoute(pattern), m_handler(handler), m_need_auth(need_auth) {}

    Poco::JSON::Object handle(Poco::JSON::Object & m, int & user) const { return m_handler(m, user); }
    bool isNeedAuth() { return m_need_auth; }
private:
    WebsocketRouteHandler m_handler;
    bool m_need_auth;
};

class WebsocketRouter : public AbstractRouter {
public:
    void registerRoute(const std::string& path, const WebsocketRouteHandler& handler, bool need_auth = true) {
        AbstractRouter::registerRoute("", new WebsocketRoute(path, handler, need_auth));
    }

    Poco::JSON::Object handle(Poco::JSON::Object message, int & user) const;

    static WebsocketRouter& instance() {
        static Poco::SingletonHolder<WebsocketRouter> sh;
        return *sh.get();
    }
};

class WebSocketRequestHandler : public Poco::Net::HTTPRequestHandler {
public:
    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
};

Poco::JSON::Object jsonDataResult(std::string result);
