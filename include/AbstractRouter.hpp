#ifndef ROUTER_H
#define ROUTER_H

#include <string>
#include <vector>
#include <map>
#include <Poco/URI.h>

typedef std::vector<std::string> StringList;
typedef std::map<std::string, std::string> StringHashMap;

struct PathTraverse;

class RoutePathSegment
{
protected:
	RoutePathSegment(bool isLiteral) : m_isLiteral(isLiteral) { }

public:
	virtual bool match(PathTraverse& t) const = 0 { }

	virtual std::string toString() const = 0 { }

	bool isLiteral() const { return m_isLiteral; }

	static bool isCatchAll(const RoutePathSegment* s);

private:
	bool m_isLiteral;
};

class RouteLiteralPathSegment : public RoutePathSegment
{
public:
	explicit RouteLiteralPathSegment(const std::string& value) :
		RoutePathSegment(true), m_value(value) { }

	bool match(PathTraverse& t) const override;

	std::string toString() const override;

	const std::string& value() const { return m_value; }

private:
	std::string m_value;
};

class RouteParameterPathSegment : public RoutePathSegment
{
public:
	RouteParameterPathSegment(const std::string& name, bool isCatchAll) :
		RoutePathSegment(false), m_name(name), m_isCatchAll(isCatchAll)
	{
	}

	bool match(PathTraverse& t) const override;

	std::string toString() const override;

	bool isCatchAll() const { return m_isCatchAll; }

	const std::string& name() const { return m_name; }

private:
	bool m_isCatchAll;
	std::string m_name;
};

class AbstractRoute
{
public:
	typedef std::vector<RoutePathSegment*> PathSegmentList;

	AbstractRoute(const std::string& pattern) { parse(pattern); }

	virtual ~AbstractRoute();

	void parse(const std::string& pattern);

	bool match(const StringList& segments, StringHashMap& captures) const;

	std::string toString() const;

	const PathSegmentList& segments() const { return m_segments; }

private:
	PathSegmentList m_segments;
};

class AbstractRouter
{
public:
	typedef std::vector<AbstractRoute*> RouteList;

	virtual ~AbstractRouter();

	const RouteList& routes(const std::string& method) const;

	void validate() const;

protected:
	AbstractRouter();

	void registerRoute(const std::string& method, AbstractRoute* route);

	AbstractRoute* match(const std::string& method, Poco::URI uri, StringHashMap& captures) const;

private:
	AbstractRoute* match(const RouteList& routeGroup, Poco::URI& uri, StringHashMap& captures) const;

	void validate(const RouteList& routeGroup) const;

	static bool ambiguous(const AbstractRoute* a, const AbstractRoute* b);

	RouteList m_any;
	RouteList m_get;
	RouteList m_post;

	std::map<std::string, RouteList&> m_routes;
};

#endif