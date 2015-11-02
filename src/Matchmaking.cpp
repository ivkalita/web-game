#include "Matchmaking.hpp"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/ParseHandler.h"
#include "Poco/JSON/Stringifier.h"

using namespace std;
using namespace Poco::JSON;


UserInfo::UserInfo(int id, string login, string name){
    mId = id;
    mLogin = login;
    mName = name;
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

Game::Game(Object::Ptr game){
    this->name = game->get("name").toString();
    this->maxNumPlayers = atoi(game->get("maxNumPlayers").toString().c_str());
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

AccessToken::AccessToken(Object::Ptr token){}

int AccessToken::getPlayerId(){
    return playerId;
}

Object AccessToken::toJson(){
    return Object();
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
    obj.set("name", mName);
    return obj;
}

Object GameInfo::toJson(){
    Poco::JSON::Object obj = Poco::JSON::Object();
    obj.set("id", mId);
    obj.set("owner", mOwner.toJson());
    obj.set("curNumPlayers", mCurNumPlayers);
    return obj;
}

Response MathcmakingAPI::GetGames()
{
    return Response();
}

Response MathcmakingAPI::JoinToGame(int gameId, AccessToken accessToken)
{
    return Response();
}

Response MathcmakingAPI::CreateGame(Game game, AccessToken accessToken)
{
    return Response();
}

Response MathcmakingAPI::StartGame(AccessToken accessToken)
{
    return Response();
}

Response MathcmakingAPI::LeaveGame(AccessToken accessToken)
{
    return Response();
}
