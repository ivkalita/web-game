#include "JSONCommandHandler.hpp"

#include "WebgameServer.hpp"
#include "Poco/LogStream.h"

#include "Poco/Dynamic/Var.h"

std::string JSONCommandHandler::ErrorResponse() {
    return "{\"result\":\"JSONError\",\"data\":{\"message\":\"Json syntax error\"}}";
}

void JSONCommandHandler::RegisterCommand(std::string command, CommandHandler handler) {
    handlers[command] = handler;
}

void JSONCommandHandler::Handle(std::string& json, void* extra_data) {
    Poco::LogStream logger(WebgameServer::instance().logger());
    try {
        parser.reset();
        auto json_obj = parser.parse(json);
        Poco::JSON::Object::Ptr p = json_obj.extract<Poco::JSON::Object::Ptr>();
        std::string command = p->getValue<std::string>("command");
        Poco::JSON::Object::Ptr data = p->getObject("data");
        if (handlers.find(command) != handlers.end()) {
            handlers[command](data, extra_data);
        }
    }
    catch (const Poco::JSON::JSONException& e) {
        logger << e.displayText() << std::endl;
        throw JsonException(ErrorResponse());
    }
    catch (const Poco::InvalidAccessException& e) {
        logger << e.displayText();
        throw JsonException(ErrorResponse());
    }
    catch (const Poco::Exception& e) {
        logger << e.displayText() << std::endl;
        throw JsonException(ErrorResponse());
    }
}