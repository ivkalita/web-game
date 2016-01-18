#ifndef MY_UTILS_HPP_DEFINED
#define MY_UTILS_HPP_DEFINED

#include <string>
#include <initializer_list>

#include "Poco/JSON/JSON.h"
#include "Poco/JSON/Object.h"

namespace MyUtils {
    std::string SimpleJSON(std::initializer_list<std::string> values);
}

#endif
