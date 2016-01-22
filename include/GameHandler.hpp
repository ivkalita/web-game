#ifndef GAME_HANDLER_HPP_INCLUDED
#define GAME_HANDLER_HPP_INCLUDED

#include "GameEngine.hpp"
#include "Router.hpp"

#include "Poco/Net/SocketNotification.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Mutex.h"
#include "Poco/Thread.h"
#include "Poco/AutoPtr.h"
#include "Poco/Random.h"

namespace GameHandler {

    class EventHandler {
    private:
        std::string send_buffer, recv_buffer;
        Poco::Mutex* send_mutex, *recv_mutex;
        Poco::Net::WebSocket* socket;
        Poco::Net::SocketReactor* reactor;
        int player_id;
        bool disconnected;
    public:
        EventHandler(Poco::Mutex* _send_mutex, Poco::Mutex* _recv_mutex,
            Poco::Net::WebSocket* _socket, Poco::Net::SocketReactor* _reactor, int _player_id);

        void Unregister();

        void HandleReadable(const Poco::AutoPtr<Poco::Net::ReadableNotification>& n);

        void HandleWritable(const Poco::AutoPtr<Poco::Net::WritableNotification>& n);

        void HandleShutdown(const Poco::AutoPtr<Poco::Net::ShutdownNotification>& n);

        void HandleError(const Poco::AutoPtr<Poco::Net::ErrorNotification>& n);

        void Idle(const Poco::AutoPtr<Poco::Net::IdleNotification>& n);

        std::string& GetRecvBuf();

        void ClearRecvBuf();

        void SetSendBuffer(std::string& s);

        bool IsDisconnected();

        int GetID();

        Poco::Net::WebSocket* GetSocket();
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
        Game(int _id);

        void RemovePlayer(int id);

        int AddPlayer(Poco::Net::WebSocket* _socket);

        Poco::Thread& GetOwnThread();

        int GetID();

        std::map<int, EventHandler*>& GetPlayers();

        void run() override;
    };


    class Games {
    private:
        std::map<int, Game*> games;

        static int id_generator;
        static int gen_id();

    public:
        void CreateGame(const RouteMatch& m);

        Game* GetGame(int id);

        void Join(const RouteMatch& m);

        void Run(const RouteMatch& m);

        Games();
    };
};


#endif
