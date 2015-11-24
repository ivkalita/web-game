#include "Matchmaking.hpp"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/ParseHandler.h"
#include "Poco/JSON/Stringifier.h"

using namespace std;
using namespace Poco::JSON;

const string MathcmakingAPI::OK_STATUS = "Ok";
const string Game::VERBOSE_NAME = "name";
const string Game::VERBOSE_MAXNUMPLAYERS = "maxNumPlayers";
const string Game::VERBOSE_MODE = "mode";
const string Game::VERBOSE_MAP = "map";
const string AccessToken::VERBOSE_THIS = "accessToken";
const string Game::VERBOSE_THIS = "game";


UserInfo::UserInfo(int id, string login){
    mId = id;
    mLogin = login;
};

void GameInfoData::addGame(GameInfo & info){
    games.push_back(info);
}

Object GameInfoData::toJson(){
    Poco::JSON::Object obj = Poco::JSON::Object();
    Poco::JSON::Array array = Poco::JSON::Array();
    for (auto game : games)
    {
        array.add(game.toJson());
    }
    obj.set("game", array);
    return obj;
}

Game::Game(){
}

Game::Game(Object game){
    this->name = game.get("name").toString();
    this->maxNumPlayers = atoi(game.get("maxNumPlayers").toString().c_str());
    // mode and map are undefined 
}

Object Game::toJson(){
    return Object();
}

string Game::getName(){
    return name;
}

int Game::getMaxNumPlayers(){
    return maxNumPlayers;
}

int AccessToken::getPlayerId(){
    return mPlayerId;
}

Response::Response(Data *data, string result, string action){
    mData = data;
    mResult = result;
    mAction = action;
}

Object Response::toJson(){
    Poco::JSON::Object obj = Poco::JSON::Object();
    auto buf = (mData) ? mData->toJson() : NULL;
    obj.set("data", buf);
    obj.set("result", mResult);
    return obj;
}

void Response::setData(Data *data){
    mData = data;
}

void Response::setResult(string result){
    mResult = result;
}

void Response::setAction(string action){
    mAction = action;
}

Object UserInfo::toJson(){
    Poco::JSON::Object obj = Poco::JSON::Object();
    obj.set("id", mId);
    obj.set("login", mLogin);
    return obj;
}

Object GameInfo::toJson(){
    Poco::JSON::Object obj = Poco::JSON::Object();
    obj.set("id", mId);
    obj.set("owner", mOwner.toJson());
    obj.set("curNumPlayers", mCurNumPlayers);
    return obj;
}

Response MathcmakingAPI::GetGames(int player)
{
    DBConnection con = DBConnection::instance();
    Response response = Response();
    GameInfoData *gameInfoData = new GameInfoData();
    try {
        BaseConnsection::connect(con);
		if (player == NULL){
			auto res = con.ExecParams("SELECT * FROM GAMES", {});
			for (auto game : res)
			{
				string host = game.field_by_name("owner_id");
				auto ownerRes = con.ExecParams("SELECT * FROM PLAYERS WHERE id=$1", { host });
				GameInfo gameInfo = GameInfo(
					atoi(game.field_by_name("id").c_str()),
					UserInfo(atoi(host.c_str()), ownerRes.field_by_name(0, "login")),
					atoi(game.field_by_name("curNumPlayers").c_str())
				);
				gameInfoData->addGame(gameInfo);
			}
		}else
		{
			auto res = con.ExecParams("SELECT * FROM GAMES WHERE owner_id=$1", {std::to_string(player)});
			int host = atoi(res.field_by_name(1, "owner_id").c_str());
			auto ownerRes = con.ExecParams("SELECT * FROM PLAYERS WHERE id=$1", { std::to_string(host) });
			GameInfo gameInfo = GameInfo(
				atoi(res.field_by_name(1, "id").c_str()),
				UserInfo(atoi(ownerRes.field_by_name(0, "id").c_str()), ownerRes.field_by_name(0, "login")),
				atoi(res.field_by_name(1, "curNumPlayers").c_str())
			);
			gameInfoData->addGame(gameInfo);
		}
        response.setData(gameInfoData);
        response.setResult(OK_STATUS);
		con.Disconnect();
    }
    catch (ConnectionException &e)
    {
        cout << e.what() << endl;
        response.setResult("ServerError");
		con.Disconnect();
    }
    return response;
}

Response MathcmakingAPI::JoinToGame(int gameId, AccessToken accessToken)
{
    DBConnection con = DBConnection::instance();
    Response response = Response();
    // GameInfoData *gameInfoData = new GameInfoData();
    try {
        BaseConnsection::connect(con);
        auto res = con.ExecParams("SELECT * FROM GAMES WHERE id=$1", { std::to_string(gameId) });
        int curNumPlayers = atoi(res.field_by_name(0, "curnumplayers").c_str());
        int maxNumPlayers = atoi(res.field_by_name(0, "maxnumplayers").c_str());
        response.setData(NULL);
		int host = accessToken.getPlayerId();
        if (curNumPlayers == maxNumPlayers)
        {
            response.setResult("CannotConnect"); // add to api.md
            return response;
        }
        con.ExecParams("INSERT INTO connections (game_id, player_id) VALUES($1, $2)", { std::to_string(gameId), to_string(host) });
        curNumPlayers++;
        con.ExecParams("UPDATE games SET curnumplayers = curnumplayers + 1 WHERE id=$1", { std::to_string(gameId) });
        response.setResult(OK_STATUS);
		con.Disconnect();
    }
    catch (ConnectionException &e)
    {
        cout << e.what() << endl;
        response.setResult("ServerError");
		con.Disconnect();
    }
    return response;
}

Response MathcmakingAPI::CreateGame(Game game, AccessToken accessToken)
{
    Response response = Response();
    DBConnection con = DBConnection::instance();
    response.setData(new Data());
    int host = accessToken.getPlayerId();
    try {
        BaseConnsection::connect(con);
        auto res = con.ExecParams("INSERT INTO GAMES (name, owner_id, maxnumplayers, curNumPlayers) values ($1, $2, $3, $4) RETURNING id", { game.getName(),  std::to_string(host), std::to_string(game.getMaxNumPlayers()), "1" });
		string id = res.field_by_name(0, "id");
        con.ExecParams("INSERT INTO connections(game_id, player_id) VALUES($1, $2)", { id, std::to_string(host) });
    }
    catch (ConnectionException &e)
    {
        cout << e.what() << endl;
        response.setResult("ServerError");
    }
    con.Disconnect();
    response.setResult(OK_STATUS);
    return response;
}

Response MathcmakingAPI::StartGame(AccessToken accessToken)
{
    return Response();
}

Response MathcmakingAPI::LeaveGame(AccessToken accessToken)
{
    DBConnection con = DBConnection::instance();
    Response response = Response();
    try {
        BaseConnsection::connect(con);
        auto res = con.ExecParams("SELECT * FROM connection WHERE player_id=$1", { std::to_string(accessToken.getPlayerId()) });
        if (res.row_count() != 0)
        {
            auto game_id = res.field_by_name(0, "game_id");
            auto ownerid = con.ExecParams("SELECT owner_id FROM games WHERE id=$1", { game_id }).field_by_name(0, "owner_id");
            con.ExecParams("DELETE FROM connection WHERE game_id=$1", { game_id });
            if (atoi(ownerid.c_str()) == accessToken.getPlayerId())
            {
                con.ExecParams("DELETE FROM games WHERE id=$1", { game_id });
            }
            else
            {
                con.ExecParams("UPDATE TABLE games SET curentNumPlayers = curentNumPlayers - 1 WHERE id=$1)", { game_id });
            }
        }
        response.setResult(OK_STATUS);
    }

    catch (ConnectionException &e)
    {
        cout << e.what() << endl;
        response.setResult("ServerError");
    }
    return response;
}

void MathcmakingAPI::DeleteGame(int gameId)
{
	DBConnection con = DBConnection::instance();
	Response response = Response();
	try {
		BaseConnsection::connect(con);
		con.ExecParams("DELETE FROM connections WHERE game_id=$1", { std::to_string(gameId) });
		con.ExecParams("DELETE FROM games WHERE id=$1", { std::to_string(gameId) });
	}catch (ConnectionException &e)
	{
		cout << e.what() << endl;
		response.setResult("ServerError");
	}
	con.Disconnect();
}