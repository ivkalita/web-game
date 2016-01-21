#include "MyUtils.hpp"

std::string MyUtils::SimpleJSON(std::initializer_list<std::string> values) {
    Poco::JSON::Object json;
    if (values.size() % 2 != 0)
        throw Poco::Exception("SimpleJSON: initializer list length must be even");

    for (auto i = values.begin(); i != values.end(); i += 2) {
        json.set(*i, *(i + 1));
    }
    std::stringstream o;
    json.stringify(o);
    return o.str();
}