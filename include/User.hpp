#ifndef USER_HPP_INCLUDED
#define USER_HPP_INCLUDED

#include <string>

class User {
    int id;
    std::string name;
    User(std::string field, std::string value);
public:
    User(const std::string & accesToken) : User("token", accesToken) {}
    User(int userId) : User("id", std::to_string(userId)) {}
    int getId() { return id; }
    std::string getName() { return name; }
};

#endif
