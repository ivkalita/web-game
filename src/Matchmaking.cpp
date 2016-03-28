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


void Matchmaking::CreateConnection(User user, Poco::Net::WebSocket& ws) {
    ConnectionsPoll::instance().addThread(
        user.getId(),
        ws,
        Matchmaking::onCloseConnection,
        MatchmakingActionHandler
        );
}

Response Matchmaking::getGames() {
    Response response = Response();
    GameInfoData *gameInfoData = new GameInfoData();
    auto res = DBConnection::instance().ExecParams("SELECT * FROM GAMES JOIN USERS ON GAMES.owner_id=USERS.id", {});
    for (auto game = res.begin(); game != res.end(); ++game) {
        string host = (*game).field_by_name("owner_id");
        GameInfo gameInfo = GameInfo(
                res.field_by_name(0, "name"),
                stoi(res.field_by_name(0, "maxNumPlayers")),
                Mode(),
                Map(),
                stoi((*game).field_by_name("id")),
                UserInfo(stoi(host), (*game).field_by_name("login")),
                stoi((*game).field_by_name("curNumPlayers"))
            );
        gameInfoData->addGame(gameInfo);
    }
    response.setData(gameInfoData);
    response.setResult(Response::OK);
    response.setAction(Actions::getActionText(Actions::GET_GAMES));
    return response;
}

Response Matchmaking::joinToGame(int gameId, int playerId) {
    Response response = Response();
    response.setData(NULL);
    int host, curNumPlayers, maxNumPlayers;

    auto res = DBConnection::instance().ExecParams("SELECT * FROM GAMES WHERE id=$1", { to_string(gameId) });
    if (res.row_count() == 0) {
        throw MatchmakingException(Response::NOT_FOUND);
    }
    curNumPlayers = stoi(res.field_by_name(0, "curnumplayers"));
    maxNumPlayers = stoi(res.field_by_name(0, "maxnumplayers"));

    host = playerId;
    if (curNumPlayers == maxNumPlayers) {
        response.setResult(Response::LOBBY_IS_FULL);
        return response;
    }
    if (DBConnection::instance().ExecParams("SELECT * FROM CONNECTIONS WHERE game_id=$1 and player_id=$2", { to_string(gameId), to_string(host) }).row_count() != 0)
        throw MatchmakingException(Response::ALREADY_CONNECTED);

    DBConnection::instance().ExecParams("INSERT INTO connections (game_id, player_id) VALUES($1, $2)", { to_string(gameId), to_string(host) });
    curNumPlayers++;
    DBConnection::instance().ExecParams("UPDATE games SET curnumplayers = curnumplayers + 1 WHERE id=$1", { to_string(gameId) });
    response.setAction(Actions::getActionText(Actions::JOIN_TO_GAME));
    response.setResult(Response::OK);
    return response;
}

Response Matchmaking::createGame(Game game, int playerId) {
    Response response = Response();
    string id;
    int host = playerId;
    auto res = DBConnection::instance().ExecParams("INSERT INTO GAMES (name, owner_id, maxnumplayers, curNumPlayers) values ($1, $2, $3, $4) RETURNING id", { game.getName(),  std::to_string(host), std::to_string(game.getMaxNumPlayers()), "1" });
    id = res.field_by_name(0, "id");
    DBConnection::instance().ExecParams("INSERT INTO connections(game_id, player_id) VALUES($1, $2)", { id, to_string(host) });
    response.setAction(Actions::getActionText(Actions::CREATE_GAME));
    response.setResult(Response::OK);
    return response;
}

Response Matchmaking::startGame(int playerId) {
    return Response(); //TBD
}


Response Matchmaking::leaveGame(int playerId, string* gameId, string* ownerId) {
    Response response = Response();
    ostringstream stream;

    response.setAction(Actions::getActionText(Actions::LEAVE_GAME));
    string player_ID = to_string(playerId);
    auto res = DBConnection::instance().ExecParams("SELECT * FROM CONNECTIONS JOIN GAMES ON CONNECTIONS.game_id=GAMES.id WHERE player_id=$1", { player_ID });
    if (res.row_count() == 0)
        throw MatchmakingException(Response::NOT_FOUND);
        
    string game_id = res.field_by_name(0, "game_id");
    string owner_id = res.field_by_name(0, "owner_id");
    if (ownerId)
        *ownerId = owner_id;
    if (gameId)
        *gameId = game_id;
    auto players = DBConnection::instance().ExecParams("SELECT player_id FROM CONNECTIONS WHERE game_id=$1", { game_id });

    if (stoi(owner_id) == playerId) {
        DBConnection::instance().ExecParams("DELETE FROM games WHERE id=$1", { game_id });
        Response(NULL, Response::HOST_LEAVE_LOBBY, Actions::getActionText(Actions::LEAVE_GAME)).toJson().stringify(stream);
    }
    else {
        DBConnection::instance().ExecParams("DELETE FROM connections WHERE player_id=$1", { player_ID });
        DBConnection::instance().ExecParams("UPDATE games SET curnumplayers = curnumplayers - 1 WHERE id=$1", { game_id });
    }
    response.setResult(Response::OK);
    return response;
}

Response Matchmaking::getLobbyInfo(int playerId) {
    Response response = Response();
    GameInfoData *gameInfoData = new GameInfoData();
    string owner_id = to_string(playerId);
    string game_id;
    vector<string> playersNames;

    auto game = DBConnection::instance().ExecParams("SELECT * FROM GAMES JOIN USERS on GAMES.owner_id=USERS.id WHERE GAMES.id=(SELECT game_id FROM CONNECTIONS WHERE player_id=$1)", { to_string(playerId) });
    if (game.row_count() == 0)
        throw MatchmakingException(Response::HAVE_NO_LOBBY);
    game_id = game.field_by_name(0, "id");
    auto players = DBConnection::instance().ExecParams("SELECT name FROM CONNECTIONS JOIN USERS ON CONNECTIONS.player_id=USERS.id WHERE game_id=$1;", { game_id });

    for (auto player = players.begin(); player != players.end(); ++player) {
        playersNames.push_back((*player).field_by_name("name"));
    }
    LobbyInfo* lobbyInfo = new LobbyInfo(
        game.field_by_name(0, "name"),
        stoi(game.field_by_name(0,"maxNumPlayers")),
        Mode(),
        Map(),
        stoi(game.field_by_name(0, "id")),
        UserInfo(
            stoi(game.field_by_name(0, "owner_id")), 
            game.field_by_name(0, "name")
        ),
        stoi(game.field_by_name(0, "curNumPlayers")),
        playersNames
    );
    response.setData(lobbyInfo);
    response.setResult(Response::OK);
    response.setAction(Actions::getActionText(Actions::GET_LOBBY_INFO));
    return response;
}

void Matchmaking::DeleteGame(int gameId) {
    Response response = Response();
    try {
        DBConnection::instance().ExecParams("DELETE FROM games WHERE id=$1", { std::to_string(gameId) });
    }
    catch (ConnectionException &e) {
        cout << "Error: " << e.what() << endl;
    }
}

Response Matchmaking::HandleAction(string text) {
    try {
        Parser parser;
        Poco::Dynamic::Var result = parser.parse(text);
        Object::Ptr object = result.extract<Object::Ptr>();
        std::string action = object->get("action").convert<std::string>();
        auto params = object->getArray("params");
        switch (Actions::getActionByName(action)) {
        case Actions::GET_GAMES:
            return getGames();
        case Actions::CREATE_GAME:
            return onCreateGame(params);
            break;
        case Actions::JOIN_TO_GAME:
            return onJoinGame(params);
            break;
        case Actions::LEAVE_GAME:
            return onLeaveGame(params);
            break;
        case Actions::GET_LOBBY_INFO:
            return onGetLobbyInfo(params);
            break;
        default:
            return Response(NULL, Response::BADREQUEST, action);
        }
    }
    catch (Poco::JSON::JSONException &e) {
        return Response(NULL, Response::BADREQUEST, "");
    }
    catch (...) {
        return Response(NULL, Response::INTERNALLERROR, "");
    }
}

Response Matchmaking::onJoinGame(Poco::JSON::Array::Ptr params) {
    try {
        string accessToken = params->getElement<string>(0);
        int gameId = params->getElement<int>(1);
        Response r = Matchmaking::joinToGame(gameId, User(accessToken).getId());
        if (r.getResult() != Response::OK)
            return r;
        ostringstream ostr;
        getLobbyInfo(User(accessToken).getId()).toJson().stringify(ostr);
        string message = ostr.str();
        for (auto player : getPlayersList(gameId)) {
            if (player != to_string(User(accessToken).getId()))
                ConnectionsPoll::instance().sendMessage(message, stoi(player));
        }
        return r;
    }
    catch (MatchmakingException &e) {
        cout << "Error: " << e.what() << endl;
        Response r;
        r.setAction(Actions::getActionText(Actions::JOIN_TO_GAME));
        r.setResult(e.getResult());
        return r;
    }
}

Response Matchmaking::onLeaveGame(Poco::JSON::Array::Ptr params) {
    try {
        ostringstream stream;
        string ownerid;
        string gameId;
        string accessToken = params->getElement<string>(0);
        Response r = Matchmaking::leaveGame(User(accessToken).getId(), &gameId, &ownerid);
        getLobbyInfo(stoi(ownerid)).toJson().stringify(stream);
        for (auto player : getPlayersList(atoi(gameId.c_str()))) {
            ConnectionsPoll::instance().sendMessage(stream.str(), stoi(player));
        }
        return r;
    }
    catch (MatchmakingException &e) {
        cout << "Error: " << e.what() << endl;
        Response r;
        r.setAction(Actions::getActionText(Actions::LEAVE_GAME));
        r.setResult(e.getResult());
        return r;
    }
}

Response Matchmaking::onGetLobbyInfo(Poco::JSON::Array::Ptr params) {
    try {
        string acessToken = params->getElement<string>(0);
        return Matchmaking::getLobbyInfo(User(acessToken).getId());
    }
    catch (MatchmakingException &e) {
        cout << "Error: " << e.what() << endl;
        Response r;
        r.setAction(Actions::getActionText(Actions::GET_LOBBY_INFO));
        r.setResult(e.getResult());
        return r;
    }
}

vector<string> Matchmaking::getPlayersList(int game_id) {
    vector<string> playerList;
    Response response = Response();
    GameInfoData *gameInfoData = new GameInfoData();
    try {
        auto players = DBConnection::instance().ExecParams("SELECT player_id FROM Connections WHERE game_id=$1", { to_string(game_id) });
        for (auto player = players.begin(); player != players.end(); ++player) {
            playerList.push_back((*player).field_by_name("player_id"));
        }
    }
    catch (ConnectionException &e) {
        cout << "Error: " << e.what() << endl;
    }
    return playerList;
}


void Matchmaking::Matchmaking::clearDB() {
    try {
        auto result = DBConnection::instance().ExecParams("DELETE FROM Games", { });
    }
    catch (exception &e) {
        cout << "Error: " << e.what() << endl;
    }
}

Response Matchmaking::onCreateGame(Poco::JSON::Array::Ptr params) {
    try {
        Poco::JSON::Object gameObj = *params->getObject(0);
        string accessToken = params->getElement<string>(1);
        Game gameobj = Game(gameObj);
        int userid = User(accessToken).getId();
        return Matchmaking::createGame(gameobj, userid);
    }
    catch (exception &e) {
        cout << "Error: " << e.what() << endl;
        Response r;
        r.setResult(Response::NOT_CREATED);
        return r;
    }
}

bool Matchmaking::isInGame(int player_id) {
    return DBConnection::instance().ExecParams("SELECT * FROM CONNECTIONS WHERE player_id=$1", { to_string(player_id) }).row_count() != 0;
}

void Matchmaking::onCloseConnection(int id) {
    cout << "Close connection " << id << endl;
    if (isInGame(id))
        leaveGame(id);
}
