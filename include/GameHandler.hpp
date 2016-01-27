#pragma once

#include "GameEngine.hpp"
#include "Router.hpp"
#include "MyUtils.hpp"
#include "WebgameServer.hpp"

#include "Poco/LogStream.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Random.h"
#include "Poco/NObserver.h"
#include "Poco/AutoPtr.h"
#include "Poco/Net/SocketNotification.h"
#include "Poco/JSON/JSON.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "Poco/Mutex.h"
#include <list>
#include <sstream>
#include <string>

namespace GameHandler {
    using namespace Poco::Net;

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
        void Idle(const Poco::AutoPtr<IdleNotification>& n);

        std::string& GetRecvBuf() { return recv_buffer; }
        void ClearRecvBuf() { recv_buffer.clear(); }
        void SetSendBuffer(std::string& s) { send_buffer = s; }
        bool IsDisconnected() { return disconnected; }
        int GetID() { return player_id; }
        WebSocket* GetSocket() { return socket; }
    };

    class Game : public Poco::Runnable {
    private:
        GameEngine::Engine engine;
        Poco::Random randomGen;
        Poco::Net::SocketReactor reactor;
        Poco::Mutex send_mutex, recv_mutex;
        Poco::Thread reactor_thread, own_thread;
        std::map<int, EventHandler*> players;
        int id;
        double x, y;
    public:
        Game(int _id) : id(_id), x(.0), y(.0) {}
        void RemovePlayer(int id) { players.erase(id); }
        int AddPlayer(WebSocket* _socket);
        Poco::Thread& GetOwnThread() { return own_thread; }
        int GetID() { return id; }
        std::map<int, EventHandler*>& GetPlayers() { return players; }
        void run() override;
    };
};
