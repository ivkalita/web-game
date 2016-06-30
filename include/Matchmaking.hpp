#pragma once
#include <iostream>
#include <String>
#include <vector>
#include <Map>
#include "DBConnector.hpp"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/ParseHandler.h"
#include "User.hpp"
#include "Poco/JSON/Stringifier.h"
#include "Poco/Net/WebSocket.h"
#include "WSConnection.hpp"
#include "CommonObjects.hpp"


class Matchmaking {
public:
    static Response getGames();
    static Response joinToGame(int gameId, int playerId);
    static Response createGame(Game game, int playerId);
    static Response startGame(int playerId);
    static Response leaveGame(int playerId, std::string* gameId = NULL, std::string* ownerId = NULL);
    static Response getLobbyInfo(int playerId);
    static Response HandleAction(std::string text);
    static Response onJoinGame(Poco::JSON::Array::Ptr params);
    static Response onCreateGame(Poco::JSON::Array::Ptr params);
    static Response onLeaveGame(Poco::JSON::Array::Ptr params);
    static Response OnLeaveGame(std::string userId, std::string gameId);
    static Response onGetLobbyInfo(Poco::JSON::Array::Ptr params);

    static std::vector<std::string> getPlayersList(int game_id);

    static void clearDB();
    static void onCloseConnection(int playerId);
    static std::string isInGame(int player_id);
    static void CreateConnection(User user, Poco::Net::WebSocket& ws);
};

static class Actions {
public:
    static enum ACTIONS {
        CREATE_GAME,
        GET_GAMES,
        LEAVE_GAME,
        JOIN_TO_GAME,
        START_GAME,
        GET_LOBBY_INFO
    };

    static const int getActionByName(const std::string name) {
        try {
            return actions.at(name);
        }
        catch (...) {
            return -1;
        }
    }

    static const std::string getActionText(ACTIONS id) {
        for (auto it = actions.begin(); it != actions.end(); ++it) {
            if (it->second == id) {
                return it->first;
            }
        }
    }

private:
    static const std::map<std::string, ACTIONS> actions;
};


static void MatchmakingActionHandler(std::string& action, std::ostringstream& stream) {
    Response r = Matchmaking::HandleAction(action);
    r.toJson().stringify(stream);
}

class MatchmakingException : std::exception {
private:
    Response::RESULT mId;
public:
    MatchmakingException(const  Response::RESULT id) :mId(id) {}
    Response::RESULT getResult() { return mId; }
    virtual const char* what() const throw() {
        return Response::getResponseText(mId).c_str();
    };
};
