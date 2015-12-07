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
	static void DeleteGame(int gameId);
};

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
