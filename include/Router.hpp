#ifndef ROUTER_H
#define ROUTER_H

#include <AbstractRouter.hpp>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include "Poco/SingletonHolder.h"
#include <functional>

class RouteMatch
{
public:
	RouteMatch(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
		: m_request(request), m_response(response), m_uri(request.getURI())
	{
	}

	Poco::Net::HTTPServerRequest& request() const { return m_request; }
	Poco::Net::HTTPServerResponse& response() const { return m_response; }

	Poco::URI uri() const { return m_uri; }

	StringHashMap& captures() { return m_captures; }
	const StringHashMap& captures() const { return m_captures; }

private:
	Poco::Net::HTTPServerRequest& m_request;
	Poco::Net::HTTPServerResponse& m_response;
	Poco::URI m_uri;
	StringHashMap m_captures;
};

typedef std::function<void(const RouteMatch&)> RouteHandler;

class Route : public AbstractRoute
{
public:
	Route(const std::string& pattern, const RouteHandler& handler) :
		AbstractRoute(pattern), m_handler(handler)
	{
	}

	void handle(const RouteMatch& m) const { m_handler(m); }

private:
	RouteHandler m_handler;
};

class Router : public AbstractRouter
{
public:
	void registerRoute(const std::string& path, const RouteHandler& handler);
	void registerRoute(const std::string& method, const std::string& path, const RouteHandler& handler);

	bool handle(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) const;

	static Router& instance() {
		static Poco::SingletonHolder<Router> sh;
		return *sh.get();
	}
};

#endif