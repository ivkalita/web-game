#pragma once

#include<iostream>
#include<String>
#include<vector>
#include<Map>
#include "DBConnector.hpp"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/ParseHandler.h"
#include "Poco/JSON/Stringifier.h"

using namespace std;
using namespace Poco;
using namespace Poco::JSON;

#define MATCHMAKING_DEBUG

class MObject
{
public:
    virtual  Object toJson() = 0;
};

class Data : MObject {
public:
    virtual  Object toJson() { return NULL; };
    Data() {};
};

class Response : MObject {
public:
    Response() {};
    Response(Data *data, string result, string action);
    void setData(Data *data);
    void setResult(string result);
    void setAction(string action);
    Object toJson();
private:
    string mResult;
    Data *mData;
    string mAction;
};

class UserInfo : MObject {
public:
    virtual  Object toJson();
    UserInfo(int id, string login);
private:
    int mId;
    string mLogin;
    string mName;
};

class Mode {};
class Map {};

class Game : MObject {
public:

	static const string VERBOSE_NAME;
	static const string VERBOSE_MAXNUMPLAYERS;
	static const string VERBOSE_MODE;
	static const string VERBOSE_MAP;
	static const string VERBOSE_THIS;

    Game();
    Game(Object game);
    virtual  Object toJson();
    string getName();
    int getMaxNumPlayers();

private:
    string name;
    int maxNumPlayers;
    Mode mode;
    Map map;
};

class BaseConnsection
{
public:
    static void connect(DBConnection &conn)
    {
        conn.Connect("127.0.0.1", "5432", "galcon", "postgres", "475976q");
    }
};

class AccessToken : MObject {
public:
	static const string VERBOSE_THIS;
	AccessToken(Object token){};
    int getPlayerId();
	virtual Object toJson() { return Object(); };

#ifdef MATCHMAKING_DEBUG
	AccessToken(int playerId) { mPlayerId = playerId; };
#endif

private:
    int mPlayerId;
};

class GameInfo : Game {
private:
    int mId;
    UserInfo mOwner;
    int mCurNumPlayers;
public:
    GameInfo(int id, UserInfo& owner, int curNumPlayers) : mId(id), mOwner(owner), mCurNumPlayers(curNumPlayers) {};
    virtual Object toJson();
};

class GameInfoData : public Data {
private:
    std::vector<GameInfo> games;
public:
    void addGame(GameInfo& info);
    virtual Object toJson();
};

class UserInfoData : public Data {
public:
    UserInfo user;
};


class MathcmakingAPI
{
public:
    static const string OK_STATUS;
    static Response GetGames(int player=NULL);
    static Response JoinToGame(int gameId, AccessToken accessToken);
    static Response CreateGame(Game game, AccessToken accessToken);
    static Response StartGame(AccessToken accessToken);
    static Response LeaveGame(AccessToken accessToken);
	static void DeleteGame(int gameId);
};