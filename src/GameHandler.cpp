#include "GameHandler.hpp"

#include "WebgameServer.hpp"
#include "MyUtils.hpp"

#include "Poco/JSON/JSON.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "Poco/LogStream.h"
#include "Poco/Util/Application.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Random.h"
#include "Poco/NObserver.h"
#include "Poco/AutoPtr.h"
#include "Poco/Net/SocketNotification.h"
#include "Poco/Exception.h"
#include "Poco/Net/NetException.h"
#include "Poco/Buffer.h"

using namespace GameHandler;
using namespace Poco::Net;

static Poco::LogStream& GetLogger() {
    static Poco::LogStream logger(WebgameServer::instance().logger());
    return logger;
}

EventHandler::EventHandler(Poco::Mutex* _send_mutex, Poco::Mutex* _recv_mutex, WebSocket* _socket, SocketReactor* _reactor,
    int _player_id) :

    send_mutex(_send_mutex), recv_mutex(_recv_mutex), socket(_socket), reactor(_reactor), player_id(_player_id)
{
    disconnected = false;

    reactor->addEventHandler(*socket, Poco::NObserver<EventHandler, ReadableNotification>
        (*this, &EventHandler::HandleReadable));

    reactor->addEventHandler(*socket, Poco::NObserver<EventHandler, WritableNotification>
        (*this, &EventHandler::HandleWritable));

    reactor->addEventHandler(*socket, Poco::NObserver<EventHandler, ShutdownNotification>
        (*this, &EventHandler::HandleShutdown));

    reactor->addEventHandler(*socket, Poco::NObserver<EventHandler, ErrorNotification>
        (*this, &EventHandler::HandleError));

    reactor->addEventHandler(*socket, Poco::NObserver<EventHandler, IdleNotification>
        (*this, &EventHandler::Idle));
}

void EventHandler::Unregister() {
    disconnected = true;

    reactor->removeEventHandler(*socket, Poco::NObserver<EventHandler, ReadableNotification>
        (*this, &EventHandler::HandleReadable));

    reactor->removeEventHandler(*socket, Poco::NObserver<EventHandler, WritableNotification>
        (*this, &EventHandler::HandleWritable));

    reactor->removeEventHandler(*socket, Poco::NObserver<EventHandler, ShutdownNotification>
        (*this, &EventHandler::HandleShutdown));

    reactor->removeEventHandler(*socket, Poco::NObserver<EventHandler, ErrorNotification>
        (*this, &EventHandler::HandleError));

    reactor->removeEventHandler(*socket, Poco::NObserver<EventHandler, IdleNotification>
        (*this, &EventHandler::Idle));

}

static int WebsocketReceive(WebSocket& s, Poco::Buffer<char>& buf, int& flags) {
    try {
        return s.receiveFrame(buf, flags);
    }
    catch (const Poco::TimeoutException& e) {
        GetLogger() << "Socket receiveFrame timeout exceeded." << e.displayText() << std::endl;
    }
    catch (const NetException& e) {
        GetLogger() << "Socket receiveFrame exception." << e.displayText() << std::endl;
    }

    return -1;
}

void EventHandler::HandleReadable(const Poco::AutoPtr<ReadableNotification>& n) {
    int flags, received;
    Poco::Buffer<char> buf(100 * 1024);

    received = WebsocketReceive(static_cast<WebSocket>(n->socket()), buf, flags);

    if (received == 0 || (flags & WebSocket::FRAME_OP_BITMASK) == WebSocket::FRAME_OP_CLOSE) {
        GetLogger() << "closed socket: " << n->socket().address().toString() << std::endl;

        Unregister();
    }
    else {
        Poco::Mutex::ScopedLock lock(*recv_mutex);

        recv_buffer.emplace_back(buf.begin(), received);
    }
}

void EventHandler::HandleWritable(const Poco::AutoPtr<WritableNotification>& n) {
    if (send_buffer.size() == 0)
        return;

    Poco::Mutex::ScopedLock lock(*send_mutex);

    WebSocket& s = static_cast<Poco::Net::WebSocket>(n->socket());
    for (const auto& i: send_buffer)
        s.sendFrame(i.c_str(), i.size());

    send_buffer.clear();
}

void EventHandler::HandleShutdown(const Poco::AutoPtr<ShutdownNotification>& n) {
    std::cout << "socket shutdown notification" << std::endl;
}

void EventHandler::HandleError(const Poco::AutoPtr<ErrorNotification>& n) {
    std::cout << "socket error notofication" << std::endl;
}

void EventHandler::Idle(const Poco::AutoPtr<IdleNotification>& n) {
    std::cout << "socket idle notofication" << std::endl;
}

StrList& EventHandler::GetRecvBuf() {
    return recv_buffer;
}

void EventHandler::ClearRecvBuf() {
    recv_buffer.clear();
}

void EventHandler::SendBufferPushBack(std::string& s) {
    send_buffer.push_back(s);
}

bool EventHandler::IsDisconnected() {
    return disconnected;
}

int EventHandler::GetID() {
    return player_id;
}

WebSocket* EventHandler::GetSocket() {
    return socket;
}


Game::Game(int _id) : id(_id), x(.0), y(.0) {}

void Game::RemovePlayer(int id) {
    players.erase(id);
}

int Game::AddPlayer(WebSocket* _socket) {
    int new_id;
    do {
        new_id = randomGen.next();
    } while (players[new_id] != nullptr);

    EventHandler* h = new EventHandler(&send_mutex, &recv_mutex, _socket, &reactor, new_id);

    players[new_id] = h;

    const double radius = 10;
    bool valid = true;
    do {
        x = randomGen.nextDouble() * 1000;
        y = randomGen.nextDouble() * 1000;
        bool valid = true;
        for (auto& p : engine.GetPlanets()) {
            if (std::hypot(p.GetX() - x, p.GetY() - y) <= 2 * (radius + GameEngine::Planet::CLOSE_RANGE)) {
                valid = false;
                break;
            }
        }
    } while (!valid);

    engine.AddPlanet(x, y, radius, 10, new_id);

    return new_id;
}

Poco::Thread& Game::GetOwnThread() {
    return own_thread;
}

int Game::GetID() {
    return id;
}

std::map<int, EventHandler*>& Game::GetPlayers() {
    return players;
}

void Game::run() {
    reactor_thread.setName("Game thread - socket reactor");
    reactor_thread.start(reactor);

    while (1) {
        using namespace Poco::JSON;
        using namespace Poco::Dynamic;

        Parser parser;
        Poco::Thread::current()->sleep(500);
        {
            Poco::Mutex::ScopedLock lock(recv_mutex);

            for (auto& player : players) {
                StrList& buf = player.second->GetRecvBuf();
                if (buf.size() == 0 || player.second->IsDisconnected())
                    continue;

                for (std::string& buf_elem : buf) {
                    try {
                        auto json = parser.parse(buf_elem);
                        Object::Ptr obj = json.extract<Object::Ptr>();
                        Var sender_id = obj->get("sender_id");
                        Var dest_id = obj->get("dest_id");
                        Var num = obj->get("num");

                        if (sender_id == dest_id)
                            throw Poco::Exception("sender_id = dest_id");

                        GameEngine::Planet* sender = engine.GetPlanetsMap()[sender_id];
                        GameEngine::Planet* dest = engine.GetPlanetsMap()[dest_id];
                        if (sender == nullptr || dest == nullptr)
                            throw Poco::Exception("There isn't planet with id: " + (sender == nullptr ? sender_id.toString() : dest_id.toString()));

                        if (engine.GetPlanetsMap()[sender_id]->GetOwner() != player.first)
                            throw Poco::Exception("Planet id: " + sender_id.toString() + " doesn't belong to player id: " + std::to_string(player.first));

                        engine.Launch(num, *sender, *dest);
                    }
                    catch (JSONException& e) {
                        GetLogger() << "Poco json exception: " << e.what() << std::endl << "buf=" << buf_elem << std::endl;
                    }
                    catch (Poco::Exception& e) {
                        GetLogger() << "Poco exc:" << e.what() << std::endl;
                    }
                    catch (std::exception& e) {
                        GetLogger() << "std exc: " << e.what() << std::endl;
                    }
                }

                player.second->ClearRecvBuf();
            }
        }

        engine.Step();

        {
            Poco::Mutex::ScopedLock lock(send_mutex);

            Object json;
            Poco::JSON::Array ships_array;
            for (auto& ship : engine.GetShips()) {
                Object s;
                s.set("x", ship.GetX());
                s.set("y", ship.GetY());
                s.set("owner", ship.GetOwner());
                ships_array.add(s);
            }
            json.set("ships", ships_array);

            Poco::JSON::Array planets_array;
            for (auto& p : engine.GetPlanets()) {
                Object s;
                s.set("x", p.GetX());
                s.set("y", p.GetY());
                s.set("id", p.GetID());
                s.set("owner", p.GetOwner());
                s.set("radius", p.GetRadius());
                s.set("ships_sum", p.ShipCount());
                planets_array.add(s);
            }
            json.set("planets", planets_array);

            std::stringstream str;
            json.stringify(str);
            std::string s = str.str();
            for (auto& player : players) {
                if (player.second->IsDisconnected())
                    continue;

                player.second->SendBufferPushBack(s);
            }
        }
    }
}

int Games::gen_id() {
    return id_generator++;
}

void Games::CreateGame(const RouteMatch& m) {
    int new_id = gen_id();
    Game* g = new Game(new_id);
    games[new_id] = g;
    m.response().send() << MyUtils::SimpleJSON({ "action", "new_game_id", "game_id", std::to_string(new_id) });
}

Game* Games::GetGame(int id) {
    return games[id];
}

void Games::Join(const RouteMatch& m) {
    const std::string& game_id = m.captures().at(std::string("game_id"));
    const std::string& access_token = m.captures().at(std::string("access_token"));

    WebSocket* s = new WebSocket(m.request(), m.response());

    std::string msg;

    Game* game = GetGame(std::stoi(game_id));
    if (game == nullptr) {
        msg = MyUtils::SimpleJSON({ "action", "error", "message", "No game with id=" + game_id });
        s->sendFrame(msg.c_str(), msg.size());
        s->close();
        return;
    }

    int new_id = game->AddPlayer(s);

    msg = MyUtils::SimpleJSON({ "action", "player_id", "player_id", std::to_string(new_id) });
    s->sendFrame(msg.c_str(), msg.size());
}

void Games::Run(const RouteMatch& m) {
    const std::string& game_id = m.captures().at(std::string("game_id"));
    const std::string& access_token = m.captures().at(std::string("access_token"));

    std::string msg;
    Game* game = GetGame(std::stoi(game_id));
    if (game == nullptr) {
        m.response().send() << MyUtils::SimpleJSON({ "action", "error", "message", "No game with id=" + game_id });
        return;
    }

    auto json = MyUtils::SimpleJSON({ "action", "game_run" });
    m.response().send() << json;
    for (auto& i : game->GetPlayers()) {
        i.second->GetSocket()->sendFrame(json.c_str(), json.size());
    }
    Poco::Thread& th = game->GetOwnThread();
    th.setName("game thread");
    th.start(*game);
    //g->run();
}

Games::Games() {
    using namespace std::placeholders;
    Router& r = Router::instance();
    r.registerRoute("/api/join/{game_id}/{access_token}", std::bind(&Games::Join, this, _1));
    r.registerRoute("/api/run/{game_id}/{access_token}", std::bind(&Games::Run, this, _1));
    r.registerRoute("/api/create/{access_token}", std::bind(&Games::CreateGame, this, _1));
}

static Games games;

int Games::id_generator = 0;
