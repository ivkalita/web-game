#ifndef MY_UTILS_HPP_INCLUDED
#define MY_UTILS_HPP_INCLUDED

#include <string>
#include <initializer_list>

namespace MyUtils {
    std::string SimpleJSON(std::initializer_list<std::string> values);
    std::string genRandomString(int len = 64);
}

#endif
