#include "Matchmaking.hpp"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/ParseHandler.h"
#include "Poco/JSON/Stringifier.h"

using namespace std;
using namespace Poco::JSON;


UserInfo::UserInfo(int id, string login, string name) {
    mId = id;
    mLogin = login;
    mName = name;
};

void GameInfoData::addGame(GameInfo & info) {
    games.push_back(info);
}

Object GameInfoData::toJson() {
    return Object();
}

Game::Game(Object::Ptr game) {

}

Object Game::toJson() {
    return Object();
}

string Game::getName() {
    return name;
}

int Game::getMaxNumPlayers() {
    return maxNumPlayers;
}

AccessToken::AccessToken(Object::Ptr token) {}

int AccessToken::getPlayerId() {
    return playerId;
}

Object AccessToken::toJson() {
    return Object();
}

Response::Response(Data *data, string result, string action) {
    mData = data;
    mResult = result;
    mAction = action;
}

Object Response::toJson() {
    return Object();
}

void Response::setData(Data *data) {
    mData = data;
}

void Response::setResult(string result) {
    mResult = result;
}

void Response::setAction(string action) {
    mAction = action;
}

Object UserInfo::toJson() {
    return Object();
}

Object GameInfo::toJson() {
    return Object();
}