#include "CommonObjects.hpp"
#include "Matchmaking.hpp"
#include "WSConnection.hpp"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/ParseHandler.h"
#include <Poco/Dynamic/Var.h>
#include "Poco/JSON/Stringifier.h"

using namespace std;
using namespace Poco::JSON;



const map<string, Actions::ACTIONS> Actions::actions =
{
	{ "CreateGame", CREATE_GAME  },
	{ "GetGames",   GET_GAMES    },
	{ "LeaveGame",  LEAVE_GAME   },
	{ "JoinToGame", JOIN_TO_GAME },
	{ "StartGame",  START_GAME   }
};

Response Matchmaking::GetGames()
{
	DBConnection con = DBConnection::instance();
	Response response = Response();
	GameInfoData *gameInfoData = new GameInfoData();
	BaseConnection::connect(con);
	auto res = con.ExecParams("SELECT * FROM GAMES JOIN PLAYERS ON GAMES.owner_id=PLAYERS.id", {});
	for (auto game = res.begin(); game != res.end(); ++game)
	{
		string host = (*game).field_by_name("owner_id");
		GameInfo gameInfo = GameInfo(
			stoi((*game).field_by_name("id")),
			UserInfo(stoi(host), (*game).field_by_name("login")),
			stoi((*game).field_by_name("curNumPlayers"))
			);
		gameInfoData->addGame(gameInfo);
	}
	response.setData(gameInfoData);
	response.setResult(Response::_OK);
	response.setAction("GetGames");
	con.Disconnect();
	return response;
}

Response Matchmaking::JoinToGame(int gameId, AccessToken accessToken)
{
	DBConnection con = DBConnection::instance();
	Response response = Response();
	BaseConnection::connect(con);
	response.setData(NULL);
	int host, curNumPlayers, maxNumPlayers;

	auto res = con.ExecParams("SELECT * FROM GAMES WHERE id=$1", { to_string(gameId) });
	curNumPlayers = stoi(res.field_by_name(0, "curnumplayers"));
	maxNumPlayers = stoi(res.field_by_name(0, "maxnumplayers"));
	
	host = accessToken.getPlayerId();
	if (curNumPlayers == maxNumPlayers)
	{
		response.setResult(Response::_ERROR);
		return response;
	}
	if (con.ExecParams("SELECT * FROM CONNECTIONS WHERE game_id=$1 and player_id=$2", { to_string(gameId), to_string(host) }).row_count() != 0)
		throw MatchMakingException("almost connected");

	con.ExecParams("INSERT INTO connections (game_id, player_id) VALUES($1, $2)", { to_string(gameId), to_string(host) });
	curNumPlayers++;
	con.ExecParams("UPDATE games SET curnumplayers = curnumplayers + 1 WHERE id=$1", { to_string(gameId) });
	response.setAction("JoinToGame");
	response.setResult(Response::_OK);
	con.Disconnect();
	return response;
}

Response Matchmaking::CreateGame(Game game, AccessToken accessToken)
{
	Response response = Response();
	DBConnection con = DBConnection::instance();
	string id;
	int host = accessToken.getPlayerId();
	BaseConnection::connect(con);
	auto res = con.ExecParams("INSERT INTO GAMES (name, owner_id, maxnumplayers, curNumPlayers) values ($1, $2, $3, $4) RETURNING id", { game.getName(),  std::to_string(host), std::to_string(game.getMaxNumPlayers()), "1" });
	id = res.field_by_name(0, "id");
	con.ExecParams("INSERT INTO connections(game_id, player_id) VALUES($1, $2)", { id, to_string(host) });
	con.Disconnect();
	response.setAction("CreateGame");
	response.setResult(Response::_OK);
	return response;
}

Response Matchmaking::StartGame(AccessToken accessToken)
{
	return Response(); //TBD
}

Response Matchmaking::LeaveGame(AccessToken accessToken)
{
	DBConnection con = DBConnection::instance();
	Response response = Response();
	ostringstream stream;
	BaseConnection::connect(con);
	
	response.setAction("LeaveGame");
	string player_ID = to_string(accessToken.getPlayerId());
	auto res = con.ExecParams("SELECT * FROM CONNECTIONS JOIN GAMES ON CONNECTIONS.game_id=GAMES.id WHERE player_id=$1", { player_ID });
	if (res.row_count() == 0)
		throw MatchMakingException("game not found");
		
	string game_id = res.field_by_name(0, "game_id");
	string ownerid = res.field_by_name(0, "owner_id");
	auto players = con.ExecParams("SELECT player_id FROM CONNECTIONS WHERE game_id=$1", { game_id });
	
	if (stoi(ownerid) == accessToken.getPlayerId()){
		con.ExecParams("DELETE FROM games WHERE id=$1", { game_id });
		Response(NULL, Response::_HOST_LEAVE_LOBBY, "GetLobby").toJson().stringify(stream);
	}else{
		con.ExecParams("DELETE FROM connections WHERE player_id=$1", { player_ID });
		con.ExecParams("UPDATE games SET curnumplayers = curnumplayers - 1 WHERE id=$1", { game_id });
		//send Lobby state
	}
	#ifndef MATCHMAKING_TEST
		for (auto iter = players.begin(); iter != players.end(); ++iter){
			if ((*iter).get(0) != player_ID) {
				// Send a message
			}
		}
	#endif
	response.setResult(Response::_OK);
	return response;
}

void Matchmaking::DeleteGame(int gameId)
{
	DBConnection con = DBConnection::instance();
	Response response = Response();
	try {
		BaseConnection::connect(con);
		con.ExecParams("DELETE FROM games WHERE id=$1", { std::to_string(gameId) });
	}
	catch (ConnectionException &e)
	{
		cout << e.what() << endl;
	}
	con.Disconnect();
}

Response Matchmaking::HandleAction(string text)
{
	Parser parser;
	Poco::Dynamic::Var result = parser.parse(text);
	Object::Ptr object = result.extract<Object::Ptr>();
	std::string action = object->get("action").convert<std::string>();
	cout << "Action: " << action << endl;
	switch (Actions::getActionByName(action))
	{
	case Actions::GET_GAMES:
		return GetGames();
	case Actions::CREATE_GAME:
		return onCreateGame(object->getArray("params"));
		break;
	case Actions::JOIN_TO_GAME:
		return onJoinGame(object->getArray("params"));
		break;
	case Actions::LEAVE_GAME:
		return onLeaveGame(object->getArray("params"));
		break;
	default:
		Response r;
		r.setResult(Response::_ERROR);
		return r;
	}
}

Response Matchmaking::onJoinGame(Poco::JSON::Array::Ptr params)
{
	try {
		string acessTokenObj = params->getElement<string>(0);
		int gameId = params->getElement<int>(1);
		AccessToken at = AccessToken(atoi(acessTokenObj.c_str()));
		Response r = Matchmaking::JoinToGame(gameId, at);
		ostringstream ostr;
		string message = ostr.str();
		for (auto player : getPlayersList(gameId))
		{
			if (player != to_string(at.getPlayerId()))
				ConnectionsPoll::instance().sendMessage(message, player);
		}
		return r;
	}
	catch (MatchMakingException &e)
	{
		cout << "Error: " << e.what() << endl;
		Response r;
		r.setAction("JoinGame");
		r.setResult(Response::_ERROR);
		return r;
	}
	catch (ConnectionException &e)
	{
		cout << e.what() << endl;
		Response r;
		r.setResult(Response::_ERROR);
		return r;
	}
}

Response Matchmaking::onLeaveGame(Poco::JSON::Array::Ptr params)
{
	try {
		string acessTokenObj = params->getElement<string>(0);
		AccessToken at = AccessToken(atoi(acessTokenObj.c_str()));
		return Matchmaking::LeaveGame(at);
	}
	catch (MatchMakingException &e)
	{
		cout << "Error: " << e.what() << endl;
		Response r;
		r.setAction("LeaveGame");
		r.setResult(Response::_ERROR);
		return r;
	}
	catch (ConnectionException &e)
	{
		cout << e.what() << endl;
		Response r;
		r.setResult(Response::_ERROR);
		return r;
	}
}

vector<string> Matchmaking::getPlayersList(int game_id)
{
	vector<string> playerList;
	DBConnection con = DBConnection::instance();
	Response response = Response();
	GameInfoData *gameInfoData = new GameInfoData();
	try {
		BaseConnection::connect(con);
		auto players = con.ExecParams("SELECT player_id FROM Connections WHERE game_id=$1", { to_string(game_id) });
		for (auto player = players.begin(); player != players.end(); ++player)
		{
			playerList.push_back((*player).field_by_name("player_id"));
		}
	}
	catch (ConnectionException &e)
	{
		cout << e.what() << endl;
	}
	return playerList;
}

Response Matchmaking::onCreateGame(Poco::JSON::Array::Ptr params) {
	Poco::JSON::Object gameObj = *params->getObject(0);
	string acessTokenObj = params->getElement<string>(1);
	Game gameobj = Game(gameObj);
	AccessToken at = AccessToken(atoi(acessTokenObj.c_str()));
	return Matchmaking::CreateGame(gameobj, at);
}

bool Matchmaking::isInGame(int player_id)
{
	DBConnection con = DBConnection::instance();
	bool res = con.ExecParams("SELECT * FROM CONNECTIONS WHERE player_id=$1", { to_string(player_id) }).row_count() != 0;
	con.Disconnect();
	return res;
}

void Matchmaking::onCloseConnection(string accessToken)
{
	//temperal
	if (isInGame(atoi(accessToken.c_str())))
		LeaveGame(AccessToken(atoi(accessToken.c_str())));
}

void Matchmaking::CreateConnection(string id, WebSocket& ws)
{
	ConnectionsPoll::instance().addThread(
		id,
		ws,
		Matchmaking::onCloseConnection,
		MatchmakingActionHandler
		);
}