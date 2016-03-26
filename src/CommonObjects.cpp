#include "CommonObjects.hpp"
#include "Poco/JSON/JSON.h"

using namespace Poco::JSON;
using namespace std;

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
const string LobbyInfo::P_PLAYERS = "players";


const map<Response::RESULT, string> Response::mResultText =
{
    { OK,                "Ok"                },
    { BADREQUEST,        "Bad request"       },
    { INTERNALLERROR,    "Internal error"    },
    { HOST_LEAVE_LOBBY,  "Host leave lobby"  },
    { HAVE_NO_LOBBY,     "Have no lobby"     },
    { OUT_OF_GAMES,      "Out of games"      },
    { ALREADY_STARTED,   "Already Started"   },
    { NOT_STARTED,       "Not started"       },
    { NOT_CREATED,       "Not created"       },
    { NOT_FOUND,         "Not Found"         },
    { GAME_STARTED,      "Game Started"      },
    { ALREADY_CONNECTED, "AlreadyConnected"  },
    { LOBBY_IS_FULL,     "LobbyIsFull "      }
};

Response::Response() : mData(NULL), mAction("") {}

Response::Response(Response * r)
{
    this->mAction = r->mAction;
    this->mResult = r->mResult;
    this->mData = r->mData;
}

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
    int curNumPlayers
)
    :
    Game(name, maxNumPlyers, mode,map),
    mId(id),
    mOwner(owner),
    mCurNumPlayers(curNumPlayers)
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
    mData =  std::auto_ptr<Data>(data);
    mResult = result;
    mAction = action;
}

Object Response::toJson()
{
    Object obj = Object();
    auto buf = (mData.get()) ? mData.get()->toJson() : NULL;
    obj.set(P_DATA, buf);
    obj.set(P_ACTION, mAction);
    obj.set(P_RESULT, mResultText.at(mResult) );
    return obj;
}

void Response::setData(Data *data)
{
    mData = std::auto_ptr<Data>(data);
}

void Response::setResult(RESULT result)
{
    mResult = result;
}

void Response::setAction(string action)
{
    mAction = action;
}

Response::RESULT Response::getResult()
{ 
    return mResult; 
};

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
    return obj;
}
Object LobbyInfo::toJson()
{
    Object obj = GameInfo::toJson();
    Array array = Array();
    for (auto name : mPlayerNames)
    {
        array.add(name);
    }
    obj.set(P_PLAYERS, array);
    return obj;
}


const std::string Response::getResponseText(RESULT id)
{
    for (auto it = mResultText.begin(); it != mResultText.end(); ++it)
    {
        if (it->first == id)
        {
            return it->second;
        }
    }
}