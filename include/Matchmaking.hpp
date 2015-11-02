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
    UserInfo(int id, string login, string name);
private:
    int mId;
    string mLogin;
    string mName;
};

class Mode {};
class Map {};

class Game : MObject {
public:
    Game();
    Game(Object::Ptr game);
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
    AccessToken(Object::Ptr token);
    int getPlayerId();
    virtual Object toJson();
private:
    int playerId;
};

class GameInfo : public Game {
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
    static Response GetGames();
    static Response JoinToGame(int gameId, AccessToken accessToken);
    static Response CreateGame(Game game, AccessToken accessToken);
    static Response StartGame(AccessToken accessToken);
    static Response LeaveGame(AccessToken accessToken);
};
