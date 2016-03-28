#include "CommonObjects.hpp"
#include "Matchmaking.hpp"
#include "WSConnection.hpp"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/ParseHandler.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/Stringifier.h"

using namespace std;
using namespace Poco::JSON;

const map<string, Actions::ACTIONS> Actions::actions = {
    { "CreateGame",    CREATE_GAME    },
    { "GetGames",      GET_GAMES      },
    { "LeaveGame",     LEAVE_GAME     },
    { "JoinToGame",    JOIN_TO_GAME   },
    { "StartGame",     START_GAME     },
    { "GetLobbyInfo",  GET_LOBBY_INFO }
};
