#pragma once
#ifndef COMMONOBJECTS_H_INCLUDED
#define COMMONOBJECTS_H_INCLUDED
#include<iostream>
#include<String>
#include<vector>
#include<Map>
#include "DBConnector.hpp"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/ParseHandler.h"
#include "Poco/JSON/Stringifier.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Util/IniFileConfiguration.h"

#define MATCHMAKING_DEBUG

class BaseConnection
{
public:
    static void connect(DBConnection &conn)
    {
        Poco::AutoPtr<Poco::Util::IniFileConfiguration> c = new Poco::Util::IniFileConfiguration("bin/runner.ini");
        conn.Connect(
            c->getString("database.hostaddr"),
            c->getString("database.port"),
            c->getString("database.dbname"),
            c->getString("database.user"),
            c->getString("database.password"));
    }
};

class BaseObject
{
public:
    virtual  Poco::JSON::Object toJson() = 0;
    virtual string  toJsonString(){
        ostringstream ostr;
        this->toJson().stringify(ostr);
        return ostr.str();
    }
};

class Data :BaseObject
{
public:
    virtual  Poco::JSON::Object toJson() = 0;
    Data() {};
};

class Response :BaseObject
{
public:
    enum RESULT{
        _OK,
        _ERROR,
        _HOST_LEAVE_LOBBY
    };
    
    static const string P_DATA;
    static const string P_ACTION;
    static const string P_RESULT;

    Response();
    Response(Data *data, RESULT result, string action);
    void setData(Data *data);
    void setResult(RESULT result);
    void setAction(string action);
    Poco::JSON::Object toJson();
private:
    static const map<RESULT, string> mResultText;
    RESULT mResult;
    Data *mData;
    string mAction;
};

class UserInfo :BaseObject
{
public:

    static const string P_ID;
    static const string P_LOGIN;

    virtual Poco::JSON::Object toJson();
    UserInfo(int id, string login);
private:
    int mId;
    string mLogin;
    string mName;
};

class Mode {}; //TBD
class Map {}; //TBD

class Game :BaseObject
{
public:

    static const string P_NAME;
    static const string P_MAXNUMPLAYERS;
    static const string P_MODE;
    static const string P_MAP;
    static const string P_THIS;

    Game();
    Game(
        string name,
        int maxNumPlyers,
        Mode mode,
        Map map
    )
    :
        mName(name),
        mMaxNumPlayers(maxNumPlyers),
        mMode(mode),
        mMap(map)
    {};
    Game(Poco::JSON::Object game);
    virtual Poco::JSON::Object toJson();
    string getName();
    int getMaxNumPlayers();

protected:
    string mName;
    int mMaxNumPlayers;
    Mode mMode;
    Map mMap;
};


class AccessToken :BaseObject
{
public:
    static const string P_THIS;
    AccessToken(Poco::JSON::Object token){};
    int getPlayerId();
    virtual Poco::JSON::Object toJson();
private:
    int mPlayerId;
};


class GameInfo : Game
{
private:
    static const string P_ID;
    static const string P_CURNUMPLAYERS;
    static const string P_OWNER;
    static const string P_PLAYERS;

    int mId;
    UserInfo mOwner;
    int mCurNumPlayers;

public:
    GameInfo(
        string name,
        int maxNumPlyers,
        Mode mode,
        Map map,
        int id, 
        UserInfo& owner, 
        int curNumPlayers 
    );
    virtual  Poco::JSON::Object toJson();
};

class LobbyInfo : public GameInfo, public Data
{
public:
    static const string P_PLAYERS;

    LobbyInfo(
        string name,
        int maxNumPlyers,
        Mode mode,
        Map map,
        int id,
        UserInfo& owner,
        int curNumPlayers,
        vector<string> players = {}
    )
    :
    GameInfo(name, maxNumPlyers, mode, map, id, owner, curNumPlayers),
    mPlayerNames(players)
    { };
    virtual  Poco::JSON::Object toJson();
private:
    std::vector<string> mPlayerNames;

};


class GameInfoData : public Data
{
private:
    std::vector<GameInfo> games;
public:
    static const string P_GAME;
    void addGame(GameInfo& info);
    virtual Poco::JSON::Object toJson();
};

class UserInfoData : public Data
{
public:
    UserInfo user;
};

#endif