#include <AbstractRouter.hpp>
#include <unordered_set>
#include <Poco/String.h>
#include <Poco/StringTokenizer.h>
#include <Poco/Format.h>
#include <Poco/Exception.h>

using namespace std;
using namespace Poco;

struct PathTraverse
{
	PathTraverse(const StringList& segments, StringHashMap& captures) :
		curSegment(segments.begin()), endSegment(segments.end()), captures(captures)
	{
	}

	bool done() const { return curSegment == endSegment; }

	StringList::const_iterator curSegment;
	StringList::const_iterator endSegment;
	StringHashMap& captures;
};

AbstractRouter::AbstractRouter()
{
	m_routes.insert({ "", m_any });
	m_routes.insert({ "GET", m_get });
	m_routes.insert({ "POST", m_post });
}

void AbstractRouter::registerRoute(const std::string& method, AbstractRoute* route)
{
	auto group = m_routes.find(toUpper(trim(method)));
	if (group == m_routes.end())
		throw Exception(format("Unknown method `%s`", method));
	group->second.push_back(route);
}

void AbstractRouter::validate() const
{
	for (auto& r : m_routes)
		validate(r.second);
}

void AbstractRouter::validate(const RouteList& routeGroup) const
{
	for (auto i = 0; i < routeGroup.size(); i++) {
		for (auto j = i + 1; j < routeGroup.size(); j++) {
			auto a = routeGroup[i];
			auto b = routeGroup[j];
			if (ambiguous(a, b))
				throw Exception(format("Ambiguity between `%s` and `%s", a->toString(), b->toString()));
		}
	}
}

bool AbstractRouter::ambiguous(const AbstractRoute* a, const AbstractRoute* b)
{
	auto minLength = min(a->segments().size(), b->segments().size());
	for (auto i = 0; i < minLength; i++) {
		auto s1 = a->segments()[i];
		auto s2 = b->segments()[i];

		if (RoutePathSegment::isCatchAll(s1) || RoutePathSegment::isCatchAll(s2))
			return true;

		if (s1->isLiteral() && s2->isLiteral()) {
			auto l1 = static_cast<RouteLiteralPathSegment*>(s1);
			auto l2 = static_cast<RouteLiteralPathSegment*>(s2);
			if (l1->value() != l2->value())
				return false;
		}
	}

	if (a->segments().size() == b->segments().size())
		return true;

	auto diff = a->segments().size() > minLength ?
		a->segments()[minLength] :
		b->segments()[minLength];

	return RoutePathSegment::isCatchAll(diff);
}

AbstractRoute* AbstractRouter::match(const string& method, URI uri, StringHashMap& captures) const
{
	auto routeGroup = m_routes.find(toUpper(method));
	if (routeGroup != m_routes.end()) {
		auto r = match(routeGroup->second, uri, captures);
		if (r)
			return r;
	}
	return match(m_any, uri, captures);
}

const AbstractRouter::RouteList& AbstractRouter::routes(const std::string& method) const
{
	return m_routes.at(toUpper(trim(method)));
}

string AbstractRoute::toString() const
{
	string res("/");
	auto segment = m_segments.begin();
	if (segment != m_segments.end()) {
		res += (*segment)->toString();
		while (++segment != m_segments.end()) {
			res += "/";
			res += (*segment)->toString();
		}
	}
	return res;
}

AbstractRoute* AbstractRouter::match(const RouteList& routeGroup, URI& uri, StringHashMap& captures) const
{
	StringList segments;
	uri.getPathSegments(segments);
	for (auto r : routeGroup) {
		captures.clear();
		if (r->match(segments, captures))
			return r;
	}
	return nullptr;
}

AbstractRouter::~AbstractRouter()
{
	for (auto& routeGroup : m_routes)
		for (auto route : routeGroup.second)
			delete route;
}

AbstractRoute::~AbstractRoute()
{
	for (auto seg : m_segments)
		delete seg;
}

void AbstractRoute::parse(const string& pattern)
{
	auto foundCatchAll = false;
	StringTokenizer segments(pattern, "/", StringTokenizer::TOK_IGNORE_EMPTY);
	for (auto& s : segments) {
		if (foundCatchAll)
			throw Exception("Parameter which catches all should be at the end");

		if (!s.empty() && s.front() == '{') {
			if (s.back() != '}')
				throw Exception("Missing `}`");

			auto name = trim(s.substr(1, s.length() - 2));
			if (!name.empty() && name.front() == '*') {
				name = trim(name.substr(1));
				foundCatchAll = true;
			}
			m_segments.push_back(new RouteParameterPathSegment(name, foundCatchAll));
		} else {
			m_segments.push_back(new RouteLiteralPathSegment(s));
		}
	}
}

bool AbstractRoute::match(const StringList& segments, StringHashMap& captures) const
{
	PathTraverse traverse(segments, captures);
	for (auto parsedSegment : m_segments) {
		if (!parsedSegment->match(traverse))
			return false;
	}
	return traverse.done();
}

bool RoutePathSegment::isCatchAll(const RoutePathSegment* s)
{
	return !s->isLiteral() && static_cast<const RouteParameterPathSegment*>(s)->isCatchAll();
}

bool RouteLiteralPathSegment::match(PathTraverse& t) const
{
	if (t.curSegment != t.endSegment && m_value == *t.curSegment) {
		t.curSegment++;
		return true;
	}
	return false;
}

string RouteLiteralPathSegment::toString() const
{
	return m_value;
}

bool RouteParameterPathSegment::match(PathTraverse& t) const
{
	auto shouldCapture = !m_name.empty();

	if (t.curSegment == t.endSegment) {
		if (m_isCatchAll && shouldCapture)
			t.captures[m_name] = "";
		return m_isCatchAll;
	}

	if (m_isCatchAll) {
		if (shouldCapture) {
			auto capture = *t.curSegment;
			while (++t.curSegment != t.endSegment) {
				capture += '/';
				capture += *t.curSegment;
			}
			t.captures[m_name] = capture;
		}
		t.curSegment = t.endSegment;
		return true;
	}

	if (shouldCapture) {
		auto oldCapture = t.captures.find(m_name);
		if (oldCapture == t.captures.end())
			t.captures[m_name] = *t.curSegment;
		else if (oldCapture->second != *t.curSegment)
			return false;
	}

	t.curSegment++;
	return true;
}

string RouteParameterPathSegment::toString() const
{
	string res(m_name);
	if (m_isCatchAll)
		res = '*' + res;
	res = '{' + res + '}';
	return res;
}