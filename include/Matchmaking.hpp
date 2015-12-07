#pragma once
#include<iostream>
#include<String>
#include<vector>
#include<Map>
#include "DBConnector.hpp"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/ParseHandler.h"
#include "Poco/JSON/Stringifier.h"
#include "Poco/Net/WebSocket.h"
#include "WSConnection.hpp"
#include "CommonObjects.hpp"

using namespace std;
using namespace Poco::JSON;

class Matchmaking
{
public:
	static Response GetGames();
	static Response JoinToGame(int gameId, AccessToken accessToken);
	static Response CreateGame(Game game, AccessToken accessToken);
	static Response StartGame(AccessToken accessToken);
	static Response LeaveGame(AccessToken accessToken);
	static Response GetLobby(int player_id);

	static Response HandleAction(string text);

	static Response onJoinGame(Poco::JSON::Array::Ptr params);
	static Response onCreateGame(Poco::JSON::Array::Ptr params);
	static Response onLeaveGame(Poco::JSON::Array::Ptr params);
	static Response onGetLobby(Poco::JSON::Array::Ptr params);

	static vector<string> getPlayersList(int game_id);
	static void DeleteGame(int gameId);
	static void onCloseConnection(string accessToken);
	static bool isInGame(int player_id);
	static void CreateConnection(string id, WebSocket& ws);
};

static class Actions
{
public:
	static enum ACTIONS{
		CREATE_GAME,
		GET_GAMES,
		LEAVE_GAME,
		JOIN_TO_GAME,
		START_GAME,
		GET_LOBBY
	};

	static const int getActionByName(const string name) {
		return actions.at(name);
	}
private:
	static const std::map<string, ACTIONS> actions;
};


static void MatchmakingActionHandler(string& action, ostringstream& stream)
{
	Response r = Matchmaking::HandleAction(action);
	r.toJson().stringify(stream);
}


class MatchMakingException : std::exception
{
private:
	string msg;
public:
	MatchMakingException(const string m) :msg(m) {}
	virtual const char* what() const throw()
	{
		return msg.c_str();
	};
};
