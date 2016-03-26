#pragma once
#include<iostream>
#include<String>
#include<vector>
#include<Map>
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/ParseHandler.h"
#include "Poco/JSON/Stringifier.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Util/IniFileConfiguration.h"

#define MATCHMAKING_DEBUG

class BaseObject
{
public:
    virtual  Poco::JSON::Object toJson() = 0;
    virtual std::string  toJsonString(){
        std::ostringstream ostr;
        this->toJson().stringify(ostr);
        return ostr.str();
    }
};

class Data :BaseObject
{
public:
    virtual  Poco::JSON::Object toJson()
    {
        return this->toJson();
    };
    Data() {};
    virtual ~Data() {};
};

class Response :BaseObject
{
public:
    enum RESULT{
        OK,
        BADREQUEST,
        INTERNALLERROR,
        NOT_FOUND,
        OUT_OF_GAMES,
        HOST_LEAVE_LOBBY,
        HAVE_NO_LOBBY,
        LOBBY_IS_FULL,
        NOT_STARTED,
        NOT_CREATED,
        GAME_STARTED,
        ALREADY_STARTED,
        ALREADY_CONNECTED
    };

    static const std::string P_DATA;
    static const std::string P_ACTION;
    static const std::string P_RESULT;

    static const std::string getResponseText(RESULT id);

    Response();
    Response(Response *r);
    Response(Data *data, RESULT result, std::string action);
    void setData(Data *data);
    void setResult(RESULT result);
    void setAction(std::string action);
    RESULT getResult();
    Poco::JSON::Object toJson();

private:
    static const std::map<RESULT, std::string> mResultText;
    RESULT mResult;
    std::auto_ptr<Data> mData;
    std::string mAction;
};

class UserInfo :BaseObject
{
public:
    static const std::string P_ID;
    static const std::string P_LOGIN;

    virtual Poco::JSON::Object toJson();
    UserInfo(int id, std::string login);

private:
    int mId;
    std::string mLogin;
    std::string mName;
};

class Mode {}; //TBD
class Map {}; //TBD

class Game :BaseObject
{
public:
    static const std::string P_NAME;
    static const std::string P_MAXNUMPLAYERS;
    static const std::string P_MODE;
    static const std::string P_MAP;
    static const std::string P_THIS;

    Game();
    Game(
        std::string name,
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
    std::string getName();
    int getMaxNumPlayers();

protected:
    std::string mName;
    int mMaxNumPlayers;
    Mode mMode;
    Map mMap;
};


class AccessToken :BaseObject
{
public:
    static const std::string P_THIS;
    AccessToken(Poco::JSON::Object token) {};
    int getPlayerId();
    virtual Poco::JSON::Object toJson();

private:
    int mPlayerId;
};


class GameInfo : Game
{
public:
    GameInfo(
        std::string name,
        int maxNumPlyers,
        Mode mode,
        Map map,
        int id, 
        UserInfo& owner, 
        int curNumPlayers 
    );
    virtual  Poco::JSON::Object toJson();

private:
    static const std::string P_ID;
    static const std::string P_CURNUMPLAYERS;
    static const std::string P_OWNER;
    static const std::string P_PLAYERS;

    int mId;
    UserInfo mOwner;
    int mCurNumPlayers;
};

class LobbyInfo : public GameInfo, public Data
{
public:
    static const std::string P_PLAYERS;

    LobbyInfo(
        std::string name,
        int maxNumPlyers,
        Mode mode,
        Map map,
        int id,
        UserInfo& owner,
        int curNumPlayers,
        std::vector<std::string> players = {}
    )
    :
    GameInfo(name, maxNumPlyers, mode, map, id, owner, curNumPlayers),
    mPlayerNames(players)
    {};
    virtual ~LobbyInfo() {};
    virtual  Poco::JSON::Object toJson();

private:
    std::vector<std::string> mPlayerNames;
};


class GameInfoData : public Data
{
public:
    static const std::string P_GAME;
    void addGame(GameInfo& info);
    virtual ~GameInfoData() {};
    virtual Poco::JSON::Object toJson();

private:
    std::vector<GameInfo> games;
};

class UserInfoData : public Data
{
public:
    UserInfo user;
    virtual ~UserInfoData() {};
};