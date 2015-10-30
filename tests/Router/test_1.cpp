#include <AbstractRouter.hpp>
#include <gtest/gtest.h>
#include <vector>
#include <functional>
#include <memory>
#include <Poco/Exception.h>

using namespace std;
using namespace Poco;

class TestRouteMatch
{
public:
	TestRouteMatch(const string& url) : m_uri(url) { }

	URI uri() const { return m_uri; }

	StringHashMap& captures() { return m_captures; }
	const StringHashMap& captures() const { return m_captures; }

private:
	URI m_uri;
	StringHashMap m_captures;
};

typedef std::function<void(const TestRouteMatch&)> TestRouteHandler;

class TestRoute : public AbstractRoute
{
public:
	TestRoute(const string& pattern, const TestRouteHandler& handler) :
		AbstractRoute(pattern), m_handler(handler)
	{
	}

	void handle(const TestRouteMatch& m) const { m_handler(m); }

private:
	TestRouteHandler m_handler;
};

class TestRouter : public AbstractRouter
{
public:
	void registerRoute(const std::string& path, const TestRouteHandler& handler)
	{
		registerRoute("", path, handler);
	}

	void registerRoute(const std::string& method, const std::string& path, const TestRouteHandler& handler)
	{
		AbstractRouter::registerRoute(method, new TestRoute(path, handler));
	}

	bool handle(const string& method, const string& url) const
	{
		TestRouteMatch m(url);
		auto route = static_cast<TestRoute*>(match(method, m.uri(), m.captures()));
		if (!route)
			return false;
		route->handle(m);
		return true;
	}
};

string buildRoutePath(const vector<shared_ptr<RoutePathSegment>>& segments)
{
	string res("/");
	auto segment = segments.begin();
	if (segment != segments.end()) {
		res += (*segment)->toString();
		while (++segment != segments.end()) {
			res += "/";
			res += (*segment)->toString();
		}
	}
	return res;
}

bool equals(const RoutePathSegment* a, const RoutePathSegment* b)
{
	if (a->isLiteral() != b->isLiteral())
		return false;
	if (a->isLiteral()) {
		auto l1 = static_cast<const RouteLiteralPathSegment*>(a);
		auto l2 = static_cast<const RouteLiteralPathSegment*>(b);
		return l1->value() == l2->value();
	}
	auto p1 = static_cast<const RouteParameterPathSegment*>(a);
	auto p2 = static_cast<const RouteParameterPathSegment*>(b);
	return p1->name() == p2->name() && p1->isCatchAll() == p2->isCatchAll();
}

void testRegisterRoute(const string& method, const vector<shared_ptr<RoutePathSegment>>& pathSegments)
{
	TestRouter router;
	router.registerRoute(method, buildRoutePath(pathSegments), [](const TestRouteMatch& m) { });
	auto route = router.routes(method).front();
	ASSERT_EQ(route->segments().size(), pathSegments.size());
	for (auto i = 0; i < pathSegments.size(); i++) {
		ASSERT_TRUE(equals(route->segments()[i], pathSegments[i].get()));
	}
}

TEST(Router, registerRoute)
{
	//Test empty path ("/")
	testRegisterRoute("GET", { });
	testRegisterRoute("POST", { });
	testRegisterRoute("", { });

	testRegisterRoute("GET", {
		make_shared<RouteLiteralPathSegment>("literal_1"),
	});
	testRegisterRoute("POST", {
		make_shared<RouteLiteralPathSegment>("literal_1"),
		make_shared<RouteLiteralPathSegment>("literal_2")
	});
	testRegisterRoute("GET", {
		make_shared<RouteLiteralPathSegment>("literal_1"),
		make_shared<RouteLiteralPathSegment>("literal_2"),
		make_shared<RouteLiteralPathSegment>("literal_3")
	});
	testRegisterRoute("POST", {
		make_shared<RouteLiteralPathSegment>("literal_1"),
		make_shared<RouteParameterPathSegment>("param_1", false),
		make_shared<RouteLiteralPathSegment>("literal_2"),
		make_shared<RouteParameterPathSegment>("param_2", false),
	});
	testRegisterRoute("get", {
		make_shared<RouteParameterPathSegment>("param_1", false),
		make_shared<RouteLiteralPathSegment>("literal_1"),
		make_shared<RouteParameterPathSegment>("param_2", false),
		make_shared<RouteLiteralPathSegment>("literal_2"),
	});
	testRegisterRoute("post", {
		make_shared<RouteParameterPathSegment>("param_1", false),
		make_shared<RouteLiteralPathSegment>("literal_1"),
		make_shared<RouteParameterPathSegment>("param_2", false),
		make_shared<RouteLiteralPathSegment>("literal_2"),
		make_shared<RouteParameterPathSegment>("param_catch_all", true)			// Catch all
	});
	testRegisterRoute("get", {
		make_shared<RouteLiteralPathSegment>("literal_1"),
		make_shared<RouteParameterPathSegment>("param_1", false),
		make_shared<RouteLiteralPathSegment>("literal_2"),
		make_shared<RouteParameterPathSegment>("param_2", false),
		make_shared<RouteParameterPathSegment>("param_catch_all", true)			// Catch all
	});


	//Invalid method
	ASSERT_THROW(testRegisterRoute("xxx", { }), Exception);

	//Path segment after parameter which catches all
	ASSERT_THROW(testRegisterRoute("GET", {
		make_shared<RouteParameterPathSegment>("param_catch_all", true),
		make_shared<RouteLiteralPathSegment>("literal_after_catch_all")
	}), Exception);

	ASSERT_THROW(testRegisterRoute("GET", {
		make_shared<RouteParameterPathSegment>("param_catch_all", true),
		make_shared<RouteParameterPathSegment>("param_after_catch_all", false)
	}), Exception);

	ASSERT_THROW(testRegisterRoute("GET", {
		make_shared<RouteParameterPathSegment>("param_catch_all_1", true),
		make_shared<RouteParameterPathSegment>("param_catch_all_2", true)
	}), Exception);
}

TEST(Router, validate)
{
	{
		TestRouter r;
		r.registerRoute("/", TestRouteHandler());
		r.registerRoute("/", TestRouteHandler());
		ASSERT_THROW(r.validate(), Exception);
	}
	{
		TestRouter r;
		r.registerRoute("/literal_1", TestRouteHandler());
		r.registerRoute("/literal_1", TestRouteHandler());
		ASSERT_THROW(r.validate(), Exception);
	}
	{
		TestRouter r;
		r.registerRoute("/literal_1", TestRouteHandler());
		r.registerRoute("/literal_1/{*}", TestRouteHandler());
		ASSERT_THROW(r.validate(), Exception);
	}
	{
		TestRouter r;
		r.registerRoute("/literal_1/literal_2", TestRouteHandler());
		r.registerRoute("/literal_1/literal_2", TestRouteHandler());
		ASSERT_THROW(r.validate(), Exception);
	}
	{
		TestRouter r;
		r.registerRoute("/literal_1/{parameter_1}", TestRouteHandler());
		r.registerRoute("/literal_1/{parameter_1}", TestRouteHandler());
		ASSERT_THROW(r.validate(), Exception);
	}
	{
		TestRouter r;
		r.registerRoute("/literal_1/{parameter_1}/literal_2", TestRouteHandler());
		r.registerRoute("/literal_1/{parameter_1}/literal_2/{*}", TestRouteHandler());
		ASSERT_THROW(r.validate(), Exception);
	}
	{
		TestRouter r;
		r.registerRoute("/literal_1/{parameter_1}/literal_2/{*}", TestRouteHandler());
		r.registerRoute("/literal_1/{parameter_1}/literal_2/{*}", TestRouteHandler());
		ASSERT_THROW(r.validate(), Exception);
	}
	{
		TestRouter r;
		r.registerRoute("/literal_1/{*}", TestRouteHandler());
		r.registerRoute("/literal_1/{parameter_1}/literal_2/{*}", TestRouteHandler());
		ASSERT_THROW(r.validate(), Exception);
	}
	{
		TestRouter r;
		r.registerRoute("/{*}", TestRouteHandler());
		r.registerRoute("/literal_1/{parameter_1}/literal_2/{*}", TestRouteHandler());
		ASSERT_THROW(r.validate(), Exception);
	}
	{
		TestRouter r;
		r.registerRoute("/{parameter_1}/{parameter_2}", TestRouteHandler());
		r.registerRoute("/literal_1/literal_2", TestRouteHandler());
		ASSERT_THROW(r.validate(), Exception);
	}
	{
		TestRouter r;
		r.registerRoute("/literal_1/{parameter_1}", TestRouteHandler());
		r.registerRoute("/literal_2/{parameter_2}", TestRouteHandler());
		r.registerRoute("/{*}", TestRouteHandler());
		ASSERT_THROW(r.validate(), Exception);
	}
	{
		TestRouter r;
		r.registerRoute("/literal_1/{parameter_1}", TestRouteHandler());
		r.registerRoute("/literal_2/{parameter_2}", TestRouteHandler());
		r.registerRoute("/literal_1/{parameter_2}", TestRouteHandler());
		ASSERT_THROW(r.validate(), Exception);
	}
	{
		TestRouter r;
		r.registerRoute("/literal/{parameter}", TestRouteHandler());
		r.registerRoute("/literal/{*parameter_catches_all}", TestRouteHandler());
		ASSERT_THROW(r.validate(), Exception);
	}
	{
		TestRouter r;
		ASSERT_NO_THROW(r.validate());
	}
	{
		TestRouter r;
		r.registerRoute("/json", TestRouteHandler());
		r.registerRoute("GET", "/json", TestRouteHandler());
		r.registerRoute("POST", "/json", TestRouteHandler());
		ASSERT_NO_THROW(r.validate());
	}
	{
		TestRouter r;
		r.registerRoute("/literal_1", TestRouteHandler());
		r.registerRoute("/literal_2", TestRouteHandler());
		ASSERT_NO_THROW(r.validate());
	}
	{
		TestRouter r;
		r.registerRoute("/literal_1/{parameter_1}", TestRouteHandler());
		r.registerRoute("/literal_2/{parameter_1}/literal_3", TestRouteHandler());
		ASSERT_NO_THROW(r.validate());
	}
	{
		TestRouter r;
		r.registerRoute("/literal_1/{parameter_1}/literal_2", TestRouteHandler());
		r.registerRoute("/literal_1/{parameter_1}/literal_3", TestRouteHandler());
		ASSERT_NO_THROW(r.validate());
	}
	{
		TestRouter r;
		r.registerRoute("/{parameter_1}/{parameter_2}", TestRouteHandler());
		r.registerRoute("/{parameter_1}/{parameter_2}/{parameter_3}", TestRouteHandler());
		ASSERT_NO_THROW(r.validate());
	}
	{
		TestRouter r;
		r.registerRoute("/literal_1/{parameter_1}/literal_2/{*}", TestRouteHandler());
		r.registerRoute("/literal_1/{parameter_1}/literal_3/{*}", TestRouteHandler());
		r.registerRoute("/literal_4/{parameter_1}/literal_5/{*}", TestRouteHandler());
		r.registerRoute("/literal_5/", TestRouteHandler());
		r.registerRoute("/{parameter_1}/{parameter_2}/literal_6/", TestRouteHandler());
		ASSERT_NO_THROW(r.validate());
	}
}

TEST(Router, handle)
{
	TestRouteHandler failHandler = [](const TestRouteMatch&) { FAIL(); };

	TestRouteHandler emptyHandler = [](const TestRouteMatch& m) {
		ASSERT_EQ(m.captures(), StringHashMap());
	};

	{
		TestRouter r;
		r.registerRoute("/", emptyHandler);
		r.validate();
		ASSERT_TRUE(r.handle("GET", "http://www.test.com/"));
	}

	{
		TestRouter r;
		r.registerRoute("/", failHandler);
		r.registerRoute("POST", "/", emptyHandler);
		r.registerRoute("GET", "/", failHandler);
		r.validate();
		ASSERT_TRUE(r.handle("POST", "http://www.test.com"));
	}

	{
		TestRouter r;
		r.registerRoute("/images/{*image_relative_file_name}", failHandler);
		r.registerRoute("POST", "/images/{*image_relative_file_name}", failHandler);
		r.registerRoute("GET", "/images/{*image_relative_file_name}", [](const TestRouteMatch& m) {
			ASSERT_EQ(m.captures(), StringHashMap({
				{ "image_relative_file_name", "buttons/close.png" }
			}));
		});
		r.validate();
		ASSERT_TRUE(r.handle("GET", "http://www.test.com/images/buttons/close.png"));
	}

	{
		TestRouter r;
		r.registerRoute("/", failHandler);
		r.registerRoute("/literal_1", emptyHandler);
		r.validate();
		ASSERT_TRUE(r.handle("GET", "http://www.test.com/literal_1"));
	}

	{
		TestRouter r;
		r.registerRoute("/", failHandler);
		r.registerRoute("/literal_1", failHandler);
		r.registerRoute("/literal_1/{parameter_1}", [](const TestRouteMatch& m) {
			ASSERT_EQ(m.captures(), StringHashMap({
				{ "parameter_1", "test_parameter" }
			}));
		});
		r.validate();
		ASSERT_TRUE(r.handle("GET", "http://www.test.com/literal_1/test_parameter"));
	}

	{
		TestRouter r;
		r.registerRoute("/", failHandler);
		r.registerRoute("/literal_1", failHandler);
		r.registerRoute("/literal_1/{parameter_1}/literal_2/{parameter_2}", [](const TestRouteMatch& m) {
			ASSERT_EQ(m.captures(), StringHashMap({
				{ "parameter_1", "test_parameter_1" },
				{ "parameter_2", "test_parameter_2" }
			}));
		});
		r.validate();
		ASSERT_TRUE(r.handle("GET", "http://www.test.com/literal_1/test_parameter_1/literal_2/test_parameter_2"));
	}

	{
		TestRouter r;
		r.registerRoute("/", failHandler);
		r.registerRoute("/literal_1", failHandler);
		r.registerRoute("/literal_1/{parameter_1}/{parameter_2}", [](const TestRouteMatch& m) {
			ASSERT_EQ(m.captures(), StringHashMap({
				{ "parameter_1", "test_parameter_1" },
				{ "parameter_2", "test_parameter_2" }
			}));
		});
		r.validate();
		ASSERT_TRUE(r.handle("GET", "http://www.test.com/literal_1/test_parameter_1/test_parameter_2"));
	}

	{
		TestRouter r;
		r.registerRoute("/", failHandler);
		r.registerRoute("/literal_1", failHandler);
		r.registerRoute("/{parameter_1}/literal_1/{*parameter_catches_all}", [](const TestRouteMatch& m) {
			ASSERT_EQ(m.captures(), StringHashMap({
				{ "parameter_1", "test_parameter_1" },
				{ "parameter_catches_all", "catched_1/catched_2/catched_3" }
			}));
		});
		r.validate();
		ASSERT_TRUE(r.handle("GET", "http://www.test.com/test_parameter_1/literal_1/catched_1/catched_2/catched_3"));
	}

	{
		TestRouter r;
		r.registerRoute("/", failHandler);
		r.registerRoute("/literal_1", failHandler);
		r.registerRoute("/{parameter_1}/literal_2", failHandler);
		r.registerRoute("/{parameter_1}/literal_1/{*parameter_catches_all}", [](const TestRouteMatch& m) {
			ASSERT_EQ(m.captures(), StringHashMap({
				{ "parameter_1", "test_parameter_1" },
				{ "parameter_catches_all", "" }
			}));
		});
		r.validate();
		ASSERT_TRUE(r.handle("GET", "http://www.test.com/test_parameter_1/literal_1"));
	}

	{
		TestRouter r;
		r.validate();
		ASSERT_FALSE(r.handle("GET", "http://www.test.com/literal_1/literal_2"));
	}

	{
		TestRouter r;
		r.registerRoute("/", failHandler);
		r.registerRoute("/literal_1", failHandler);
		r.registerRoute("/literal_1/literal_2/literal_3", failHandler);
		r.validate();
		ASSERT_FALSE(r.handle("GET", "http://www.test.com/literal_1/literal_2"));
	}

	{
		TestRouter r;
		r.registerRoute("/", failHandler);
		r.registerRoute("/literal_1", failHandler);
		r.registerRoute("/literal_1/{parameter_1}/literal_3", failHandler);
		r.validate();
		ASSERT_FALSE(r.handle("GET", "http://www.test.com/literal_1/literal_2"));
	}

	{
		TestRouter r;
		r.registerRoute("/", failHandler);
		r.registerRoute("/literal_1", failHandler);
		r.registerRoute("/{parameter_1}/literal_2", failHandler);
		r.validate();
		ASSERT_FALSE(r.handle("GET", "http://www.test.com/json"));
	}

	{
		TestRouter r;
		r.registerRoute("/", failHandler);
		r.registerRoute("POST", "/json", failHandler);
		r.validate();
		ASSERT_FALSE(r.handle("GET", "http://www.test.com/json"));
	}
}