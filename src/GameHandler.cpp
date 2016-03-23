#include "GameEngine.hpp"
#include "Router.hpp"
#include "WebgameServer.hpp"

#include "Poco/LogStream.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Random.h"
#include "Poco/NObserver.h"
#include "Poco/Net/SocketNotification.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"

#include <fstream>

namespace {
    using namespace Poco::Net;

    const long game_refresh_interval = 50;

    class EventHandler {
    private:
        std::string send_buffer, recv_buffer;
        Poco::Mutex* send_mutex, *recv_mutex;
        WebSocket* socket;
        SocketReactor* reactor;
        int player_id;
        bool disconnected;
    public:
        EventHandler(Poco::Mutex* _send_mutex, Poco::Mutex* _recv_mutex, WebSocket* _socket, SocketReactor* _reactor,
            int _player_id);
        void Unregister();
        void HandleReadable(const Poco::AutoPtr<ReadableNotification>& n);
        void HandleWritable(const Poco::AutoPtr<WritableNotification>& n);
        void HandleShutdown(const Poco::AutoPtr<ShutdownNotification>& n);
        void HandleError(const Poco::AutoPtr<ErrorNotification>& n);
        void HandleIdle(const Poco::AutoPtr<IdleNotification>& n);

        std::string& GetRecvBuf() { return recv_buffer; }
        void ClearRecvBuf() { recv_buffer.clear(); }
        void SetSendBuffer(std::string& s) { send_buffer = s; }
        bool IsDisconnected() { return disconnected; }
        int GetID() { return player_id; }
        WebSocket* GetSocket() { return socket; }
    };

    class Game : public Poco::Runnable {
    private:
        struct PlanetInfo {
            int x, y, radius, owner, ships_count;
        };

        struct MapInfo {
            int players_count;
            int width, heigth;
            std::vector<PlanetInfo> planets;
        };

        GameEngine::Engine engine;
        Poco::Random randomGen;
        Poco::Net::SocketReactor reactor;
        Poco::Mutex send_mutex, recv_mutex;
        Poco::Thread reactor_thread, own_thread;
        std::map<int, EventHandler*> players;
        int id;
        MapInfo map;
        bool is_running;
    public:
        Game(int _id, const std::string & map_name);
        void RemovePlayer(int id) { players.erase(id); }
        int AddPlayer(WebSocket* _socket);
        Poco::Thread& GetOwnThread() { return own_thread; }
        int GetID() { return id; }
        int GetMaxPlayersCount() { return map.players_count; }
        bool IsRunning() { return is_running; }
        std::map<int, EventHandler*>& GetPlayers() { return players; }
        void run() override;
    };

    std::string SimpleJSON(std::initializer_list<std::string> values) {
        Poco::JSON::Object json;
        if (values.size() % 2 != 0)
            throw Poco::Exception("SimpleJSON: initializer list length must be even");
    
        for (auto i = values.begin(); i != values.end(); i += 2) {
            json.set(*i, *(i + 1));
        }
        std::stringstream o;
        json.stringify(o);
        return o.str();
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
            (*this, &EventHandler::HandleIdle));
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
            (*this, &EventHandler::HandleIdle));
    }

    void EventHandler::HandleReadable(const Poco::AutoPtr<ReadableNotification>& n) {
        char buf[1024];
        int flags, received;
        received = static_cast<WebSocket>(n->socket()).receiveFrame(buf, 1024, flags);
        if (received == 0 || (flags & WebSocket::FRAME_OP_BITMASK) == WebSocket::FRAME_OP_CLOSE) {
            WebgameServer::instance().logger().information("closed socket: " + n->socket().address().toString());
            Unregister();
        } else {
            Poco::Mutex::ScopedLock lock(*recv_mutex);
            recv_buffer = std::string(buf, received);
        }
    }

    void EventHandler::HandleWritable(const Poco::AutoPtr<WritableNotification>& n) {
        if (send_buffer.size() == 0)
            return;
        Poco::Mutex::ScopedLock lock(*send_mutex);
        static_cast<Poco::Net::WebSocket>(n->socket()).sendFrame(send_buffer.c_str(), (int)send_buffer.size());
        send_buffer.clear();
    }

    void EventHandler::HandleShutdown(const Poco::AutoPtr<ShutdownNotification>& n) {
        std::cout << "socket shutdown notification" << std::endl;
    }

    void EventHandler::HandleError(const Poco::AutoPtr<ErrorNotification>& n) {
        std::cout << "socket error notofication" << std::endl;
    }

    void EventHandler::HandleIdle(const Poco::AutoPtr<IdleNotification>& n) {
        std::cout << "socket idle notofication" << std::endl;
    }

    Game::Game(int _id, const std::string & map_name) : id(_id), is_running(false) {
        std::string filename = Poco::Util::Application::instance().config().getString("application.rootpath")
            + "assets/maps/" + map_name + ".json";
        Poco::JSON::Object::Ptr _map, _planet;
        Poco::JSON::Array::Ptr _planets;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var _file;

        std::ifstream in(filename);
        _file = parser.parse(in);
        _map = _file.extract<Poco::JSON::Object::Ptr>();

        map.players_count = _map->get("playersCount").extract<int>();
        map.width = _map->get("width").extract<int>();
        map.heigth = _map->get("heigth").extract<int>();

        _planets = _map->get("planets").extract<Poco::JSON::Array::Ptr>();
        for (int i = 0; i < _planets->size(); i++) {
            _planet = _planets->getObject(i);
            PlanetInfo p;
            p.x = _planet->get("x").convert<int>();
            p.y = _planet->get("y").convert<int>();
            p.radius = _planet->get("radius").convert<int>();
            p.owner = _planet->get("owner").convert<int>();
            p.ships_count = _planet->get("shipsCount").convert<int>();
            map.planets.push_back(p);
        }
    }

    int Game::AddPlayer(WebSocket* _socket) {
        int new_id;
        do {
            new_id = randomGen.next();
        } while (new_id != 0 && players[new_id] != nullptr);

        EventHandler* h = new EventHandler(&send_mutex, &recv_mutex, _socket, &reactor, new_id);
        players[new_id] = h;
        return new_id;
    }

    void Game::run() {
        reactor_thread.setName("Game thread - socket reactor");
        reactor_thread.start(reactor);

        int i = 0;
        std::vector<int> map_players(map.players_count);
        for (auto player : players)
            map_players[i++] = player.first;
        for (auto pl : map.planets)
            engine.AddPlanet(pl.x, pl.y, pl.radius, pl.ships_count, pl.owner ? map_players[pl.owner - 1] : 0);

        is_running = true;

        while (1) {
            using namespace Poco::JSON;
            using namespace Poco::Dynamic;

            Parser parser;
            Poco::Thread::current()->sleep(game_refresh_interval);
            {
                Poco::Mutex::ScopedLock lock(recv_mutex);
                Poco::LogStream logger(WebgameServer::instance().logger());

                for (auto& player : players) {
                    std::string& buf = player.second->GetRecvBuf();
                    if (buf.size() == 0 || player.second->IsDisconnected())
                        continue;

                    try {
                        auto json = parser.parse(buf);
                        Object::Ptr obj = json.extract<Object::Ptr>();
                        Var sender_id = obj->get("sender_id");
                        Var dest_id = obj->get("dest_id");
                        Var num = obj->get("num");

                        if (sender_id == dest_id)
                            throw Poco::Exception("sender_id = dest_id");

                        GameEngine::Planet* sender = engine.GetPlanetsMap().find(sender_id)->second;
                        GameEngine::Planet* dest = engine.GetPlanetsMap().find(dest_id)->second;
                        if (sender == nullptr || dest == nullptr)
                            throw Poco::Exception("There isn't planet with id: " + (sender == nullptr ? sender_id.toString() : dest_id.toString()));

                        if (engine.GetPlanetsMap().find(sender_id)->second->GetOwner() != player.first)
                            throw Poco::Exception("Planet id: " + sender_id.toString() + " doesn't belong to player id: " + std::to_string(player.first));

                        engine.Launch(num, *sender, *dest);
                    }
                    catch (JSONException& e) {
                        logger << "Poco json exception: " << e.what() << std::endl << "buf=" << buf << std::endl;
                    }
                    catch (Poco::Exception& e) {
                        logger << e.what() << std::endl;
                    }
                    catch (std::exception& e) {
                        logger << "std exc: " << e.what() << std::endl;
                    }

                    player.second->ClearRecvBuf();
                }
            }

            engine.Step();

            {
                Poco::Mutex::ScopedLock lock(send_mutex);

                Object json;
                Poco::JSON::Array ships_array;
                for (auto& group : engine.GetGroups()) {
                    for (auto &ship : group.GetShips()) {
                        Object s;
                        s.set("x", ship.GetX());
                        s.set("y", ship.GetY());
                        s.set("owner", ship.GetOwner());
                        s.set("angle", ship.GetAngle());
                        ships_array.add(s);
                    }
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
                    player.second->SetSendBuffer(s);
                }
            }
        }
    }

    int id_generator = 0;
    std::map<int, Game*> games;

    void create(const RouteMatch& m) {
        const std::string & map_name = m.captures().at(std::string("map_name"));

        int new_id = id_generator++;
        Game* g = new Game(new_id, map_name);
        games[new_id] = g;
        m.response().send() << SimpleJSON({ "action", "new_game_id", "game_id", std::to_string(new_id) });
    }

    void join(const RouteMatch& m) {
        const std::string& game_id = m.captures().at(std::string("game_id"));
        const std::string& access_token = m.captures().at(std::string("access_token"));

        WebSocket* s = new WebSocket(m.request(), m.response());

        Game* game = games[std::stoi(game_id)];

        std::string msg;
        bool f = false;
        if (f = game == nullptr)
            msg = "No game with id=" + game_id;
        else if (f = game->GetPlayers().size() >= game->GetMaxPlayersCount())
            msg = "No empty roles in game with id=" + game_id;
        else if (f = game->IsRunning())
            msg = "Can't join to running game with id=" + game_id;
        else
            msg = std::to_string(game->AddPlayer(s));

        std::string r(SimpleJSON({ "action", f ? "error" : "player_id", f ? "message" : "player_id", msg }));
        s->sendFrame(r.c_str(), (int)r.size());
        if (f) s->close();
    }

    void run(const RouteMatch& m) {
        const std::string& game_id = m.captures().at(std::string("game_id"));
        const std::string& access_token = m.captures().at(std::string("access_token"));

        std::string msg;
        Game* game = games[std::stoi(game_id)];
        if (game == nullptr) {
            m.response().send() << SimpleJSON({ "action", "error", "message", "No game with id=" + game_id });
            return;
        }

        auto json = SimpleJSON({ "action", "game_run" });
        m.response().send() << json;
        for (auto& i : game->GetPlayers()) {
            i.second->GetSocket()->sendFrame(json.c_str(), (int)json.size());
        }
        Poco::Thread& th = game->GetOwnThread();
        th.setName("game thread");
        th.start(*game);
        //g->run();
    }

    class Pages {
    public:
        Pages() {
            auto & router = Router::instance();
            router.registerRoute("/api/join/{game_id}/{access_token}", join);
            router.registerRoute("/api/run/{game_id}/{access_token}", run);
            router.registerRoute("/api/create/{map_name}/{access_token}", create);
        }
    };

    Pages pages;
}
