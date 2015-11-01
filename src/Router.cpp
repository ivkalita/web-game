#include <Router.hpp>

using namespace std;
using namespace Poco::Net;

void Router::registerRoute(const string& path, const RouteHandler& handler)
{
	registerRoute("", path, handler);
}

void Router::registerRoute(const string& method, const string& path, const RouteHandler& handler)
{
	AbstractRouter::registerRoute(method, new Route(path, handler));
}

bool Router::handle(HTTPServerRequest& request, HTTPServerResponse& response) const
{
	RouteMatch m(request, response);
	auto route = static_cast<Route*>(match(request.getMethod(), m.uri(), m.captures()));
	if (!route)
		return false;
	route->handle(m);
	return true;
}