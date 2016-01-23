#ifndef JSON_COMMAND_HANDLER_HPP_INCLUDED
#define JSON_COMMAND_HANDLER_HPP_INCLUDED

#include <string>
#include <functional>
#include <map>

#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/JSON.h"

#include "JSONRequestHandler.hpp"

typedef std::function<void(Poco::JSON::Object::Ptr, void*)> CommandHandler;

class JSONCommandHandler {
private:
    std::map<std::string, CommandHandler> handlers;
    Poco::JSON::Parser parser;

    std::string ErrorResponse();

public:
    void RegisterCommand(std::string command, CommandHandler handler);

    void Handle(std::string& json, void* extra_data);
};

#endif
