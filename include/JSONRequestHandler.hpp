#ifndef JSON_REQUEST_HANDLER_HPP_INCLUDED
#define JSON_REQUEST_HANDLER_HPP_INCLUDED

#include <exception>
#include <string>
#include "Router.hpp"

#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Query.h"


class JsonException : public std::runtime_error {
public:
    JsonException(std::string message);
};


class JsonRequest {
    Poco::Dynamic::Var v;
    const RouteMatch& _m;
public:
    JsonRequest(const RouteMatch& m);

    template <typename T>
    void get(const std::string & name, T & value) const {
        Poco::JSON::Query q(v);
        try {
            q.find(name).convert(value);
        }
        catch (...) {
            throw JsonException("BadRequest");
        }
    }
};


class JsonResponse {
    const RouteMatch& _m;
    Poco::JSON::Object data;
public:
    JsonResponse(const RouteMatch& m);

    template <typename T>
    void set(const std::string & name, T & value) {
        data.set(name, value);
    }

    void send(const std::string result);
};


template <void(*T)(JsonRequest &, JsonResponse &)>
void CatchException(const RouteMatch& m) {
    JsonRequest rq(m);
    JsonResponse rs(m);
    try {
        T(rq, rs);
    }
    catch (const JsonException & e) {
        rs.send(e.what());
    }
    catch (const std::exception & e) {
        if (m.response().sent())
            throw e;
        rs.send("InternalError");
    }
}

#endif
