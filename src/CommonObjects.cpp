#include "CommonObjects.hpp"
#include "Poco/JSON/JSON.h"

using namespace Poco::JSON;

const string Game::P_NAME = "name";
const string Game::P_MAXNUMPLAYERS = "maxNumPlayers";
const string Game::P_MODE = "mode";
const string Game::P_MAP = "map";
const string Game::P_THIS = "game";

const string AccessToken::P_THIS = "accessToken";

const string GameInfoData::P_GAME = "game";

const string Response::P_DATA ="data";
const string Response::P_ACTION = "action";
const string Response::P_RESULT = "result";


const string UserInfo::P_ID = "id";
const string UserInfo::P_LOGIN = "login";


const string GameInfo::P_ID = "id";
const string GameInfo::P_CURNUMPLAYERS = "curNumPlayers";
const string GameInfo::P_OWNER ="owner";
const string GameInfo::P_PLAYERS = "players";


const map<Response::RESULT, string> Response::mResultText =
{
    { _OK,    "Ok"    },
    { _ERROR, "ERROR" },
    { _HOST_LEAVE_LOBBY, "HOST_LEAVE_LOBBY"}
};


Response::Response() : mData(NULL), mAction("") {};

Object AccessToken::toJson()
{ 
    return Object();  //TBD
};
GameInfo::GameInfo(
    string name,
    int maxNumPlyers,
    Mode mode,
    Map map,
    int id,
    UserInfo& owner,
    int curNumPlayers, 
    vector<string> players
)
    : 
    Game(name, maxNumPlyers, mode,map),
    mId(id),
    mOwner(owner),
    mCurNumPlayers(curNumPlayers),
    mPlayerNames(players)
{};

UserInfo::UserInfo(int id, string login)
{
    mId = id;
    mLogin = login;
};

void GameInfoData::addGame(GameInfo & info)
{
    games.push_back(info);
}

Object GameInfoData::toJson()
{
    Object obj = Object();
    Array array = Array();
    for (auto game : games)
    {
        array.add(game.toJson());
    }
    obj.set(P_GAME, array);
    return obj;
}

Game::Game() {}

Game::Game(Object game)
{
    mName = game.get(P_NAME).toString();
    mMaxNumPlayers = abs(atoi(game.get(P_MAXNUMPLAYERS).toString().c_str()));
    // mode and map are undefined 
}

Object Game::toJson()
{
    return Object();
}

string Game::getName()
{
    return mName;
}

int Game::getMaxNumPlayers()
{
    return mMaxNumPlayers;
}

int AccessToken::getPlayerId()
{
    return mPlayerId;
}

Response::Response(Data *data, RESULT result, string action)
{
    mData = data;
    mResult = result;
    mAction = action;
}

Object Response::toJson()
{
    Object obj = Object();
    auto buf = (mData) ? mData->toJson() : NULL;
    obj.set(P_DATA, buf);
    obj.set(P_ACTION, mAction);
    obj.set(P_RESULT, mResultText.at(mResult) );
    return obj;
}

void Response::setData(Data *data)
{
    mData = data;
}

void Response::setResult(RESULT result)
{
    mResult = result;
}

void Response::setAction(string action)
{
    mAction = action;
}

Object UserInfo::toJson()
{
    Object obj = Object();
    obj.set(P_ID, mId);
    obj.set(P_LOGIN, mLogin);
    return obj;
}

Object GameInfo::toJson()
{

    Object obj = Object();
    obj.set(P_ID, mId);
    obj.set(P_OWNER, mOwner.toJson());
    obj.set(P_CURNUMPLAYERS, mCurNumPlayers);
    obj.set(P_MAXNUMPLAYERS, mMaxNumPlayers);
    obj.set(P_NAME, mName);

    Array array = Array();
    for (auto name : mPlayerNames)
    {
        array.add(name);
    }
    obj.set(P_PLAYERS, array);
    return obj;
}