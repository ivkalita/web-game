#include "CommonObjects.hpp"
#include "Matchmaking.hpp"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/ParseHandler.h"
#include <Poco/Dynamic/Var.h>
#include "Poco/JSON/Stringifier.h"

using namespace std;
using namespace Poco::JSON;


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