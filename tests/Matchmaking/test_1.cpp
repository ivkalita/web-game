#include "CommonObjects.hpp"
#include "Matchmaking.hpp"
#include "DBConnector.hpp"
#include <gtest/gtest.h>
#include "Poco/JSON/JSON.h"

using namespace Poco::JSON;
string PLAYERS[2];

TEST(Matchmaking, init)
{
	DBConnection con = DBConnection::instance();
	BaseConnection::connect(con);
	PLAYERS[0] = con.ExecParams("INSERT INTO PLAYERS (login, password) values ($1, $2) RETURNING id", { "__SAMPLE__LOGIN__", "___SAMPLE__PASSWOD__"}).field_by_name(0, "id");
	PLAYERS[1] = con.ExecParams("INSERT INTO PLAYERS (login, password) values ($1, $2) RETURNING id", { "__SAMPLE__LOGIN2__", "___SAMPLE__PASSWOD__" }).field_by_name(0, "id");
	con.Disconnect();
}

TEST(Matchmaking, DeleteGame)
{
	DBConnection con = DBConnection::instance();
	BaseConnection::connect(con);

	string gameName = "test_game";
	string host = PLAYERS[0];
	string maxNumPlayers = "5";
	string curNumPalyers = "1";

	auto res = con.ExecParams("INSERT INTO GAMES (name, owner_id, maxnumplayers, curNumPlayers) values ($1, $2, $3, $4)", { gameName,  host, maxNumPlayers, curNumPalyers });
	auto id_res = con.ExecParams("SELECT id FROM GAMES WHERE owner_id=$1", { PLAYERS[0] });
	Matchmaking::DeleteGame(atoi(id_res.field_by_name(0,"id").c_str()));
	int size = con.ExecParams("SELECT id FROM GAMES WHERE id=$1", { id_res.field_by_name(0,"id") }).row_count();
	con.Disconnect();
	ASSERT_EQ(0, size);
}


TEST(Matchmaking, CreateGame)
{
	string playerId = PLAYERS[0];
	DBConnection con = DBConnection::instance();
	BaseConnection::connect(con);

    Object gameJson = Object();
    gameJson.set(Game::P_NAME, "sample_name");
    gameJson.set(Game::P_MAXNUMPLAYERS, 5);
    gameJson.set(Game::P_MAP, "sample_map_name");
    gameJson.set(Game::P_MODE, "sample_mode_name");
    Object accessTokenJson = Object();
    Object data = Object();
    data.set(Game::P_THIS, gameJson);
    data.set(AccessToken::P_THIS, accessTokenJson);

    Game game = Game(gameJson);
    AccessToken tok = AccessToken(atoi(playerId.c_str()));
    Matchmaking::CreateGame(game, tok);
	auto newGame = con.ExecParams("SELECT id FROM GAMES WHERE owner_id=$1", { playerId});
	auto newConnection = con.ExecParams("SELECT id FROM CONNECTIONS WHERE player_id=$1", { playerId });
	ASSERT_EQ(1, newConnection.row_count());
	ASSERT_EQ(1, newGame.row_count());
	Matchmaking::DeleteGame(atoi(newGame.field_by_name(0, "id").c_str()));
}


TEST(Matchmaking, JoinToGame)
{
	string owner_id = PLAYERS[0];
	string player_id = PLAYERS[1];

	DBConnection con = DBConnection::instance();
	BaseConnection::connect(con);

	Object gameJson = Object();
	gameJson.set(Game::P_NAME, "sample_name");
	gameJson.set(Game::P_MAXNUMPLAYERS, 5);
	gameJson.set(Game::P_MAP, "sample_map_name");
	gameJson.set(Game::P_MODE, "sample_mode_name");
	Object accessTokenJson = Object();
	Object data = Object();
	data.set(Game::P_THIS, gameJson);
	data.set(AccessToken::P_THIS, accessTokenJson);

	Game game = Game(gameJson);
	AccessToken player_acc_token = AccessToken(atoi(player_id.c_str()));
	AccessToken owner_acc_token = AccessToken(atoi(owner_id.c_str()));
	Matchmaking::CreateGame(game, owner_acc_token);
	int gameId = atoi(con.ExecParams("SELECT * FROM GAMES WHERE owner_id=$1", { owner_id }).field_by_name(0, "id").c_str());
	Matchmaking::JoinToGame(gameId, player_acc_token);
	auto connection = con.ExecParams("SELECT * FROM CONNECTIONS WHERE game_id=$1 and player_id=$2", { to_string(gameId), owner_id });
	auto newGame = con.ExecParams("SELECT * FROM GAMES WHERE owner_id=$1", { owner_id });
	auto curnumpalyerscount = atoi(newGame.field_by_name(0, "curnumplayers").c_str());

	ASSERT_EQ(2, curnumpalyerscount);
	ASSERT_EQ(1, connection.row_count());

	Matchmaking::DeleteGame(gameId);
}

TEST(Matchmaking, finalize)
{
	DBConnection con = DBConnection::instance();
	BaseConnection::connect(con);
	con.ExecParams("DELETE FROM PLAYERS WHERE id=$1 or id=$2", {PLAYERS[0], PLAYERS[1]});
	con.Disconnect();
}