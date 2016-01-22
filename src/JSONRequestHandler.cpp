#include "JSONRequestHandler.hpp"

#include "Poco/Net/HTMLForm.h"
#include "Poco/JSON/Parser.h"

JsonException::JsonException(std::string message) : runtime_error(message) {}

JsonRequest::JsonRequest(const RouteMatch& m) : _m(m) {
    Poco::Net::HTMLForm form(m.request(), m.request().stream());
    Poco::JSON::Parser p;
    v = p.parse(form.get("jsonObj"));
}

JsonResponse::JsonResponse(const RouteMatch& m) : _m(m) {}

void JsonResponse::send(const std::string result) {
    Poco::JSON::Object response;
    response.set("result", result);
    response.set("data", (data.size() != 0) ? data : "null");
    std::ostream& st = _m.response().send();
    Poco::JSON::Stringifier::condense(response, st);
    st.flush();
}
