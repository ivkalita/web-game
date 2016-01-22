#include "MyUtils.hpp"

#include <iomanip>
#include "Poco/Random.h"

#include "Poco/JSON/JSON.h"
#include "Poco/JSON/Object.h"

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

static Poco::Random rnd;

std::string MyUtils::genRandomString(int len) {
    std::stringstream res;
    for (int i = 0; i < (int)ceil(len / 8); i++)
        res << std::setfill('0') << std::setw(8) << std::hex << rnd.next();
    return res.str().substr(0, len);
}